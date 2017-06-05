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
#define FILELIST_FORMAT "%20s%c%10s%c%8lu%c%4d%4d%c%30s\n"
#define HEADER_FORMAT "%20s%10s%8s%4s%4s%30s\n"

std::string locFileLtoString(std::string& filter) {
	std::map<std::string, FileInfo> filemap;
	filemap = netFileList.getFileList_nowait();

	std::vector<std::string> loclist;
	loclist = localFileList.getLocalFiles();

	std::string result = "";
	// add header
	char line0[0];
	memset(line0, 0, MAX_LINE_WIDTH);
	snprintf(line0, MAX_LINE_WIDTH, HEADER_FORMAT,"Name","Owner","Size","Blkd","Rev","Add date");
	std::string line_str(line0);

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
		std::string saddTime;
		if(dt != nullptr){
			saddTime=std::string(dt);
		}
		char s = ' ';
		snprintf(line, MAX_LINE_WIDTH, FILELIST_FORMAT,
				f.getName().c_str(),s,
				f.getOwner().c_str(),s,
				f.getSize(),s,
				f.isBlocked(),
				f.isRevoked(),s,
				saddTime.c_str() );

		std::string line_str(line);

		//if (filter != "" && line_str.find(filter) == std::string::npos )
			//continue;
		result += line_str;
	}
	return result;
}


