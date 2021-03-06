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
#include <message.h>
#include "tcplistener.h"

#define COMMAND_ADD_FILE		"add"
#define COMMAND_DEL_FILE		"del"
#define COMMAND_LOCK_FILE		"lock"
#define COMMAND_UNLOCK_FILE		"unlock"
#define COMMAND_REV_FILE		"rev"
#define COMMAND_GET_FILE		"get"
#define COMMAND_SHOW_LIST		"ls"
#define COMMAND_SHOW_LOC_LIST	"lls"
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

// broadcasts list request, may throw exceptions
void sendListRequest();

/* locally triggered operations (by user commands) handled by Controller */

// greeting thread
bool greetingThread(const char* nick);

/* synchronize files on init */
void synchrFiles();

void stopUploading();

// show all files in network list
void showNetworkListThread(std::string filter);

// show all files locally available
void showLocalListThread(std::string filter);

// add file thread
void addFileThread(std::string filaname);

// download file thread
void downloadFileThread(std::string filaname);
// download single file chunk
bool downloadChunkThread(unsigned long offset, unsigned long size, std::string filename,
		MessageRESPFILE* m, TCPlistener& tcplis);

// lock file thread
void lockFileThread(std::string filaname);

// unlock file thread
void unlockFileThread(std::string filaname);

// revoke file thread
void revokeFileThread(std::string filaname);

// delete file thread
void deleteFileThread(std::string filaname);


#endif /* LOGIC_CONTROLLER_H_ */
