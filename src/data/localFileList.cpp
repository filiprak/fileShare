/*
 * localFileList.cpp
 *
 *  Created on: 30.05.2017
 *      Author: raqu
 */

#include "localFileList.h"
#include <map>
#include "fileinfo.h"
#include "networkFileList.h"
#include "console.h"

// local file list: list of
// files currently available on the host
LocalFileList localFileList;
std::string local_dirname;

#define MAX_LINE_WIDTH	128
#define FILELIST_FORMAT "%-20s%-20s%-20lu%-8d%-8d%-15f\n"

std::string locFileLtoString(std::string& filter) {
	std::map<std::string, FileInfo> filemap;
	filemap = netFileList.getFileList_nowait();

	std::vector<std::string> loclist;
	loclist = localFileList.getLocalFiles();

	std::string result = "";
	// add header
	for (int i = 0; i != loclist.size(); ++i) {
		std::string fname = loclist[i];

		if (filemap.count(fname) < 1) {
			UI.error("Inconsistent file lists data...");
			return result;
		}
		FileInfo& f = filemap[fname];

		char line[MAX_LINE_WIDTH];
		memset(line, 0, MAX_LINE_WIDTH);
		snprintf(line, MAX_LINE_WIDTH, FILELIST_FORMAT,
				f.getName().c_str(),
				f.getOwner().c_str(),
				f.getSize(),
				f.isBlocked(),
				f.isRevoked(),
				f.getAddTime() );

		std::string line_str(line);

		//if (filter != "" && line_str.find(filter) == std::string::npos )
			//continue;
		result += line_str;
	}
	return result;
}


