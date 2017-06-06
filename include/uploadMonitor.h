/*
 * uploadMonitor.h
 *
 *  Created on: 06.06.2017
 *      Author: raqu
 */

#ifndef NETWORK_UPLOADMONITOR_H_
#define NETWORK_UPLOADMONITOR_H_

/*
 * downloadMonitor.h
 *
 *  Created on: 02.06.2017
 *      Author: raqu
 */

#include <vector>
#include <mutex>
#include <network.h>
#include <string>

class UploadMonitor {
private:
	std::mutex mux;
	std::vector< std::pair< std::string, Network* > > upldtasks;

public:
	UploadMonitor() {};
	virtual ~UploadMonitor() {};

	bool add(std::string filename, Network* driver) {
		std::unique_lock<std::mutex> lock(mux);
		std::pair< std::string, Network* > t =
				std::pair< std::string, Network* >(filename, driver);
		upldtasks.push_back(t);
		return true;
	}

	bool stop_file(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		for (int i = 0; i < upldtasks.size(); ++i) {
			if ( upldtasks[i].first == filename) {
				upldtasks[i].second->stopUploading();
			}
		}
		return true;
	}

	bool stop_all() {
		std::unique_lock<std::mutex> lock(mux);
		for (int i = 0; i < upldtasks.size(); ++i) {
			upldtasks[i].second->stopUploading();
		}
		return true;
	}

	bool clear_file(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		std::vector< std::pair< std::string, Network* > >::iterator i;
		i = upldtasks.begin();
		while ( i != upldtasks.end()) {
			if ( i->first == filename) {
				upldtasks.erase(i);
			} else ++i;
		}
		return true;
	}

};

// upld monitor class
extern UploadMonitor uloadMonitor;

#endif /* NETWORK_UPLOADMONITOR_H_ */
