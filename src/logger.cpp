/*
 * logger.cpp
 *
 *  Created on: 25.05.2017
 *      Author: raqu
 */

#include "spdlog/spdlog.h"
#include <memory>

/* console logger */
std::shared_ptr<spdlog::logger> console = spdlog::stdout_color_mt("console");

//std::shared_ptr<spdlog::logger> console = spdlog::rotating_logger_mt("console", "log.spdlog", 1048576 * 5, 3);
