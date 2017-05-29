/*
 * utilFunctions.h
 *
 *  Created on: 22.05.2017
 *      Author: raqu
 */

#ifndef UTILS_UTILFUNCTIONS_H_
#define UTILS_UTILFUNCTIONS_H_

#include <string>
#include <vector>

std::string strError(std::string cont, std::string prefix="");

double getCurrentTimeSeconds();

int strip(std::string& str, std::vector< std::string >& result,
		const char* delims=" \t\n\r");

void mkdirectory(const char* dirname);
long long fsize(const char* filename);
bool fexists(const char* filename);

#endif /* UTILS_UTILFUNCTIONS_H_ */
