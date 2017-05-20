/*
 * common.h
 *
 *  Created on: 20.05.2017
 *      Author: raqu
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#define MESS_QUEUE_SIZE 256

#include <blockingQueue.h>


BlockingQueue<Message> messageQueue(MESS_QUEUE_SIZE, false);


#endif /* INCLUDE_COMMON_H_ */
