/*
 * main.cpp
 *
 *	Program fileShare main start point
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <blockingQueue.h>
#include <common.h>
#include <thread>
#include <controller.h>
#include <listener.h>
#include <logger.h>
#include <message.h>
#include <network.h>
#include <responder.h>
#include <spdlog/details/spdlog_impl.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <exception>
#include <string>
#include <console.h>
#include <dirent.h>
#include "utilFunctions.h"
#include "localFileList.h"


std::string usage = "Program usage: command <iface> <nick>\n";

void initLogger(char* nick) {
	// init logger

	// remove old logs if exist
	std::string logfile = std::string(LOG_FILE_NAME) + "." + std::string(nick) + ".log";
	std::remove(logfile.c_str());

	logger = spdlog::rotating_logger_mt("logger",
			logfile.c_str(), 1048576 * 5, 3);

	spdlog::set_pattern("[%H:%M:%S.%e][thread %t][%l]: %v");
	logger->flush_on(spdlog::level::info);
}

void initLocalDirectory(char* nick)
{
	// assign local directory name
	local_dirname = "/home/" + std::string(getlogin()) + "/" + 
		std::string(LOCAL_FILES_DIRNAME) + "." + std::string(nick);

	// for testing purposes --> assign local file directory in shared folder
	//local_dirname = std::string(LOCAL_FILES_DIRNAME_TESTING) + "." + std::string(nick);

	// try to open local files directory to check if it exists --> if not create it
	DIR* dir = opendir(local_dirname.c_str());
	if (dir)
	{
    	// Directory exists --> nothing to do, just close it
    	closedir(dir);
	}
	else if (ENOENT == errno)
	{
    	// Directory does not exist --> create directory 
    	mkdirectory(local_dirname.c_str());
	}
	else
	{
    	// opendir() failed for some other reason --> report error and close program.
    	logger->warn("Could not open exisiting or create new local files folder.");
    	printf("Could not open exisiting or create new local files folder.");
    	exit(0);
	}
}

int main( int argc, char* argv[] ) {

	if (argc < 3) {
		printf(usage.c_str());
		exit(0);
	}

	initLogger(argv[2]);
	initLocalDirectory(argv[2]);

	logger->warn("Starting fileShare program..." );

	BlockingQueue< Message* > messageQueue(MESS_QUEUE_SIZE, false);
	Controller controller;
	UI.setController(&controller);

	Listener listener(messageQueue);
	Responder responder(messageQueue);
	responder.setRespondOnlyGreetings(true);

	try {
		// get local ipv4 and broadcast address and save it to Network module
		Network::initMyAddress( argv[1] );
	} catch (const std::exception &e) {
		printf("%s", e.what());
		exit(0);
	}

	try {
		// save my nick to network module
		Network::setMyNick( argv[2] );
		logger->info("Your host IPv4: {} ({})", Network::getMyIpv4Addr(), argv[2] );

		// run ncurses ui
		UI.run();

		// run responder thread
		std::thread responderT(responderThread, std::ref(responder) );
		// run listener thread
		std::thread br_listenerT(listenerThread, std::ref(listener) );
		// run listener parser thread
		std::thread br_parserT(parserThread, std::ref(listener) );

		// check for nick duplication
		bool validNick = greetingThread(argv[2]);

		// ensure that listener started
		while ( !listener.isListening() )
			usleep(100);

		//run controller
		if (validNick) {
			responder.setRespondOnlyGreetings(false);

			Network net;
			MessageREQLIST req(Network::getMyIpv4Addr(), Network::getMyNick());
			net.broadcastUDP(&req, LISTENER_PORT);

			// run user interface
			UI.msg("Welcome to fileShare !", "");
			UI.msg("----------------------------------------------<<<", "");
			UI.msg("Your IPv4 address: ", "%s", Network::getMyIpv4Addr());
			UI.msg("Your nick: ", "%s", Network::getMyNick().c_str());
			UI.msg("----------------------------------------------<<<", "");

			while( UI.isRunning() ) {
				UI.inputLoop();
				usleep(5000);
			}
		}
		// destroy ncurses windows
		UI.exit();

		// stop threads
		listener.stop();
		br_listenerT.join();
		br_parserT.join();

		responder.stop();
		responderT.join();

		// clear left messages
		listener.clearQueues();


	} catch (const std::exception &e) {
		logger->error(  std::string("Exception occured: ") + std::string(e.what())  );
		logger->flush();
	}

	logger->warn("Exciting fileShare program...");
	return 0;
}


