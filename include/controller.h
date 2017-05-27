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

#include <vector>
#include <thread>

#define COMMAND_ADD_FILE		"add"
#define COMMAND_DEL_FILE		"del"
#define COMMAND_LOCK_FILE		"lock"
#define COMMAND_UNLOCK_FILE		"unlock"
#define COMMAND_REV_FILE		"rev"
#define COMMAND_GET_FILE		"get"
#define COMMAND_SHOW_LIST		"ls"
#define COMMAND_EXIT			"q"


class Controller {
private:
	std::vector<std::thread> threads;

public:
	Controller();
	virtual ~Controller();

	void run();

	void runCommand(std::string command);

	void joinThreads();
};

/* locally triggered operations (by user commands) handled by Controller */

// greeting thread
bool greetingThread(const char* nick);

// show all files list
void showListThread(std::string filter);

// add file thread
void addFileThread(std::string filaname);

// download file thread
void downloadFileThread(std::string filaname);

// lock file thread
void lockFileThread(std::string filaname);

// unlock file thread
void unlockFileThread(std::string filaname);

// revoke file thread
void revokeFileThread(std::string filaname);

// delete file thread
void deleteFileThread(std::string filaname);


#endif /* LOGIC_CONTROLLER_H_ */
