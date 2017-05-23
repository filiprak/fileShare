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

int main() {
	BlockingQueue<Message> messageQueue(MESS_QUEUE_SIZE, false);
	Controller controller;
	Listener listener(messageQueue);

	try {
		std::cout << "Invocating fileShare..." << std::endl;
		// get local ipv4 and broadcast address and save it to Network module
		Network::initMyAddress();

		// run listener thread
		std::thread br_listenerT(listenerThread, std::ref(listener) );
		// run listener parser thread
		std::thread br_parserT(parserThread, std::ref(listener) );

		Message m = messageQueue.take();
		std::cout << m.sender_ipv4 << ", " << m.type << std::endl;

		listener.stop();

		//listener.stop();

		controller.run();

		//join listeners threads
		br_listenerT.join();
		br_parserT.join();

	} catch (const std::exception &e) {
		std::cout << std::string("Exception occured: ") + std::string(e.what()) << std::endl;
	}

	return 0;
}


