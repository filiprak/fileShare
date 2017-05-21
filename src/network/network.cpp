/*
 * network.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "network.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>
#include <unistd.h>

Network::Network() {
	// TODO Auto-generated constructor stub

}

Network::~Network() {
	// TODO Auto-generated destructor stub
}

void Network::sendUDP(const char* data, const char* ipv4, int port) {

	int udpsock;

	struct sockaddr_in destAddr;
	int numbytes;

	/* Specify the address family */
	destAddr.sin_family = AF_INET;
	/* Specify the destination port */
	destAddr.sin_port = htons(port);
	/* Specify the destination IP address */
	destAddr.sin_addr.s_addr = inet_addr(ipv4);

	/* Create a socket */
	udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	/*
	 * Verify the socket was created correctly. If not, return
	 * immediately
	 */
	if (udpsock == -1) {
		throw std::runtime_error( strerror(errno) );
	}

	/* Send another packet to the destination specified above */
	if ((numbytes=sendto(udpsock, data, strlen(data), 0,
	    (struct sockaddr *) &destAddr, sizeof destAddr)) == -1) {
		close(udpsock);
		throw std::runtime_error( strerror(errno) );
	}

	if (numbytes < strlen(data)) {
		close(udpsock);
		throw std::runtime_error("Some data was lost while sending UDP datagram");
	}

	printf("UDP datagram was sent, nr bytes: %d\n", numbytes);
	close(udpsock);
}

void Network::broadcastUDP(const char* data, int port) {
}

TCPlistener::TCPlistener() {
}

TCPlistener::~TCPlistener() {
}

void TCPlistener::init() {
}

int TCPlistener::run() {
}

void TCPlistener::stop() {
}

UDPlistener::UDPlistener() {
}

UDPlistener::~UDPlistener() {
}

void UDPlistener::init() {
}

int UDPlistener::run() {
}

void UDPlistener::stop() {
}
