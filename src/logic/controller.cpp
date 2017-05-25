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

Controller::Controller() {
}

Controller::~Controller() {
}

void Controller::run() {
}

bool greetingThread(const char* nick) {
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
			if ( ((MessageGREETING*) m)->getSender() == std::string(nick) ) {
				console->error("Nick '{} is already used by host: {}", nick, m->getSenderIpv4() );
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
}

void addFileThread() {
}

void downloadFileThread() {
}

void lockFileThread() {
}

void unlockFileThread() {
}

void revokeFileThread() {
}

void deleteFileThread() {
}
