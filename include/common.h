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
#define GREETING_TIMEOUT		3 //seconds

// main message queue
#define MESS_QUEUE_SIZE 		256

// UDP sizes
#define MAX_DGRAM_LEN			4096 //bytes
#define UDP_QUEUE_SIZE			2048

//tcp sizes
#define MAX_CHUNK_SIZE			1024// 1 kB
#define CHUNK_DOWNLD_TRIES		3

#define TCP_SEND_TIMEOUT		5 //seconds
#define TCP_RECV_TIMEOUT		5 //seconds
#define TCP_ACCEPT_TIMEOUT		5 //seconds


#define MAX_RESPONER_THREADS	32

#define MAX_NICK_LEN			32 //chars

#define LOG_FILE_NAME			"fShare"
#define TEMP_FILE_TEMPLATE		"fchunk_XXXXXX"
#define LOCAL_FILES_DIRNAME				"fileShare"
#define LOCAL_FILES_DIRNAME_TESTING		"local"

// file list print formats
#define MAX_LINE_WIDTH	128
#define FILELIST_FORMAT "%-32s%c%-16s%c%-16lu%c%-5d%-7d%c%-40s\n"
#define HEADER_FORMAT "%-33s%-17s%-17s%-5s%-8s%-41s\n"

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
