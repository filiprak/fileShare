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

