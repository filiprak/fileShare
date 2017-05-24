/*
 * common.h
 *
 *  Created on: 20.05.2017
 *      Author: raqu
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#define NET_IFACE 				"enp0s3"
#define NET_IFACE 				"enp8s0"

#define LISTENER_PORT			4950
#define MESS_QUEUE_SIZE 		256
#define MAX_DGRAM_LEN			4096
#define UDP_QUEUE_SIZE			2048

#define MAX_RESPONER_THREADS	32

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
