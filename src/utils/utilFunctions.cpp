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
#include <pwd.h>
#include <fcntl.h>
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

bool mkdirectory(const char* dirname) {
	struct stat st = {0};
	bool res = true;

	if (stat(dirname, &st) == -1) {
	    res = (mkdir(dirname, 0777) == 0);
	}
	return res;
}

long long fsize(const char* filename) {
    struct stat stat_buf;
    int rc = stat(filename, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool fexists(const char* filename) {
	return access( filename, F_OK ) != -1;
}

bool mergeChunks(std::vector<std::string> in, std::string out) {
#define CP_BUF_SIZE 	4096

	int outfd = open(out.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (outfd < 0) return false;

	char buf[CP_BUF_SIZE];
	memset(buf, 0, CP_BUF_SIZE);

	for (int i = 0; i < in.size(); ++i) {
		int chunkfd = open(in[i].c_str(), O_RDONLY);
		if (chunkfd < 0) return false;

		int numbytes = read(chunkfd, buf, CP_BUF_SIZE);
		while(numbytes > 0) {
			write(outfd, buf, numbytes);
			memset(buf, 0, CP_BUF_SIZE);
			numbytes = read(chunkfd, buf, CP_BUF_SIZE);
		}
		close(chunkfd);

		// reading error
		if (numbytes < 0) {
			close(outfd);
			return false;
		}
	}

	close(outfd);
	return true;
}

const char *getUserName() {
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if (pw) {
		return pw->pw_name;
	}
	return nullptr;
}

