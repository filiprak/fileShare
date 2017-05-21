/*
 * listener.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "listener.h"
#include "network_task.h"
#include "blockingQueue.h"
#include "message.h"

#include <unistd.h>

Listener::Listener(BlockingQueue<Message>& q) : NetworkTask(), mqueue(q) {
}

Listener::~Listener(){
	// TODO Auto-generated destructor stub

}

int Listener::run() {
	listening = true;
	while(listening) {

		printf("listening\n");
		usleep(500000);
	}
	return 0;
}

void Listener::stop() {
	listening = false;
}

void listenerThread(BlockingQueue<Message>& q, Listener& listener) {
	listener.run();
}

