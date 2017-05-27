/*
 * common.h
 *
 *  Created on: 20.05.2017
 *      Author: raqu
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

//listener
#define LISTENER_PORT			4950
#define LISTENER_TIMEOUT		60*60 //seconds
#define GREETING_TIMEOUT		1 //seconds

// main message queue
#define MESS_QUEUE_SIZE 		256

// UDP sizes
#define MAX_DGRAM_LEN			4096 //bytes
#define UDP_QUEUE_SIZE			2048

#define MAX_RESPONER_THREADS	32

#define MAX_NICK_LEN			32 //chars

#define LOG_FILE_NAME			"fShare"

// protocol message types
typedef enum {
	GREETING,
	RESPLIST,
	REQLIST,
	REQFILE,
	RESPFILE,
	REQFDATA,
	ADDFILE,
	DELFILE,
	REVFILE,
	LOCFILE,
	UNLOCFILE
} MSG_TYPE;

#endif /* INCLUDE_COMMON_H_ */
