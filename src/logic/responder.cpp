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
#include <networkFileList.h>
#include <console.h>

Responder::Responder(BlockingQueue< Message* >& q) : messq(q) {
}

Responder::~Responder() {
	// TODO Auto-generated destructor stub
}

void Responder::run() {
	logger->info("Starting responder...");
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
					logger->warn("Message {} not responded", m->jsonify() );
					break;
			}
		} else {
			// unknown type of message
			logger->warn("Message {} not responded, respond only greetings: {}",
					m->jsonify(), respondOnlyGreetings );
		}

		// if reaches maximum number of allowed running threads
		if (threads.size() >= MAX_RESPONER_THREADS) {
			joinThreads();
			threads.clear();
		}
	}
	running = false;
	logger->info("Stopped responder...");
	joinThreads();
	threads.clear();
}

void Responder::stop() {
	running = false;
	MessageGREETING* poison_pill = new MessageGREETING("0", "0", 0);
	messq.insert(poison_pill);
}

void responseGREETINGThread(MessageGREETING* mess) {
	logger->info("Responding on GREETING message from {}({})", mess->getSenderIpv4(),
			mess->getNick() );
	try {
		Network network;
		// response on greeting
		MessageGREETING response(Network::getMyIpv4Addr(),
				Network::getMyNick(),
				mess->getRespPort());
		network.sendUDP(&response,
						mess->getSenderIpv4().c_str(),
						mess->getRespPort() );
	} catch (const std::exception &e) {
		UI.error("response GREETING: %s", e.what());
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
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
	logger->info("Responding on REQLIST message from {}({})", mess->getSenderIpv4(),
			mess->getNick() );
	try {
		Network network;
		Json::Value jlist = netFileList.jsonify();
		MessageRESPLIST response(Network::getMyIpv4Addr(), Network::getMyNick(), jlist);
		network.broadcastUDP(&response, LISTENER_PORT);
	} catch (const std::exception &e) {
		UI.error("response REQLIST: %s", e.what());
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	delete mess;
}

void responseRESPLISTThread(MessageRESPLIST* mess) {
	logger->info("Received RESPLIST message from {}({})", mess->getSenderIpv4(),
			mess->getNick() );
	try {
		Json::Value jlist = mess->getJlist();
		std::map< std::string, FileInfo > newlist = jsonToFileMap(jlist);
		netFileList.update(newlist);
		logger->info("Network file list UPDATED successfully");

	} catch (const std::exception &e) {
		UI.error("response RESPLIST: %s", e.what());
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
	delete mess;
}

void responseADDFILEThread(MessageADDFILE* mess) {
	try {

	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
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
	try {
		resp.run();
	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
}
