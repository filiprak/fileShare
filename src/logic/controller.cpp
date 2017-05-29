/*
 * controller.cpp
 *
 * Controls connection between components.
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <common.h>
#include <controller.h>
#include <logger.h>
#include <message.h>
#include <network.h>
#include <queue>
#include <string>
#include <future>
#include <console.h>
#include "utilFunctions.h"
#include "networkFileList.h"

Controller::Controller() {
}

Controller::~Controller() {
}

void Controller::run() {
}

void Controller::runCommand(std::string command) {

	std::vector< std::string > parsed;
	int nrargs = strip(command, parsed);

	if (nrargs < 1) {
		UI.error("Empty command");
		return;

	} else if (parsed[0] == COMMAND_EXIT) {
		UI.info("Waiting for all threads to finish");
		joinThreads();
		UI.stop();

	} else if (parsed[0] == COMMAND_SHOW_LIST) {
		std::string filter = "";
		if (nrargs > 1)
			filter = parsed[1];
		threads.push_back( std::thread(showListThread, filter) );

	} else if (parsed[0] == COMMAND_ADD_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_ADD_FILE);
			return;
		}
		threads.push_back( std::thread(addFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_DEL_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_DEL_FILE);
			return;
		}
		threads.push_back( std::thread(deleteFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_GET_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_GET_FILE);
			return;
		}
		threads.push_back( std::thread(downloadFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_LOCK_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_LOCK_FILE);
			return;
		}
		threads.push_back( std::thread(lockFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_UNLOCK_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_UNLOCK_FILE);
			return;
		}
		threads.push_back( std::thread(unlockFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_REV_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_REV_FILE);
			return;
		}
		threads.push_back( std::thread(revokeFileThread, parsed[1]) );

	} else {
		UI.error("Unknown command");
	}
}

void Controller::joinThreads() {
	for (unsigned i = 0; i < threads.size(); i++)
		threads[i].join();
}

bool greetingThread(const char* nick) {
	try {
		// broadcast greeting message
		// create greeting listener
		Network greetNet;
		UDPlistener& greetLis = greetNet.getUdplisten();

		greetLis.init(GREETING_TIMEOUT);

		MessageGREETING greet(Network::getMyIpv4Addr(), nick,
						greetLis.getPort() );

		auto future = std::async( &UDPlistener::receiveMessages, &greetLis );
		// broadcast greeting message
		greetNet.broadcastUDP( &greet, LISTENER_PORT );

		std::queue< Message* > recvd = future.get();

		bool validNick = true;
		while( !recvd.empty() ) {
			Message* m = recvd.front();
			recvd.pop();
			if ( m->getType() == GREETING) {
				if ( ((MessageGREETING*) m)->getNick() == std::string(nick) ) {
					UI.error("Nick '%s' is already used by host: %s", nick, m->getSenderIpv4().c_str());
					logger->error("Nick '{}' is already used by host: {}", nick, m->getSenderIpv4() );
					validNick = false;
					delete m;
					break;
				}
			}
			delete m;
		}
		// clear
		while( !recvd.empty() ) {
			Message* m = recvd.front();
			recvd.pop();
			delete m;
		}

		return validNick;

	} catch (const std::exception& e) {
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
		return false;
	}

}

void sendListRequest() {
	UI.info("Synchronizing file list... please wait...");

	// check who is alive
	Network network;
	UDPlistener& greetLis = network.getUdplisten();

	greetLis.init(1);

	MessageGREETING greet(Network::getMyIpv4Addr(), Network::getMyNick(),
					greetLis.getPort() );

	auto future = std::async( &UDPlistener::receiveMessage, &greetLis );
	// broadcast greeting message
	network.broadcastUDP( &greet, LISTENER_PORT );

	Message* recvd = future.get();
	// return if did not get any answer
	if (recvd == nullptr)
		return;

	// send list request to first host who answers
	MessageREQLIST reqlist(Network::getMyIpv4Addr(), Network::getMyNick());
	network.sendUDP(&reqlist, recvd->getSenderIpv4().c_str(), LISTENER_PORT);
	delete recvd;
}

void showListThread(std::string filter) {
	try {

		auto future = std::async( &NetworkFileList::getFileList, &netFileList );
		//send update request
		sendListRequest();
		//wait for list update
		std::map< std::string, FileInfo > fmap = future.get();

		std::string list = fileMapToString(filter, fmap);
		UI.msg("Filelist:\n", "%s", list.c_str());

	} catch (std::exception& e) {
		UI.error("Updating list: %s", e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void addFileThread(std::string filename) {
	FileInfo f(filename, Network::getMyNick(), 344554);

	try {
		Network network;
		//send update request
		sendListRequest();
		//check if file exists
		if (!netFileList.hasFile(filename)) {
			f.setAddTime(getCurrentTimeSeconds());
			MessageADDFILE msgadd(Network::getMyIpv4Addr(),
						Network::getMyNick(), f);
			network.broadcastUDP(&msgadd, LISTENER_PORT);
			netFileList.addFile(f);
			UI.info("File '%s' was added successfully", filename.c_str());

		} else
			UI.error("File '%s' already exists", filename.c_str() );

	} catch (std::exception& e) {
		UI.error("Updating list: %s", e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void deleteFileThread(std::string filename) {

	try {
		Network network;

		bool found;
		FileInfo ftodel = netFileList.getFileInfo(filename, &found);
		if (found && ftodel.getOwner() == Network::getMyNick()) {
			MessageDELFILE msgdel(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftodel);
			network.broadcastUDP(&msgdel, LISTENER_PORT);
			netFileList.deleteFile(filename);
			UI.info("File '%s' was deleted successfully", filename.c_str());
		} else {
			UI.error("File '%s' cannot be deleted", filename.c_str() );
		}

	} catch (std::exception& e) {
		UI.error("Deleting file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void downloadFileThread(std::string filename) {
	UI.warning(__FUNCTION__);
}

void lockFileThread(std::string filename) {
	try {
		Network network;

		bool found;
		FileInfo ftolock = netFileList.getFileInfo(filename, &found);
		if (found && ftolock.getOwner() == Network::getMyNick()) {
			MessageLOCFILE msglock(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftolock.getName());
			network.broadcastUDP(&msglock, LISTENER_PORT);
			netFileList.lockFile(filename);
			UI.info("File '%s' was locked successfully", filename.c_str());
		} else {
			UI.error("File '%s' cannot be locked", filename.c_str() );
		}

	} catch (std::exception& e) {
		UI.error("Locking file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void unlockFileThread(std::string filename) {
	try {
		Network network;

		bool found;
		FileInfo ftounlck = netFileList.getFileInfo(filename, &found);
		if (found && ftounlck.getOwner() == Network::getMyNick()) {
			MessageUNLOCFILE msgunlck(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftounlck.getName());
			network.broadcastUDP(&msgunlck, LISTENER_PORT);
			netFileList.unlockFile(filename);
			UI.info("File '%s' was unlocked successfully", filename.c_str());
		} else {
			UI.error("File '%s' cannot be unlocked", filename.c_str() );
		}

	} catch (std::exception& e) {
		UI.error("Unlocking file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void revokeFileThread(std::string filename) {
	try {
		Network network;

		bool found;
		FileInfo ftorev = netFileList.getFileInfo(filename, &found);
		if (found && ftorev.getOwner() == Network::getMyNick()) {
			MessageREVFILE msgrev(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftorev.getName());
			network.broadcastUDP(&msgrev, LISTENER_PORT);
			netFileList.revokeFile(filename);
			UI.info("File '%s' was revoked successfully", filename.c_str());
		} else {
			UI.error("File '%s' cannot be revoked", filename.c_str() );
		}

	} catch (std::exception& e) {
		UI.error("Revoking file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}
