/*
 * downloadMonitor.h
 *
 *  Created on: 02.06.2017
 *      Author: raqu
 */

#ifndef NETWORK_DOWNLOADMONITOR_H_
#define NETWORK_DOWNLOADMONITOR_H_

#include <vector>
#include <mutex>
#include <tcplistener.h>
#include <string>

class DownloadMonitor {
private:
	std::mutex mux;
	std::vector< std::pair< std::string, TCPlistener* > > tcptasks;

public:
	DownloadMonitor() {};
	virtual ~DownloadMonitor() {};

	bool add(std::string filename, TCPlistener* driver) {
		std::unique_lock<std::mutex> lock(mux);
		std::pair< std::string, TCPlistener* > t =
				std::pair< std::string, TCPlistener* >(filename, driver);
		tcptasks.push_back(t);
		return true;
	}

	bool stop_all(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		for (int i = 0; i < tcptasks.size(); ++i) {
			if ( tcptasks[i].first == filename) {
				tcptasks[i].second->stop();
			}
		}
		return true;
	}

	bool stop_downloading() {
		std::unique_lock<std::mutex> lock(mux);
		for (int i = 0; i < tcptasks.size(); ++i) {
			tcptasks[i].second->stop();
		}
		return true;
	}

	bool clear_all(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		std::vector< std::pair< std::string, TCPlistener* > >::iterator i;
		i = tcptasks.begin();
		while ( i != tcptasks.end()) {
			if ( i->first == filename) {
				tcptasks.erase(i);
			} else ++i;
		}
		return true;
	}

	bool isDownloading(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		for (int i = 0; i < tcptasks.size(); ++i) {
			if ( tcptasks[i].first == filename) {
				return true;
			}
		}
		return false;
	}

};

// download monitor class
extern DownloadMonitor dloadMonitor;

#endif /* NETWORK_DOWNLOADMONITOR_H_ */
