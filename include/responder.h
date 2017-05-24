/*
 * responder.h
 *
 *	Responder performs incoming messages handling,
 *	runs in his own, separate thread, takes messages from
 *	parsed received messages from message queue
 *
 *  Created on: 24.05.2017
 *      Author: raqu
 */

#ifndef LOGIC_RESPONDER_H_
#define LOGIC_RESPONDER_H_

#include "message.h"
#include "blockingQueue.h"
#include <vector>
#include <thread>

class Responder {
private:
	BlockingQueue< Message >& messq;
	vector<std::thread> threads;
	bool running = false;

public:
	Responder(BlockingQueue< Message >& q);
	virtual ~Responder();

	void run();
	void stop();

	vector<std::thread>& getThreads() {
		return threads;
	}

	void joinThreads() {
		for (unsigned i = 0; i < threads.size(); i++)
			threads[i].join();
	}
};

/* remote triggered operations (by remote peer, not local) handled by Responder*/

// response on greeting
void responseGREETINGThread(MessageGREETING& mess);

// response on file request
void responseREQFILEThread(MessageREQFILE &mess);

// response on file data request
void responseREQFDATAThread(MessageREQFDATA &mess);

// response on file list request
void responseREQLISTThread(MessageREQLIST &mess);

// response on file list received
void responseRESPLISTThread(MessageRESPLIST &mess);

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
// responder thread---------------------------
void responderThread(Responder& resp);

#endif /* LOGIC_RESPONDER_H_ */
