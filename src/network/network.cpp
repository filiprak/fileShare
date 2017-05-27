/*
 * network.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <common.h>
#include <errno.h>
#include <ifaddrs.h>
#include <logger.h>
#include <netinet/in.h>
#include <network.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <utilFunctions.h>
#include <cstring>
#include <stdexcept>
#include <console.h>


char Network::broadcast_addr[];
char Network::my_ipv4_addr[];
std::string Network::my_nick;

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

void Network::initMyAddress(const char* iface) {
	if( getIfaceInfo(iface, my_ipv4_addr) != 0)
		throw std::runtime_error("Cannot find interface IPv4 address\n");
	if( getIfaceInfo(iface, broadcast_addr, true) != 0)
		throw std::runtime_error("Cannot resolve broadcast address\n");
}

Network::Network() : udplisten(UDP_QUEUE_SIZE) {

}

Network::~Network() {
	// TODO Auto-generated destructor stub
}

void Network::sendUDP(Message* mess, const char* ipv4, int dest_port) {
	sendUDP(mess->jsonify().c_str(), ipv4, dest_port );
}

void Network::sendUDP(const char* data, const char* ipv4, int dest_port) {

	int udpsock;

	struct sockaddr_in destAddr;
	int numbytes;

	/* Specify the address family */
	destAddr.sin_family = AF_INET;
	/* Specify the destination port */
	destAddr.sin_port = htons(dest_port);
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
	logger->info("Sent UDP datagram to {}:{}: {}", ipv4, dest_port, data);
	close(udpsock);
}

void Network::broadcastUDP(Message* mess, int dest_port) {
	broadcastUDP( mess->jsonify().c_str(), dest_port );
}

void Network::broadcastUDP(const char* data, int dest_port) {

	int udpsock;
	int broadcast = 1;
	struct sockaddr_in destAddr;
	int numbytes;

	/* Specify the address family */
	destAddr.sin_family = AF_INET;
	/* Specify the destination port */
	destAddr.sin_port = htons(dest_port);
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

	logger->info("Sent broadcast to ports {}: {}", dest_port, data);
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

UDPlistener::UDPlistener(unsigned qsize) : receivedUDPs(qsize) {
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

void Network::listenUDP(unsigned timeout, int exp_dgrams, int forceport) {
	try {
		udplisten.init(timeout, forceport);
		udplisten.run(exp_dgrams);

	} catch (const std::exception& e) {
		UI.error("Network: ", e.what());
		logger->error("Exception in Network: {}: {}", __FUNCTION__, e.what());
		logger->flush();
	}
}

void listenUDPThread(UDPlistener& udplis, int exp_dgrams) {
	udplis.run(exp_dgrams);
}
