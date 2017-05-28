/*
 * udplistener.cpp
 *
 *  Created on: 29.05.2017
 *      Author: raqu
 */

#include "udplistener.h"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <common.h>
#include <logger.h>
#include <netinet/in.h>
#include <network.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <utilFunctions.h>
#include <cstring>
#include <stdexcept>


UDPlistener::UDPlistener(unsigned qsize)
	: receivedUDPs(qsize), udpsock(-1), port(-1) {
}

UDPlistener::~UDPlistener() {
}

void UDPlistener::init(unsigned timeout, int forceport) {
	if (listening)
		throw std::runtime_error( strError("UDP socket is listening now, attepmt to reinitialize existing socket",
				__FUNCTION__) );

	receivedUDPs.clear();

	struct timeval tv;
	tv.tv_sec = timeout;  /* Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(forceport);

	if ( ( udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 )
		throw std::runtime_error(strError("Failed to create UDPlistener socket",
				__FUNCTION__) );

	int err = setsockopt(udpsock, SOL_SOCKET, SO_RCVTIMEO,
			(const char*) &tv, sizeof(struct timeval));
	if (err == -1) {
		close(udpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	if (bind(udpsock, (struct sockaddr*) &addr, sizeof addr) == -1) {
		close(udpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	unsigned length = sizeof addr;
	if ( getsockname(udpsock, (struct sockaddr*) &addr, &length) == -1 ) {
		close(udpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	// save port number
	port = ntohs(addr.sin_port);
}

Message* UDPlistener::receiveMessage() {
	// init must be called before
	run(1);
	if ( receivedUDPs.notEmpty() ) {
		Datagram* dgram = receivedUDPs.take();
		Message* m = parseJSONtoMessage(dgram);
		return m;
	}
	return nullptr;
}

std::queue<Message*> UDPlistener::receiveMessages() {
	// init must be called before
	run();
	std::queue<Message*> recvd;
	while ( receivedUDPs.notEmpty() ) {
		Datagram* dgram = receivedUDPs.take();
		Message* m = parseJSONtoMessage(dgram);
		if (m != nullptr)
			recvd.push(m);
	}
	return recvd;
}

int UDPlistener::run(int exp_dgrams) {

	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof their_addr;
	int numbytes;

	bool infinite = (exp_dgrams <= 0);

	logger->info("Running UDP listener on port: {}", port);
	listening = true;
	while (listening && (infinite || exp_dgrams > 0)) {
		char buf[MAX_DGRAM_LEN];
		memset(buf, 0, sizeof buf);

		if ( (numbytes = recvfrom(udpsock,
								buf,
								MAX_DGRAM_LEN - 1,
								0,
								(struct sockaddr *)&their_addr,
								&addr_len) ) <= 0) {
			// break if time passed
			logger->info("Socket timeout or recvfrom() error");
			break;
		} else {
			struct sockaddr_in* their_addr_in = (struct sockaddr_in *) &their_addr;

			Datagram* dgram = new Datagram( inet_ntoa(their_addr_in->sin_addr), buf, numbytes);
			// drop datagrams from myself
			if ( strcmp(dgram->getSender(), Network::getMyIpv4Addr() ) == 0 ) {
				logger->info("Dropped message from myself: {}", dgram->getSender() );
				delete dgram;
				continue;
			}
			// save received datagram
			logger->info("Received datagram from {}: {}", dgram->getSender(),
					dgram->getBytes() );
			receivedUDPs.insert( dgram );
			if (!infinite)
				exp_dgrams--;
		}
	}
	listening = false;
	logger->info("Closed UDP listener on port: {}", port);
	close(udpsock);
	return 0;
}

bool UDPlistener::isListening() {
	return listening;
}

void UDPlistener::stop() {
	listening = false;
}
