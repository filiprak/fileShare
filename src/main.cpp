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
#include <exception>
#include <string>


std::string usage = "Program usage: command <iface> <nick>";


int main( int argc, char* argv[] ) {
	spdlog::set_pattern("[%H:%M:%S][thread %t]: %v");
	if (argc < 3) {
		console->error(usage);
		exit(0);
	}

	console->warn("Starting fileShare program..." );

	BlockingQueue< Message* > messageQueue(MESS_QUEUE_SIZE, false);
	Controller controller;
	Listener listener(messageQueue);
	Responder responder(messageQueue);
	responder.setRespondOnlyGreetings(true);

	try {
		// get local ipv4 and broadcast address and save it to Network module
		Network::initMyAddress( argv[1] );
		// save my nick to network module
		Network::setMyNick( argv[2] );
		console->info("Your host IPv4: {} ({})", Network::getMyIpv4Addr(), argv[2] );

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
			while(true);
		}

		// stop threads
		listener.stop();
		br_listenerT.join();
		br_parserT.join();

		responder.stop();
		responderT.join();

		// clear left messages
		listener.clearQueues();


	} catch (const std::exception &e) {
		console->error(  std::string("Exception occured: ") + std::string(e.what())  );
	}

	console->warn("Exciting fileShare program...");
	return 0;
}


