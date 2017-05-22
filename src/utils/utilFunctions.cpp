/*
 * utilFunctions.cpp
 *
 *  Created on: 22.05.2017
 *      Author: raqu
 */

#include "utilFunctions.h"
#include <string>

std::string strError(std::string cont, std::string prefix) {
	if (prefix != "")
		prefix += ": ";
	return (prefix + cont);
}
