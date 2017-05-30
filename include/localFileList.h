/*
 * localFileList.h
 *
 *	List of files that are existing locally on this host
 *	it could be downloaded files or added by this host
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#ifndef DATA_LOCALFILELIST_H_
#define DATA_LOCALFILELIST_H_

#include <vector>
#include <algorithm>
#include <mutex>

class LocalFileList {
private:
	std::mutex mux;
	std::vector< std::string > local_files;

public:
	LocalFileList() {};
	virtual ~LocalFileList() {};

	bool add(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		if (std::find(local_files.begin(),
				local_files.end(), filename) != local_files.end())
			return false;
		local_files.push_back(filename);
		return true;
	}

	bool remove(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		std::vector<std::string>::iterator elem;
		elem = std::find(local_files.begin(), local_files.end(), filename);
		if (elem == local_files.end())
			return false;
		local_files.erase(elem);
		return true;
	}

	bool contains(std::string filename) {
		std::unique_lock<std::mutex> lock(mux);
		std::vector<std::string>::iterator elem;
		elem = std::find(local_files.begin(), local_files.end(), filename);
		if (elem == local_files.end())
			return false;
		return true;
	}

	std::vector<std::string> getLocalFiles() {
		std::unique_lock<std::mutex> lock(mux);
		return local_files;
	}
};

extern LocalFileList localFileList;

// convert local list to string format
std::string locFileLtoString(std::string& filter);

#endif /* DATA_LOCALFILELIST_H_ */
