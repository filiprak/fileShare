/*
 * controller.cpp
 *
 * Controls connection between components.
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <common.h>
#include <controller.h>
#include <logger.h>
#include <message.h>
#include <network.h>
#include <queue>
#include <string>
#include <future>
#include <console.h>
#include "utilFunctions.h"
#include "networkFileList.h"
#include "localFileList.h"
#include "tcplistener.h"
#include "downloadMonitor.h"

Controller::Controller() {
}

Controller::~Controller() {
}

void Controller::run() {
}

void Controller::runCommand(std::string command) {

	std::vector< std::string > parsed;
	int nrargs = strip(command, parsed);

	if (nrargs < 1) {
		UI.error("Empty command");
		return;

	} else if (parsed[0] == COMMAND_EXIT) {
		UI.info("Waiting for all threads to finish");
		joinThreads();
		UI.stop();

	} else if (parsed[0] == COMMAND_SHOW_LIST) {
		std::string filter = "";
		if (nrargs > 1)
			filter = parsed[1];
		threads.push_back( std::thread(showNetworkListThread, filter) );

	} else if (parsed[0] == COMMAND_SHOW_LOC_LIST) {
		std::string filter = "";
		if (nrargs > 1)
			filter = parsed[1];
		threads.push_back( std::thread(showLocalListThread, filter) );

	} else if (parsed[0] == COMMAND_ADD_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_ADD_FILE);
			return;
		}
		threads.push_back( std::thread(addFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_DEL_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_DEL_FILE);
			return;
		}
		threads.push_back( std::thread(deleteFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_GET_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_GET_FILE);
			return;
		}
		threads.push_back( std::thread(downloadFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_LOCK_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_LOCK_FILE);
			return;
		}
		threads.push_back( std::thread(lockFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_UNLOCK_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_UNLOCK_FILE);
			return;
		}
		threads.push_back( std::thread(unlockFileThread, parsed[1]) );

	} else if (parsed[0] == COMMAND_REV_FILE) {
		if (nrargs < 2) {
			UI.error("Usage: %s <file_name>", COMMAND_REV_FILE);
			return;
		}
		threads.push_back( std::thread(revokeFileThread, parsed[1]) );

	} else {
		UI.error("Unknown command");
	}
}

void Controller::joinThreads() {
	for (unsigned i = 0; i < threads.size(); i++)
		threads[i].join();
}

bool greetingThread(const char* nick) {
	try {
		// broadcast greeting message
		// create greeting listener
		Network greetNet;
		UDPlistener& greetLis = greetNet.getUdplisten();

		greetLis.init(GREETING_TIMEOUT);

		MessageGREETING greet(Network::getMyIpv4Addr(), nick,
						greetLis.getPort() );

		auto future = std::async( &UDPlistener::receiveMessages, &greetLis );
		// broadcast greeting message
		greetNet.broadcastUDP( &greet, LISTENER_PORT );

		std::queue< Message* > recvd = future.get();

		bool validNick = true;
		while( !recvd.empty() ) {
			Message* m = recvd.front();
			recvd.pop();
			if ( m->getType() == GREETING) {
				if ( ((MessageGREETING*) m)->getNick() == std::string(nick) ) {
					UI.error("Nick '%s' is already used by host: %s. \n\nPlease start program again using other nick.", nick, m->getSenderIpv4().c_str());
					logger->error("Nick '{}' is already used by host: {}\tPlease start program again using other nick.", nick, m->getSenderIpv4() );
					validNick = false;
					delete m;
					break;
				}
			}
			delete m;
		}
		// clear
		while( !recvd.empty() ) {
			Message* m = recvd.front();
			recvd.pop();
			delete m;
		}

		return validNick;

	} catch (const std::exception& e) {
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
		return false;
	}

}

void sendListRequest() {
	UI.info("Synchronizing file list... please wait...");

	// check who is alive
	Network network;
	UDPlistener& greetLis = network.getUdplisten();

	greetLis.init(1);

	MessageGREETING greet(Network::getMyIpv4Addr(), Network::getMyNick(),
					greetLis.getPort() );

	auto future = std::async( &UDPlistener::receiveMessage, &greetLis );
	// broadcast greeting message
	network.broadcastUDP( &greet, LISTENER_PORT );

	Message* recvd = future.get();
	// return if did not get any answer
	if (recvd == nullptr)
		return;

	// send list request to first host who answers
	MessageREQLIST reqlist(Network::getMyIpv4Addr(), Network::getMyNick());
	network.sendUDP(&reqlist, recvd->getSenderIpv4().c_str(), LISTENER_PORT);
	delete recvd;
}

void showNetworkListThread(std::string filter) {
	try {

		auto future = std::async( &NetworkFileList::getFileList, &netFileList );
		//send update request
		sendListRequest();
		//wait for list update
		std::map< std::string, FileInfo > fmap = future.get();

		std::string list = fileMapToString(filter, fmap);
		UI.msg("Filelist:\n", "%s", list.c_str());

	} catch (std::exception& e) {
		UI.error("Updating list: %s", e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void showLocalListThread(std::string filter) {
	std::string list = locFileLtoString(filter);
	UI.msg("Files you have locally:\n", "%s", list.c_str());
}

void addFileThread(std::string filename) {
	std::string path = local_dirname + "/" + filename;

	long long file_size = fsize(path.c_str());
	if (file_size == -1) {
		UI.error("File '%s' should be put to '%s/' directory",
				filename.c_str(), LOCAL_FILES_DIRNAME);
		return;
	}
	FileInfo f(filename, Network::getMyNick(), file_size);

	try {
		Network network;
		//send update request
		sendListRequest();
		//check if file exists
		if (!netFileList.hasFile(filename)) {
			f.setAddTime(getCurrentTimeSeconds());
			MessageADDFILE msgadd(Network::getMyIpv4Addr(),
						Network::getMyNick(), f);
			network.broadcastUDP(&msgadd, LISTENER_PORT);

			netFileList.addFile(f);
			localFileList.add(filename);

			UI.info("File '%s' was added successfully", filename.c_str());

		} else
			UI.error("File '%s' already exists", filename.c_str() );

	} catch (std::exception& e) {
		UI.error("Adding file: %s", e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void deleteFileThread(std::string filename) {

	try {
		Network network;

		std::pair<bool, FileInfo> pair = netFileList.getFileInfo(filename);
		FileInfo ftodel = pair.second;
		bool found = pair.first;

		if (found && ftodel.getOwner() == Network::getMyNick()) {
			MessageDELFILE msgdel(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftodel);
			network.broadcastUDP(&msgdel, LISTENER_PORT);

			netFileList.deleteFile(filename);
			localFileList.remove(filename);

			UI.info("File '%s' was deleted from the net", filename.c_str());
		} else if (localFileList.contains(filename)) {
			std::string path = local_dirname + "/" + filename;
			localFileList.remove(filename);
			std::remove(path.c_str());
			UI.info("File '%s' was deleted from local set", filename.c_str());
		} else
			UI.error("File '%s' cannot be deleted", filename.c_str() );

	} catch (std::exception& e) {
		UI.error("Deleting file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void downloadFileThread(std::string filename) {

	try {
		Network network;
		//send update request
		sendListRequest();
		//check if file exists

		std::pair<bool, FileInfo> pair = netFileList.getFileInfo(filename);
		FileInfo f = pair.second;
		bool found = pair.first;

		if (found) {
			if (f.getOwner() == Network::getMyNick()) {
				UI.error("You can't download your file '%s'", filename.c_str() );
				return;
			} else if (localFileList.contains(f.getName())) {
				UI.error("File '%s' is already downloaded", filename.c_str() );
				return;
			}

			UDPlistener& flistener = network.getUdplisten();
			flistener.init(3);
			// determine who can share with file
			MessageREQFILE msgrqf(Network::getMyIpv4Addr(),
						Network::getMyNick(), f.getName(), flistener.getPort());
			//listen for responses
			auto future = std::async( &UDPlistener::receiveMessages, &flistener );
			// broadcast request message
			network.broadcastUDP( &msgrqf, LISTENER_PORT );
			UI.info("Looking for peers to download '%s' file", filename.c_str());

			// get responses
			std::queue< Message* > recvd = future.get();
			// filter from messages other than MessageRESPFILE
			std::vector< MessageRESPFILE* > filt_mess_vect;
			while(!recvd.empty()) {
				Message* m = recvd.front();
				recvd.pop();
				if (m->getType() == RESPFILE)
					filt_mess_vect.push_back((MessageRESPFILE*) m);
				else
					delete m;
			}

			if (filt_mess_vect.empty()) {
				UI.error("File '%s' is not available at the moment", filename.c_str());
				return;
			}

			// cut file into slices if size grater than 10 chunk sizes
			unsigned long whole_size = f.getSize();
			unsigned long fch_size = f.getSize();

			int nr_peers = 1;
			if (f.getSize() > 10*MAX_CHUNK_SIZE) {
				nr_peers = filt_mess_vect.size();
				fch_size = f.getSize() / nr_peers;
			}

			// allocate bool array for results
			std::vector< std::future<bool> > future_vec;
			std::vector< bool > fut_values;
			// array of temp file names
			std::vector< TCPlistener > tcplis;
			std::vector< std::string > temp_fnames;

			unsigned long offset = 0;
			unsigned long size = fch_size;
			unsigned long rest_size = whole_size;

			// create tcp listeners
			for (int i = 0; i < filt_mess_vect.size(); ++i)
				tcplis.push_back(TCPlistener());

			//check if we still can download file
			bool all_downloaded = true;
			if (dloadMonitor.isDownloading(filename)) {
				UI.error("File '%s' is downloading at the moment", filename.c_str() );
				goto clean_up;
			} else if (f.isBlocked() || f.isRevoked()) {
				UI.error("File '%s' is blocked or revoked", filename.c_str() );
				goto clean_up;
			}

			for (int i = 0; i < filt_mess_vect.size(); ++i) {
				MessageRESPFILE* m = filt_mess_vect[i];

				future_vec.push_back( std::async(downloadChunkThread, offset, size,
						filename, m, std::ref(tcplis[i])) );

				offset += size;
				rest_size -= size;
				if (offset >= whole_size) break;
				size = rest_size >= fch_size ? fch_size : rest_size;
				if (rest_size - 1 == size) size = rest_size;
			}

			UI.info("Downloading '%s' file from %d peer(s)",
								filename.c_str(), nr_peers);

			if (future_vec.size() > 0)
				for (int i = 0; i < future_vec.size(); ++i) {
					bool res = future_vec[i].get();
					fut_values.push_back( res );
					logger->info("Downloading file chunk terminated with status: {}",
							fut_values[i]);
					if (!res) {
						all_downloaded = false;
					}
					temp_fnames.push_back(tcplis[i].getTempFile());
				}
			else {
				all_downloaded = false;
				logger->warn("Empty future status values vector: downloading '{}'", filename);
			}

			if (all_downloaded) {
				localFileList.add(filename);
				UI.info("File '%s' was downloaded successfully", filename.c_str());
				// merge temp files
				for (int i = 0; i < temp_fnames.size(); ++i) {
					UI.info("Merging: '%s' chunk of file '%s'",
							temp_fnames[i].c_str(), filename.c_str());
				}
				std::string out_path = local_dirname + "/" + filename;
				if(!mergeChunks(temp_fnames, out_path)) {
					UI.error("While merging chunks of file '%s'", out_path.c_str());
				}

			} else {
				UI.error("File '%s' download failed", filename.c_str());
			}

			clean_up:
			// tell download monitor that downloading terminated
			dloadMonitor.clear_all(filename);

			// clean chunks
			for (int i = 0; i < temp_fnames.size(); ++i)
				std::remove(temp_fnames[i].c_str());

			// clear mess vector
			for (int i = 0; i < filt_mess_vect.size(); ++i)
				delete filt_mess_vect[i];

		} else
			UI.error("File '%s' does not exists", filename.c_str() );
		logger->info("Exciting: {}", __FUNCTION__);

	} catch (std::exception& e) {
		UI.error("Downloading '%s' file: %s", e.what(), filename.c_str());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}

}

bool downloadChunkThread(unsigned long offset, unsigned long size, std::string filename,
		MessageRESPFILE* m, TCPlistener& tcplis) {
	try {
		Network net;
		int res = -1;
		logger->info("Start downloading chunk of file: '{}'<<<<<", filename);
		for (int tries = 0; tries < CHUNK_DOWNLD_TRIES; ++tries) {
			tcplis.init(TCP_ACCEPT_TIMEOUT);
			MessageREQFDATA reqdata(Network::getMyIpv4Addr(),
					Network::getMyNick(),
					filename,
					tcplis.getPort(),
					offset,
					size);
			// signal download menager
			dloadMonitor.add(filename, &tcplis);

			auto future = std::async(&TCPlistener::run,
					&tcplis, TCP_RECV_TIMEOUT, size, std::ref(m->getSenderIpv4()) );

			net.sendUDP(&reqdata, m->getSenderIpv4().c_str(), LISTENER_PORT);
			res = future.get();

			if (res == 0) break;
			logger->info("Try nr: {} downloading chunk of file: '{}'<<<<<", tries+1, filename);
		}
		logger->info("Downloading file chunk: '{}' resulted:{}", tcplis.getTempFile(), res);

		// clean if didnt succeded
		if (res == -1)
			std::remove(tcplis.getTempFile().c_str());

		return (res == 0);

	} catch (std::exception& e) {
		UI.error("Downloading file '%s' chunk: %s", e.what(), filename.c_str());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
		return false;
	}

}

void lockFileThread(std::string filename) {
	try {
		Network network;

		std::pair<bool, FileInfo> pair = netFileList.getFileInfo(filename);
		FileInfo ftolock = pair.second;
		bool found = pair.first;

		if (found && ftolock.getOwner() == Network::getMyNick()) {
			MessageLOCFILE msglock(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftolock.getName());
			network.broadcastUDP(&msglock, LISTENER_PORT);
			netFileList.lockFile(filename);
			UI.info("File '%s' was locked successfully", filename.c_str());
		} else {
			UI.error("File '%s' cannot be locked", filename.c_str() );
		}

	} catch (std::exception& e) {
		UI.error("Locking file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void unlockFileThread(std::string filename) {
	try {
		Network network;

		std::pair<bool, FileInfo> pair = netFileList.getFileInfo(filename);
		FileInfo ftounlck = pair.second;
		bool found = pair.first;

		if (found && ftounlck.getOwner() == Network::getMyNick()) {
			MessageUNLOCFILE msgunlck(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftounlck.getName());
			network.broadcastUDP(&msgunlck, LISTENER_PORT);
			netFileList.unlockFile(filename);
			UI.info("File '%s' was unlocked successfully", filename.c_str());
		} else {
			UI.error("File '%s' cannot be unlocked", filename.c_str() );
		}

	} catch (std::exception& e) {
		UI.error("Unlocking file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

void revokeFileThread(std::string filename) {
	try {
		Network network;

		std::pair<bool, FileInfo> pair = netFileList.getFileInfo(filename);
		FileInfo ftorev = pair.second;
		bool found = pair.first;

		if (found && ftorev.getOwner() == Network::getMyNick()) {
			MessageREVFILE msgrev(Network::getMyIpv4Addr(),
						Network::getMyNick(), ftorev.getName());
			network.broadcastUDP(&msgrev, LISTENER_PORT);
			netFileList.revokeFile(filename);
			UI.info("File '%s' was revoked successfully", filename.c_str());
		} else {
			UI.error("File '%s' cannot be revoked", filename.c_str() );
		}

	} catch (std::exception& e) {
		UI.error("Revoking file '%s': %s", filename.c_str(), e.what());
		logger->error("Exception in: {}: {}", __FUNCTION__, e.what());
	}
}

