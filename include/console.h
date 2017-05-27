/*
 * console.h
 *
 *  Created on: 27.05.2017
 *      Author: raqu
 */

#ifndef UTILS_CONSOLE_H_
#define UTILS_CONSOLE_H_

#include <curses.h>
#include <mutex>
#include <controller.h>


class Console {
private:
	std::mutex mux;
	Controller* controller = nullptr;
	WINDOW* inputLine = nullptr;
	WINDOW* outputLines = nullptr;
	bool running = false;

public:
	Console();
	virtual ~Console();

	void run();

	void sendFormattedMsg(short prefixColor,
			const char* prefix, short color, const char* format, ...);
	void inputLoop(void);

	bool isRunning() const {
		return running;
	}

	void stop() {
		running = false;
	}

	void setController(Controller* controller) {
		this->controller = controller;
	}
};

//create user interface
extern Console UI;

#endif /* UTILS_CONSOLE_H_ */
