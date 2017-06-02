/*
 * network.h
 *
 *	Network service - handles sockets and connections,
 *	can listen for incoming messages or send messages
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#ifndef NETWORK_NETWORK_H_
#define NETWORK_NETWORK_H_

#include <netdb.h>
#include <udplistener.h>
#include <string>


class Network {
private:

	UDPlistener udplisten;
	static char broadcast_addr[NI_MAXHOST];
	static char my_ipv4_addr[NI_MAXHOST];

	// local host nick - network-unique
	static std::string my_nick;

public:

	Network();
	virtual ~Network();

	static void initMyAddress(const char* iface);

	// send single udp datagram
	void sendUDP(const char* data, const char* ipv4, int port);
	void sendUDP(Message* mess, const char* ipv4, int dest_port);
	// broadcast single udp datagram
	void broadcastUDP(const char* data, int dest_port);
	void broadcastUDP(Message* mess, int dest_port);
	// connect and send tcp data
	int fstreamTCP(int fd, unsigned long offset, unsigned long size,
			const char* ipv4, int dest_port, long send_timeout);

	// open tcp listen socket on any free port
	void listenTCP(const char* data, const char* ipv4, int* port, unsigned timeout);
	// receive one or several udp datagrams
	void listenUDP(unsigned timeout, int exp_dgrams=0, int forceport=0);

	static const char* getMyIpv4Addr() {
		return my_ipv4_addr;
	}

	UDPlistener& getUdplisten() {
		return udplisten;
	}

	static const std::string& getMyNick() {
		return my_nick;
	}

	static void setMyNick(const char* nick) {
		my_nick = nick;
	}
};

int getIfaceInfo(const char* iface, char* addr, bool braddr=false);

void listenUDPThread(UDPlistener& udplis, int nr_loops=0);

#endif /* NETWORK_NETWORK_H_ */
