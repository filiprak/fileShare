/*
 * network.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "network.h"
#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>
#include <unistd.h>
#include <ifaddrs.h>

// helper function to determine local ipv4 address
int getIfaceInfo(const char* iface, char* addr, bool braddr) {
	struct ifaddrs *ifaddr, *ifa;
	int s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		return -1;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		s = getnameinfo(ifa->ifa_addr,
				sizeof(struct sockaddr_in),
				host,
				NI_MAXHOST,
				NULL,
				0,
				NI_NUMERICHOST);

		if ((strcmp(ifa->ifa_name, iface) == 0)
				&& (ifa->ifa_addr->sa_family == AF_INET)) {
			if (s != 0)
				return -1;

			if (braddr) {
				struct sockaddr_in* netmask = (struct sockaddr_in*) ifa->ifa_netmask;
				struct sockaddr_in* ipaddr = (struct sockaddr_in*) ifa->ifa_addr;

				in_addr_t netbin = netmask->sin_addr.s_addr;
				in_addr_t ipbin = ipaddr->sin_addr.s_addr;
				in_addr_t braddr = ( (~netbin) | ipbin );

				struct in_addr br_inaddr = { .s_addr = braddr };

				strcpy(addr, inet_ntoa(br_inaddr));
				freeifaddrs(ifaddr);
				return 0;
			}
			freeifaddrs(ifaddr);
			strcpy(addr, host);
			return 0;
		}
	}

	freeifaddrs(ifaddr);
	return -1;
}

Network::Network() {
	if( getIfaceInfo(NET_IFACE, my_ipv4_addr) != 0)
		throw std::runtime_error("Cannot find interface IPv4 address");
	if( getIfaceInfo(NET_IFACE, broadcast_addr, true) != 0)
		throw std::runtime_error("Cannot determine broadcast address");
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

	int udpsock;
	int broadcast = 1;
	struct sockaddr_in destAddr;
	int numbytes;

	/* Specify the address family */
	destAddr.sin_family = AF_INET;
	/* Specify the destination port */
	destAddr.sin_port = htons(port);
	/* Specify the broadcast IP address */
	destAddr.sin_addr.s_addr = inet_addr(broadcast_addr);

	/* Create a socket */
	udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	/*
	 * Verify the socket was created correctly. If not, return
	 * immediately
	 */
	if (udpsock == -1) {
		throw std::runtime_error( strerror(errno) );
	}

	// this call is what allows broadcast packets to be sent:
	if (setsockopt(udpsock, SOL_SOCKET, SO_BROADCAST, &broadcast,
		sizeof broadcast) == -1) {
		close(udpsock);
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
		throw std::runtime_error("Some data was lost while broadcasting UDP datagram");
	}

	printf("UDP broadcast was sent, nr bytes: %d\n", numbytes);
	close(udpsock);
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
