/*
 * controller.h
 *
 *	Controls network task execution and user commands,
 *	central point of the program
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#ifndef LOGIC_CONTROLLER_H_
#define LOGIC_CONTROLLER_H_

class Controller {
public:
	Controller();
	virtual ~Controller();
};

// threads start functions
void addFile();

#endif /* LOGIC_CONTROLLER_H_ */
