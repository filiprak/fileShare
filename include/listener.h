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

#include "message.h"
#include "blockingQueue.h"
#include "network.h"

class Listener {
private:
	Network network;
	BlockingQueue< Message* >& messq;
	bool parsing = false;

public:
	Listener(BlockingQueue< Message* >& q);
	virtual ~Listener();

	int run();
	void parse();
	bool isListening();
	void stop();
	void clearQueues();
};

void listenerThread(Listener& listener);
void parserThread(Listener& listener);

#endif /* LOGIC_LISTENER_H_ */
