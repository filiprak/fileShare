/*
 * utilFunctions.cpp
 *
 *  Created on: 22.05.2017
 *      Author: raqu
 */

#include <sys/time.h>
#include "utilFunctions.h"
#include "string.h"

std::string strError(std::string cont, std::string prefix) {
	if (prefix != "")
		prefix += ": ";
	return (prefix + cont);
}

double getCurrentTimeSeconds() {
	struct timeval  tv;
	gettimeofday(&tv, NULL);
	double time = (tv.tv_sec) + (double) (tv.tv_usec) / 1000000.0 ;
	return time;
}

int strip(std::string& str, std::vector< std::string >& result,
		const char* delims) {
	char* cstr = (char*) str.c_str();
	char* pch;
	pch = strtok(cstr, delims);
	while (pch != NULL) {
		result.push_back(std::string(pch));
		pch = strtok(NULL, delims);
	}
	return result.size();
}
