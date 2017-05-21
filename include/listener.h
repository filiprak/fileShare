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

#include "network_task.h"
#include "message.h"
#include "blockingQueue.h"

class Listener : public NetworkTask {
private:
	bool listening = false;
	BlockingQueue<Message>& mqueue;

public:
	Listener(BlockingQueue<Message>& q);
	virtual ~Listener();

	int run() override;
	void stop();
};

// listener thread main function
void listenerThread(BlockingQueue<Message>& q, Listener& listener);

#endif /* LOGIC_LISTENER_H_ */
