/*
 * controller.h
 *
 *	Controls network task execution and user commands,
 *	central point of the program
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#ifndef LOGIC_CONTROLLER_H_
#define LOGIC_CONTROLLER_H_

#include "message.h"
#include "blockingQueue.h"
#include "listener.h"
#include <thread>
#include <vector>

class Controller {
private:
	vector<std::thread> threads;

public:
	Controller();
	virtual ~Controller();

	void run();
};

/* locally triggered operations (by user commands) handled by Controller */

// greeting thread
void greetingThread();

// add file thread
void addFileThread();

// download file thread
void downloadFileThread();

// lock file thread
void lockFileThread();

// unlock file thread
void unlockFileThread();

// revoke file thread
void revokeFileThread();

// delete file thread
void deleteFileThread();


#endif /* LOGIC_CONTROLLER_H_ */
