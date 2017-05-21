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

#include "blockingQueue.h"
#include "listener.h"
#include "common.h"
#include "controller.h"
#include "message.h"

int main() {
	BlockingQueue<Message> messageQueue(MESS_QUEUE_SIZE, false);
	Controller controller(messageQueue);
	Listener listener(messageQueue);

	// run listener thread
	std::thread listenerT(listenerThread, std::ref(messageQueue), std::ref(listener));


	sleep(3);
	listener.stop();

	controller.run();

	// join listener thread
	listenerT.join();
	return 0;
}


