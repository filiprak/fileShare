/*
 * datagram.h
 *
 *	Container class for received bytes and sender address
 *
 *  Created on: 23.05.2017
 *      Author: raqu
 */

#ifndef NETWORK_DATAGRAM_H_
#define NETWORK_DATAGRAM_H_

#include <common.h>
#include <netdb.h>
#include <cstring>


class Datagram {
private:
	// ipv4 address of datagram sender
	char sender[NI_MAXHOST];
	// data received
	char bytes[MAX_DGRAM_LEN];
	// data size
	unsigned dsize;

public:

	Datagram(const char* sender, const char* data, unsigned dsize) : dsize(dsize) {
		if ( dsize > MAX_DGRAM_LEN)
			dsize = MAX_DGRAM_LEN;

		memset(this->sender, 0, sizeof this->sender);
		memset(this->bytes, 0, sizeof this->bytes);

		memcpy(this->sender, sender, sizeof this->sender);
		memcpy(this->bytes, data, dsize);
	}

	const char* getBytes() const {
		return bytes;
	}

	const char* getSender() const {
		return sender;
	}
};

#endif /* NETWORK_DATAGRAM_H_ */
