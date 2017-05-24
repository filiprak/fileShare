/*
 * listener.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "listener.h"
#include "blockingQueue.h"
#include "datagram.h"
#include "message.h"
#include <unistd.h>
#include <iostream>

Listener::Listener(BlockingQueue< Message* >& q) : messq(q)  {
}

Listener::~Listener(){
}

int Listener::run() {
	std::cout << "running listener" << std::endl;
	network.listenUDP(3, 5, LISTENER_PORT);
	std::cout << "exiting listener" << std::endl;
}

void Listener::parse() {
	std::cout << "running parser" << std::endl;
	BlockingQueue< Datagram* >& qdgrams = network.getUdplisten().getReceivedUdPs();
	parsing = true;
	while( parsing ) {
		Datagram* dgram = qdgrams.take();
		if ( !network.getUdplisten().isListening() ) {
			delete dgram;
			break;
		}
		Message* parsed = parseJSONtoMessage( dgram );
		messq.insert(parsed);
	}
	parsing = false;
	std::cout << "exiting parser" << std::endl;
}

void Listener::stop() {
	network.getUdplisten().stop();
	parsing = false;
	Datagram* poison_pill = new Datagram("pill", "{}", 2);
	network.getUdplisten().getReceivedUdPs().insert(poison_pill);

	//clear message queue
	while( messq.notEmpty() ) {
		delete messq.take();
	}
}

void Listener::clearQueues() {
	network.getUdplisten().clearReceivedUDPs();

	//clear message queue
	while( messq.notEmpty() ) {
		delete messq.take();
	}
}

bool Listener::isListening() {
	return network.getUdplisten().isListening() && parsing;
}

void listenerThread(Listener& listener) {
	listener.run();
}

void parserThread(Listener& listener) {
	listener.parse();
}

