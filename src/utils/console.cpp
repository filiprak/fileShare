/*
 * console.cpp
 *
 *  Created on: 27.05.2017
 *      Author: raqu
 */

#include "console.h"
#include "ncurses.h"
#include "logger.h"
#include <string>
#include "errno.h"

Console UI;

Console::Console() {
}

void Console::run() {
	initscr();

	inputLine = newwin(1, COLS, LINES - 1, 0);
	outputLines = newwin(LINES - 1, COLS, 0, 0);

	if (has_colors()) {
		start_color();
		for (int i = 1; i <= COLOR_WHITE; ++i) {
			init_pair(i, i, COLOR_BLACK);
		}
	} else
		wprintw(outputLines, "Terminal cannot print colors.\n");

	scrollok(outputLines, TRUE);
	scrollok(inputLine, TRUE);

	leaveok(inputLine, TRUE);
	nodelay(inputLine, FALSE);
	cbreak();
	noecho();
	keypad(inputLine, TRUE);
	// hide cursor
	curs_set(0);

	running = true;
}

void Console::error(const char* format, ...) {
	if (!running) return;
	va_list args;
	va_start(args, format);
	sendFormattedMsg(COLOR_RED, "Error: ", COLOR_RED, format, args);
	va_end(args);
}

void Console::info(const char* format, ...) {
	if (!running) return;
	va_list args;
	va_start(args, format);
	sendFormattedMsg(COLOR_GREEN, "Info: ", COLOR_WHITE, format, args);
	va_end(args);
}

void Console::warning(const char* format, ...) {
	if (!running) return;
	va_list args;
	va_start(args, format);
	sendFormattedMsg(COLOR_YELLOW, "Warning: ", COLOR_YELLOW, format, args);
	va_end(args);
}

void Console::msg(const char* prefix, const char* format, ...) {
	if (!running) return;
	va_list args;
	va_start(args, format);
	sendFormattedMsg(COLOR_YELLOW, prefix, COLOR_WHITE, format, args);
	va_end(args);
}

void Console::sendFormattedMsg(short prefixColor,
		const char* prefix, short color, const char* format, va_list args) {
	if (!running) return;;

	//append new line to format
	int size = strlen(format);
	char* formatln = new char[size + 2];

	snprintf(formatln, size + 2, "%s\n", format);

    mux.lock();
	if (has_colors()) {
		if (prefix) {
			wattron(outputLines, A_BOLD | COLOR_PAIR(prefixColor));
			wprintw(outputLines, prefix);
		}

		//if (color == COLOR_WHITE)
		wattroff(outputLines, A_BOLD);
		wattron(outputLines, COLOR_PAIR(color));
		vwprintw(outputLines, formatln, args);

		wattroff(outputLines, A_BOLD | COLOR_PAIR(color));

	} else {
		wprintw(outputLines, prefix);
		vwprintw(outputLines, formatln, args);
	}

    wrefresh(outputLines);
    mux.unlock();
    delete [] formatln;
}

void Console::inputLoop(void) {

    static std::string input;

    mux.lock();
    wattron(inputLine, A_BOLD | COLOR_PAIR(COLOR_WHITE));
    wprintw(inputLine, "\n>: ");
    wattroff(inputLine, A_BOLD | COLOR_PAIR(COLOR_WHITE));

    wprintw(inputLine, input.c_str());
    wrefresh(inputLine);
    mux.unlock();

    int c = wgetch(inputLine);

    if (c == ERR) {
    	logger->warn("Input character read error");
        return;
    }

    switch (c) {
		case '\n':
			if (input.size() > 0) {
				wprintw(inputLine, "%s\n", input);
				// run user command
				controller->runCommand(input);
				input.clear();
			}
			break;

		case KEY_DC:
		case KEY_BACKSPACE:
			if (input.size() > 0) input.pop_back();
			break;

		default:
			input += (char) c;
			break;
    }
}

void Console::exit() {
	running = false;
	delwin(inputLine);
	delwin(outputLines);
	endwin();
}

Console::~Console() {

}

