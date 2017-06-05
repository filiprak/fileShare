/*
 * networkFileList.h
 *
 * List of all files available in network at the moment
 * this list is synchronized beetween peers very often
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#ifndef DATA_NETWORKFILELIST_H_
#define DATA_NETWORKFILELIST_H_

#include <fileinfo.h>
#include <json/json.h>
#include <map>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <utility>
#include "logger.h"

#define WAIT_FOR_UPDATE		10//seconds

class NetworkFileList {
private:
	std::mutex _accessMux;
	std::condition_variable _update;
	std::map<std::string, FileInfo> fileList;

public:
	NetworkFileList() {}
	virtual ~NetworkFileList() {}

	void waitForUpdate() {
		std::unique_lock < std::mutex > lock(_accessMux);
		logger->info("Waiting for network file list update: max {} sec",
				WAIT_FOR_UPDATE);
		_update.wait_for(lock, std::chrono::seconds(WAIT_FOR_UPDATE));
	}

	void update(std::map<std::string, FileInfo>& fileList) {
		std::unique_lock < std::mutex > lock(_accessMux);

		this->fileList.swap(fileList);

		_update.notify_all();
	}

	bool hasFile(std::string filename) {
		waitForUpdate();
		std::unique_lock < std::mutex > lock(_accessMux);
		if ( fileList.count(filename) > 0)
			return true;
		return false;
	}

	bool addFile(FileInfo file) {
		std::unique_lock < std::mutex > lock(_accessMux);
		if (fileList.count(file.getName()) > 0) {
			if (fileList[file.getName()].getAddTime()
					<= file.getAddTime()) {
				return false;
			}
		}
		// insert or overwrite new element
		fileList[file.getName()] = file;
		return true;
	}

	bool deleteFile(std::string name) {
		std::unique_lock < std::mutex > lock(_accessMux);

		if (fileList.count(name) == 1) {
			fileList.erase(name);
			return true;
		}
		return false;
	}

	bool revokeFile(std::string name) {
		std::unique_lock < std::mutex > lock(_accessMux);

		if (fileList.count(name) == 1) {
			(fileList.find(name)->second).revoke();
			return true;
		}
		return false;
	}

	bool lockFile(std::string name) {
		std::unique_lock < std::mutex > lock(_accessMux);

		if (fileList.count(name) == 1) {
			(fileList.find(name)->second).lock();
			return true;
		}
		return false;
	}

	bool unlockFile(std::string name) {
		std::unique_lock < std::mutex > lock(_accessMux);

		if (fileList.count(name) == 1) {
			(fileList.find(name)->second).unlock();
			return true;
		}
		return false;
	}

	 std::pair<bool, FileInfo> getFileInfo(std::string filename) {
		std::unique_lock < std::mutex > lock(_accessMux);
		if ( fileList.count(filename) < 1 ) {
			return std::pair<bool, FileInfo>(false, FileInfo());
		}
		return std::pair<bool, FileInfo>(true, fileList[filename]);
	}

	Json::Value jsonify() {
		std::unique_lock < std::mutex > lock(_accessMux);
		Json::Value json;
		json[0] = Json::Value::null;
		int j = 0;
		std::map<std::string, FileInfo>::iterator i;
		if ( !fileList.empty() )
			for (i = fileList.begin(); i != fileList.end(); ++i) {
				json[j] = (i->second).jsonify();
				j++;
			}
		return json;
	}

	std::map<std::string, FileInfo> getFileList() {
		waitForUpdate();
		std::unique_lock < std::mutex > lock(_accessMux);
		return fileList;
	}

	std::map<std::string, FileInfo> getFileList_nowait() {
		std::unique_lock < std::mutex > lock(_accessMux);
		return fileList;
	}
};

// network file list
extern NetworkFileList netFileList;

// parse file info map from json value
std::map<std::string, FileInfo> jsonToFileMap(Json::Value& json);
std::string fileMapToString(std::string& filter, std::map<std::string, FileInfo>& filemap);

#endif /* DATA_NETWORKFILELIST_H_ */
