/*
 * main.cpp
 *
 *	Program fileShare main start point
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <iostream>
#include <thread>
#include <unistd.h>
#include <string.h>

#include <stdio.h>

#include "blockingQueue.h"
#include "listener.h"
#include "common.h"
#include "controller.h"
#include "message.h"
#include "responder.h"
#include <json/json.h>

int main() {
	BlockingQueue< Message* > messageQueue(MESS_QUEUE_SIZE, false);
	Controller controller;
	Listener listener(messageQueue);
	Responder responder(messageQueue);

	try {
		std::cout << "Invocating fileShare..." << std::endl;
		// get local ipv4 and broadcast address and save it to Network module
		Network::initMyAddress();

		// run responder thread
		std::thread responderT(responderThread, std::ref(responder) );
		// run listener thread
		std::thread br_listenerT(listenerThread, std::ref(listener) );
		// run listener parser thread
		std::thread br_parserT(parserThread, std::ref(listener) );


		/* test responder*/
		MessageREQFILE* m = new MessageREQFILE("123");
		messageQueue.insert( (Message*) m );

		MessageREQFDATA* m1 = new MessageREQFDATA("123");
		messageQueue.insert( (Message*) m1 );

		MessageGREETING* m3 = new MessageGREETING("123", "host03");
		messageQueue.insert( (Message*) m3 );

		// ensure that listener started
		while ( !listener.isListening() )
			usleep(100);

		//run controller

		listener.stop();
		br_listenerT.join();
		br_parserT.join();

		responder.stop();
		responderT.join();

		// clear left messages
		listener.clearQueues();

	} catch (const std::exception &e) {
		std::cout << std::string("Exception occured: ") + std::string(e.what()) << std::endl;
	}

	return 0;
}


