/*
 * network.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <common.h>
#include <utilFunctions.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <network.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include <cstdio>
#include <stdexcept>

char Network::broadcast_addr[];
char Network::my_ipv4_addr[];

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

void Network::initMyAddress() {
	if( getIfaceInfo(NET_IFACE, my_ipv4_addr) != 0)
		throw std::runtime_error(strError("Cannot find interface IPv4 address", __FUNCTION__));
	if( getIfaceInfo(NET_IFACE, broadcast_addr, true) != 0)
		throw std::runtime_error(strError("Cannot determine broadcast address", __FUNCTION__));
}

Network::Network() {

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
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	/* Send another packet to the destination specified above */
	if ((numbytes=sendto(udpsock, data, strlen(data), 0,
	    (struct sockaddr *) &destAddr, sizeof destAddr)) == -1) {
		close(udpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	if (numbytes < strlen(data)) {
		close(udpsock);
		throw std::runtime_error( strError("Some data was lost while sending UDP datagram",
				__FUNCTION__) );
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
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	// this call is what allows broadcast packets to be sent:
	if (setsockopt(udpsock, SOL_SOCKET, SO_BROADCAST, &broadcast,
		sizeof broadcast) == -1) {
		close(udpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	/* Send another packet to the destination specified above */
	if ((numbytes=sendto(udpsock, data, strlen(data), 0,
	    (struct sockaddr *) &destAddr, sizeof destAddr)) == -1) {
		close(udpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
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

void UDPlistener::init(unsigned timeout, int forceport) {
	if (listening)
		throw std::runtime_error( strError("UDP socket is listening now, attepmt to reinitialize existing socket",
				__FUNCTION__) );

	close(udpsock);
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

int UDPlistener::run(int nr_dgrams) {
	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof their_addr;
	int numbytes;

	listening = true;
	while (listening && (nr_dgrams > 0)) {
		char buf[MAX_DGRAM_LEN];
		if ( (numbytes = recvfrom(udpsock,
								buf,
								MAX_DGRAM_LEN - 1,
								0,
								(struct sockaddr *)&their_addr,
								&addr_len) ) == -1) {
			continue;
			//throw std::runtime_error("Failed to receive UDP datagram");
		} else {
			struct sockaddr_in* their_addr_in = (struct sockaddr_in *) &their_addr;
			std::string their_str(inet_ntoa(their_addr_in->sin_addr));
			std::string datagram(buf);

			// save received datagram
			receivedUDPs[their_str] = datagram;
			nr_dgrams--;
		}
	}
	listening = false;
	close(udpsock);
	return 0;
}

void UDPlistener::stop() {
	listening = false;
}

std::thread Network::listenUDP(unsigned timeout, int nr_dgrams, int forceport) {
	udplisten.init(timeout, forceport);
	return std::thread( [=] { udplisten.run(nr_dgrams); } );
}


