/*
 * responder.cpp
 *
 *  Created on: 24.05.2017
 *      Author: raqu
 */

#include <common.h>
#include <logger.h>
#include <network.h>
#include <responder.h>
#include <unistd.h>
#include <string>

Responder::Responder(BlockingQueue< Message* >& q) : messq(q) {
}

Responder::~Responder() {
	// TODO Auto-generated destructor stub
}

void Responder::run() {
	console->info("Starting responder...");
	running = true;
	while(running) {
		Message* m = messq.take();
		if(!running) {
			delete m;
			break;
		}
		// strategy
		if (m->getType() == GREETING) {
			threads.push_back( std::thread(responseGREETINGThread,
									(MessageGREETING*) m) );
		} else if (!respondOnlyGreetings) {
			switch ( m->getType() ) {
				case REQLIST:
					threads.push_back( std::thread(responseREQLISTThread,
									   (MessageREQLIST*) m) );
					break;
				case RESPLIST:
					threads.push_back( std::thread(responseRESPLISTThread,
									   (MessageRESPLIST*) m) );
					break;
				case REQFILE:
					threads.push_back( std::thread(responseREQFILEThread,
									   (MessageREQFILE*) m) );
					break;
				case REQFDATA:
					threads.push_back( std::thread(responseREQFDATAThread,
									   (MessageREQFDATA*) m) );
					break;
				case ADDFILE:
					threads.push_back( std::thread(responseADDFILEThread,
									   (MessageADDFILE*) m) );
					break;
				case DELFILE:
					threads.push_back( std::thread(responseDELFILEThread,
									   (MessageDELFILE*) m) );
					break;
				case REVFILE:
					threads.push_back( std::thread(responseREVFILEThread,
									   (MessageREVFILE*) m) );
					break;
				case LOCFILE:
					threads.push_back( std::thread(responseLOCFILEThread,
									   (MessageLOCFILE*) m) );
					break;
				case UNLOCFILE:
					threads.push_back( std::thread(responseUNLOCFILEThread,
									   (MessageUNLOCFILE*) m) );
					break;
				default:
					//ignore other message types
					console->warn("Message {} not responded", m->jsonify() );
					break;
			}
		} else {
			// unknown type of message
			console->warn("Message {} not responded", m->jsonify() );
		}

		// if reaches maximum number of allowed running threads
		if (threads.size() >= MAX_RESPONER_THREADS) {
			joinThreads();
			threads.clear();
		}
	}
	running = false;
	console->info("Stopped responder...");
	joinThreads();
	threads.clear();
}

void Responder::stop() {
	running = false;
	MessageGREETING* poison_pill = new MessageGREETING("0", "0", 0);
	messq.insert(poison_pill);
}

void responseGREETINGThread(MessageGREETING* mess) {
	console->info("Responding on message from {}({})", mess->getSenderIpv4(),
			mess->getSender() );
	Network network;
	// response on greeting
	MessageGREETING response(mess->getSenderIpv4().c_str(),
			Network::getMyNick(),
			mess->getRespPort());
	network.sendUDP(response.jsonify().c_str(),
					mess->getSenderIpv4().c_str(),
					mess->getRespPort() );
	delete mess;
}

void responseREQFILEThread(MessageREQFILE* mess) {
	sleep(1);
	delete mess;
}

void responseREQFDATAThread(MessageREQFDATA* mess) {
	sleep(2);
	delete mess;
}

void responseREQLISTThread(MessageREQLIST* mess) {
	sleep(5);
	delete mess;
}

void responseRESPLISTThread(MessageRESPLIST* mess) {
	sleep(6);
	delete mess;
}

void responseADDFILEThread(MessageADDFILE* mess) {
	sleep(3);
	delete mess;
}

void responseDELFILEThread(MessageDELFILE* mess) {
}

void responseREVFILEThread(MessageREVFILE* mess) {
}

void responseLOCFILEThread(MessageLOCFILE* mess) {
}

void responseUNLOCFILEThread(MessageUNLOCFILE* mess) {
}

void responderThread(Responder& resp) {
	resp.run();
}
