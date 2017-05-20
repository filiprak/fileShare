/*
 * network_task.h
 *
 *	Abstract class representing single network task,
 *	method "run" should be overriden when inheriting
 *	this class
 *
 *  Created on: 19.05.2017
 *      Author: raqu
 */

#ifndef INCLUDE_NETWORK_TASK_H_
#define INCLUDE_NETWORK_TASK_H_

#include <network.h>

class NetworkTask {
private:
	// network service
	Network network;

public:
	NetworkTask() {};
	virtual ~NetworkTask() {};

	// runs network task
	virtual int run() = 0;

};


#endif /* INCLUDE_NETWORK_TASK_H_ */
