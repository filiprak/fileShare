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

#include <message.h>

class Controller {
public:
	Controller();
	virtual ~Controller();

	void run();
};

/* locally triggered operations (by user commands) */

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


/* remote triggered operations (by remote peer, not local) */

// response on greeting
void responseGREETINGThread(MessageGREETING& mess);

// response on file request
void responseREQUFILEThread(MessageREQFILE &mess);

// response on file data request
void responseREQFDATAThread(MessageREQFDATA &mess);

// response on file list request
void responseREQLISTThread(MessageREQLIST &mess);

// response on file added
void responseADDFILEThread(MessageADDFILE &mess);

// response on file deleted
void responseDELFILEThread(MessageDELFILE &mess);

// response on file revoked
void responseREVFILEThread(MessageREVFILE &mess);

// response on file locked
void responseLOCFILEThread(MessageLOCFILE &mess);

// response on file unlocked
void responseUNLOCFILEThread(MessageUNLOCFILE &mess);


#endif /* LOGIC_CONTROLLER_H_ */
