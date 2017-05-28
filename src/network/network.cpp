/*
 * network.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <arpa/inet.h>
#include <common.h>
#include <console.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <logger.h>
#include <message.h>
#include <netinet/in.h>
#include <network.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <utilFunctions.h>
#include <cstring>
#include <stdexcept>


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
	if ((numbytes = sendto(udpsock, data, strlen(data), 0,
			(struct sockaddr *) &destAddr, sizeof destAddr)) == -1) {
		close(udpsock);
		throw std::runtime_error(strError(strerror(errno), __FUNCTION__));
	}
	if (numbytes < strlen(data)) {
		close(udpsock);
		throw std::runtime_error("Some data was lost while broadcasting UDP datagram");
	}

	logger->info("Sent broadcast to ports {}: {}", dest_port, data);
	close(udpsock);
}

int Network::fstreamTCP(int fd, unsigned long offset, unsigned long size,
		const char* ipv4, int dest_port, unsigned send_timeout) {

	int tcpsock; // socket fd

	struct timeval tv = { send_timeout , 0};

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ipv4);
	serv_addr.sin_port = htons(dest_port);

	if ( ( tcpsock = socket(AF_INET, SOCK_STREAM , IPPROTO_TCP) ) == -1 )
		return -1;

	if ( setsockopt(tcpsock, SOL_SOCKET, SO_RCVTIMEO,
			(const char*) &tv, sizeof(struct timeval)) == -1) {
		close(tcpsock);
		return -1;
	}

	// connect to server - nonblocking mode
	if (connect(tcpsock, (struct sockaddr*) &serv_addr, sizeof serv_addr) == -1) {
		logger->warn("{}: Failed to connect to TCP server, errno: {}",
				__FUNCTION__, strerror(errno));
		if (errno != EINPROGRESS) {
			close(tcpsock);
			return -1;
		}
	}

	// stream file bytes
	unsigned long nr_byt_to_send = size >= MAX_CHUNK_SIZE ? MAX_CHUNK_SIZE : size;
	char* chunk_bytes[MAX_CHUNK_SIZE];
	// move file position about offset bytes
	if (lseek(fd, offset, SEEK_SET) == -1) {
		logger->warn("{}: lseek() failed, errno: {}",
						__FUNCTION__, strerror(errno));
		close(tcpsock);
		return -1;
	}

	while(nr_byt_to_send > 0) {
		memset(chunk_bytes, 0, MAX_CHUNK_SIZE);
		if (read(fd, chunk_bytes, nr_byt_to_send) != nr_byt_to_send) {
			logger->warn("{}: read() failed, errno msg: {}", __FUNCTION__, strerror(errno));
			close(tcpsock);
			return -1;
		}
		if (send(tcpsock, chunk_bytes, nr_byt_to_send, 0) != nr_byt_to_send ) {
			logger->warn("{}: send() failed, errno msg: {}", __FUNCTION__, strerror(errno));
			close(tcpsock);
			return -1;
		}

		size -= nr_byt_to_send;
		nr_byt_to_send = size >= MAX_CHUNK_SIZE ? MAX_CHUNK_SIZE : size;
	}
	logger->info("TCP: all file chunk bytes sent sucessfully");
	close(tcpsock);
	return 0;
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

