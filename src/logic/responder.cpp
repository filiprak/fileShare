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
#include <localFileList.h>
#include <console.h>
#include <downloadMonitor.h>
#include <uploadMonitor.h>

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
	logger->info("Responding on REQFILE message from {}({})", mess->getSenderIpv4(),
			mess->getNick() );
	try {
		Network network;
		// response if have file locally
		if (localFileList.contains(mess->getRequestedFile())) {
			MessageRESPFILE response(Network::getMyIpv4Addr(),
					Network::getMyNick(), mess->getRequestedFile());
			network.sendUDP( &response, mess->getSenderIpv4().c_str(),
					mess->getRespPort() );
		}

	} catch (const std::exception &e) {
		UI.error("response REQFILE: %s", e.what());
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
	delete mess;
}

void responseREQFDATAThread(MessageREQFDATA* mess) {
	logger->info("Responding on REQFDATA message from {}({})", mess->getSenderIpv4(),
				mess->getNick() );
	int fd;
	Network network;

	try {
		// response if have file locally
		if (localFileList.contains(mess->getRequestedFile())) {
			std::string path = local_dirname +
					"/" + mess->getRequestedFile();

			fd = open(path.c_str(), O_RDONLY);
			if (fd == -1) {
				logger->error("Failed open file '{}' data", mess->getRequestedFile());
			} else { // stream file through tcp
				uloadMonitor.add(mess->getRequestedFile(), &network);
				int res = network.fstreamTCP(fd,
						mess->getOffset(),
						mess->getSize(),
						mess->getSenderIpv4().c_str(),
						mess->getWaitTcpPort(),
						TCP_SEND_TIMEOUT);
				uloadMonitor.clear_file(mess->getRequestedFile());
				close(fd);
				if (res == -1)
					logger->error("Failed to send file '{}' data", mess->getRequestedFile());
				else
					logger->info("Successfully sent file '{}' data", mess->getRequestedFile());
			}
			logger->flush();
		}

	} catch (const std::exception &e) {
		close(fd);
		UI.error("response REQFDATA: %s", e.what());
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
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
		FileInfo f = mess->getAddedFile();
		bool res = netFileList.addFile( f );
		if (!res)
			logger->warn("{}: File: {} not added to network-list",
					__FUNCTION__, f.jsonify().toStyledString() );

	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
	delete mess;
}

void responseDELFILEThread(MessageDELFILE* mess) {
	try {
		FileInfo f = mess->getDeletedFile();
		bool res = netFileList.deleteFile( f.getName() );
		localFileList.remove(f.getName());
		// delete file from hd
		std::string path = local_dirname + "/" + f.getName();
		std::remove(path.c_str());

		// stop all running transfers
		logger->warn("{}: Stopping file transfer: '{}'",
									__FUNCTION__, f.getName() );
		dloadMonitor.stop_all(f.getName());
		dloadMonitor.clear_all(f.getName());

		if (!res)
			logger->warn("{}: File: {} not deleted from network-list",
					__FUNCTION__, f.jsonify().toStyledString() );

	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
	delete mess;
}

void responseREVFILEThread(MessageREVFILE* mess) {
	try {
		std::string f = mess->getRevFile();
		bool res = netFileList.revokeFile( f );
		if (!res)
			logger->warn("{}: File: '{}' not revoked (as requested) on network-list",
					__FUNCTION__, f );

	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
	delete mess;
}

void responseLOCFILEThread(MessageLOCFILE* mess) {
	try {
		std::string f = mess->getLckFile();
		bool res = netFileList.lockFile( f );

		// stop all running transfers
		logger->warn("{}: Stopping file transfer: '{}'",
							__FUNCTION__, f );
		dloadMonitor.stop_all(f);
		dloadMonitor.clear_all(f);

		if (!res)
			logger->warn("{}: File: '{}' not locked (as requested) on network-list",
					__FUNCTION__, f );

	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
	delete mess;
}

void responseUNLOCFILEThread(MessageUNLOCFILE* mess) {
	try {
		std::string f = mess->getUnlckFile();
		bool res = netFileList.unlockFile( f );
		if (!res)
			logger->warn("{}: File: '{}' not unlocked (as requested) on network-list",
					__FUNCTION__, f );

	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
	delete mess;
}

void responderThread(Responder& resp) {
	try {
		resp.run();
	} catch (const std::exception &e) {
		logger->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
	logger->flush();
}
