/*
 * utilFunctions.cpp
 *
 *  Created on: 22.05.2017
 *      Author: raqu
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utilFunctions.h"
#include <cstring>
#include <fstream>

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

void cpy(const char* from, const char* to) {
	std::ifstream  src(from, std::ios::binary);
	std::ofstream  dst(to,   std::ios::binary);
	dst << src.rdbuf();
}

void mkdirectory(const char* dirname) {
	struct stat st = {0};

	if (stat(dirname, &st) == -1) {
	    mkdir(dirname, 0700);
	}
}

long long fsize(const char* filename) {
    struct stat stat_buf;
    int rc = stat(filename, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool fexists(const char* filename) {
	return access( filename, F_OK ) != -1;
}

