/*
 * networkFileList.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "networkFileList.h"

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

#define MAX_LINE_WIDTH	90
#define FILELIST_FORMAT "%-20s%-20s%-20lu%-8d%-8d%-15f\n"

std::string fileMapToString(std::string& filter,
		std::map<std::string, FileInfo>& filemap) {
	std::map<std::string, FileInfo>::iterator i;

	std::string result = "";
	// add header
	for (i = filemap.begin(); i != filemap.end(); ++i) {
		FileInfo& f = i->second;
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
	return netFileList.jsonify().toStyledString();
}
