/*
 * listener.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "listener.h"
#include "network_task.h"

Listener::Listener() : NetworkTask() {
	// TODO Auto-generated constructor stub

}

Listener::~Listener(){
	// TODO Auto-generated destructor stub

}

int Listener::run() {

}

void listenerThread() {
	Listener listener;
	listener.run();
}
