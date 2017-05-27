/*
 * logger.cpp
 *
 *  Created on: 25.05.2017
 *      Author: raqu
 */

#include "spdlog/spdlog.h"
#include <memory>

/* console logger */

std::shared_ptr<spdlog::logger> logger;
