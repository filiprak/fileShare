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

#include <utility>
#include <string>
#include <netdb.h>
#include <thread>
#include <blockingQueue.h>
#include "datagram.h"

// listener of tcp datagrams
class TCPlistener {
private:

	bool listening = false;
	// port of listener socket
	int port;

public:
	TCPlistener();
	virtual ~TCPlistener();

	void init();
	int run();
	void stop();
};

// listener of udp datagrams
class UDPlistener {
private:
	// pair: key = sender address, value = received bytes
	BlockingQueue< Datagram > receivedUDPs;
	bool listening = false;
	// port of listener socket
	int port;

	// listener socket fd
	int udpsock;

public:
	UDPlistener(unsigned qsize);
	virtual ~UDPlistener();

	void init(unsigned timeout, int forceport=0);
	int run(int nr_dgrams);
	void stop();

	int getPort() const {
		return port;
	}

	BlockingQueue< Datagram >& getReceivedUdPs() {
		return receivedUDPs;
	}
};



class Network {
private:

	UDPlistener udplisten;
	static char broadcast_addr[NI_MAXHOST];
	static char my_ipv4_addr[NI_MAXHOST];

public:

	Network();
	virtual ~Network();

	static void initMyAddress();

	// send single udp datagram
	void sendUDP(const char* data, const char* ipv4, int port);
	// broadcast single udp datagram
	void broadcastUDP(const char* data, int port);
	// connect and send tcp data
	void sendTCP(const char* data, const char* ipv4, int port, unsigned timeout);

	// open tcp listen socket on any free port
	void listenTCP(const char* data, const char* ipv4, int* port, unsigned timeout);
	// receive one or several udp datagrams
	std::thread listenUDP(unsigned timeout, int nr_dgrams, int forceport=0);

	const char* getMyIpv4Addr() const {
		return my_ipv4_addr;
	}

	UDPlistener& getUdplisten() {
		return udplisten;
	}
};

int getIfaceInfo(const char* iface, char* addr, bool braddr=false);

#endif /* NETWORK_NETWORK_H_ */
