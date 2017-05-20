/*
 * main.cpp
 *
 *	Program fileShare main start point
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <iostream>
#include <thread>
#include <blockingQueue.h>

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include "listener.h"
#include "common.h"

int main() {
	Controller controller;

	std::thread listenerT(listenerThread);

	controller.run();

	listenerT.join();

	return 0;
}


