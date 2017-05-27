/*
 * logger.h
 *
 *  Created on: 25.05.2017
 *      Author: raqu
 */

#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_


#include "spdlog/spdlog.h"
#include <memory>

extern std::shared_ptr<spdlog::logger> logger;

#endif /* INCLUDE_LOGGER_H_ */
