/*
 * listener.h
 *
 *	Listens broadcast messages from other hosts in the network
 *	Inserts received messages to message queue
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#ifndef LOGIC_LISTENER_H_
#define LOGIC_LISTENER_H_

#include <network_task.h>

class Listener : public NetworkTask {
public:
	Listener();
	virtual ~Listener();

	int listen();

};

// listener thread main function
void listenerThread();

#endif /* LOGIC_LISTENER_H_ */
