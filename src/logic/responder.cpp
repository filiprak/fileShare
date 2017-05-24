/*
 * responder.cpp
 *
 *  Created on: 24.05.2017
 *      Author: raqu
 */

#include "responder.h"
#include "utilFunctions.h"
#include "network.h"
#include <iostream>
#include <unistd.h>

Responder::Responder(BlockingQueue<Message>& q) : messq(q) {
}

Responder::~Responder() {
	// TODO Auto-generated destructor stub
}

void Responder::run() {
	running = true;
	while(running) {
		Message m = messq.take();
		if(!running) break;
		// strategy
		switch ( m.getType() ) {
			case GREETING:
				threads.push_back( std::thread(responseGREETINGThread,
								   std::ref( (MessageGREETING&) m) ) );
				break;
			case REQLIST:
				threads.push_back( std::thread(responseREQLISTThread,
								   std::ref( (MessageREQLIST&) m) ) );
				break;
			case RESPLIST:
				threads.push_back( std::thread(responseRESPLISTThread,
								   std::ref( (MessageRESPLIST&) m) ) );
				break;
			case REQFILE:
				threads.push_back( std::thread(responseREQFILEThread,
								   std::ref( (MessageREQFILE&) m) ) );
				break;
			case REQFDATA:
				threads.push_back( std::thread(responseREQFDATAThread,
								   std::ref( (MessageREQFDATA&) m) ) );
				break;
			case ADDFILE:
				threads.push_back( std::thread(responseADDFILEThread,
								   std::ref( (MessageADDFILE&) m) ) );
				break;
			case DELFILE:
				threads.push_back( std::thread(responseDELFILEThread,
								   std::ref( (MessageDELFILE&) m) ) );
				break;
			case REVFILE:
				threads.push_back( std::thread(responseREVFILEThread,
								   std::ref( (MessageREVFILE&) m) ) );
				break;
			case LOCFILE:
				threads.push_back( std::thread(responseLOCFILEThread,
								   std::ref( (MessageLOCFILE&) m) ) );
				break;
			case UNLOCFILE:
				threads.push_back( std::thread(responseUNLOCFILEThread,
								   std::ref( (MessageUNLOCFILE&) m) ) );
				break;
			default:
				//ignore other message types
				break;
		}
		// if reaches maximum number of allowed running threads
		if (threads.size() >= MAX_RESPONER_THREADS) {
			std::cout << "threads: " << threads.size() << std::endl;
			joinThreads();
			threads.clear();
		}
	}
	running = false;
	joinThreads();
	threads.clear();
}

void Responder::stop() {
	running = false;
	Message poison_pill(GREETING, "pill");
	messq.insert(poison_pill);
}

void responseGREETINGThread(MessageGREETING& mess) {
	std::cout << __FUNCTION__ << std::endl;

	Network network;


	std::cout << "exit" << __FUNCTION__ << std::endl;
}

void responseREQFILEThread(MessageREQFILE& mess) {
	std::cout << __FUNCTION__ << std::endl;
	sleep(10);
	std::cout << "exit" << __FUNCTION__ << std::endl;
}

void responseREQFDATAThread(MessageREQFDATA& mess) {
	std::cout << __FUNCTION__ << std::endl;
	sleep(20);
	std::cout << "exit" << __FUNCTION__ << std::endl;
}

void responseREQLISTThread(MessageREQLIST& mess) {
	std::cout << __FUNCTION__ << std::endl;
	sleep(5);
	std::cout << "exit" << __FUNCTION__ << std::endl;
}

void responseRESPLISTThread(MessageRESPLIST& mess) {
	std::cout << __FUNCTION__ << std::endl;
	sleep(6);
	std::cout << "exit" << __FUNCTION__ << std::endl;
}

void responseADDFILEThread(MessageADDFILE& mess) {
	std::cout << __FUNCTION__ << std::endl;
	sleep(3);
	std::cout << "exit" << __FUNCTION__ << std::endl;
}

void responseDELFILEThread(MessageDELFILE& mess) {
}

void responseREVFILEThread(MessageREVFILE& mess) {
}

void responseLOCFILEThread(MessageLOCFILE& mess) {
}

void responseUNLOCFILEThread(MessageUNLOCFILE& mess) {
}

void responderThread(Responder& resp) {
	resp.run();
}
