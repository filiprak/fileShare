/*
 * utilFunctions.cpp
 *
 *  Created on: 22.05.2017
 *      Author: raqu
 */

#include <sys/time.h>
#include "utilFunctions.h"

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
