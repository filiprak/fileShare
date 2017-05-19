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

class Listener: public NetworkTask {
public:
	Listener();
	virtual ~Listener();


};

#endif /* LOGIC_LISTENER_H_ */
