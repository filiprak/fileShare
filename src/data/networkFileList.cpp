/*
 * networkFileList.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "networkFileList.h"
#include "common.h"

// define network file list
NetworkFileList netFileList;


std::map<std::string, FileInfo> jsonToFileMap(Json::Value& json) {
	std::map<std::string, FileInfo> map;
	if ( json[0].isNull() ) return map;

	for (int i = 0; i < json.size(); ++i) {
		FileInfo f( json[i] );
		map[ f.getName() ] = f;
	}

	return map;
}

std::string fileMapToString(std::string& filter,
		std::map<std::string, FileInfo>& filemap) {
	std::map<std::string, FileInfo>::iterator i;

	std::string result = "";
	// add header
	char line0[0];
	memset(line0, 0, MAX_LINE_WIDTH);
	snprintf(line0, MAX_LINE_WIDTH, HEADER_FORMAT,"NAME","OWNER","SIZE","LOCK","REV","ADD-DATE");
	std::string header(line0);
	result += header;

	for (i = filemap.begin(); i != filemap.end(); ++i) {
		FileInfo& f = i->second;
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
