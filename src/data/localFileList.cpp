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
#include "common.h"

// local file list: list of
// files currently available on the host
LocalFileList localFileList;
std::string local_dirname;



std::string locFileLtoString(std::string& filter) {
	std::map<std::string, FileInfo> filemap;
	filemap = netFileList.getFileList_nowait();

	std::vector<std::string> loclist;
	loclist = localFileList.getLocalFiles();

	std::string result = "";
	// add header
	char line0[0];
	memset(line0, 0, MAX_LINE_WIDTH);
	snprintf(line0, MAX_LINE_WIDTH, HEADER_FORMAT,"NAME","OWNER","SIZE","LOCK","REV","ADD-DATE");
	std::string header(line0);
	result += header;

	for (int i = 0; i != loclist.size(); ++i) {
		std::string fname = loclist[i];

		if (filemap.count(fname) < 1) {
			UI.error("Inconsistent file lists data...");
			return result;
		}
		FileInfo& f = filemap[fname];

		char line[MAX_LINE_WIDTH];
		memset(line, 0, MAX_LINE_WIDTH);

		time_t addTime = f.getAddTime();
		char* dt = ctime(&addTime);
		if (dt == nullptr)
			dt = (char*) std::string("none").c_str();
		dt[strlen(dt)-1] = '\0';
		char s = ' ';
		snprintf(line, MAX_LINE_WIDTH, FILELIST_FORMAT,
				f.getName().c_str(),s,
				f.getOwner().c_str(),s,
				f.getSize(),s,
				f.isBlocked(),
				f.isRevoked(),s,
				dt );

		std::string line_str(line);

		if (filter != "" && line_str.find(filter) == std::string::npos )
			continue;
		result += line_str;
	}
	return result;
}


