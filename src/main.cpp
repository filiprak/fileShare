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

void producent(BlockingQueue<int>* q) {
	for (int i = 0; i < 10; ++i) {
			unsigned int t = rand()%3;
			for (int j=0; j<3; ++j) {
				int el = rand()%20 + 10000;
				q->push(el);
			}
			sleep(t);
		}
}

int main() {
	srand( time( NULL ) );

	BlockingQueue<int> queue(2, false);

	std::thread prodthread(producent, &queue);

	for (int i = 0; i < 10; ++i) {
		int elem = queue.pop();
	}

	prodthread.join();

	return 0;
}


