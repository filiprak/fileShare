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

#include <string>
#include <netdb.h>

class Network {
private:
	char broadcast_addr[NI_MAXHOST];
	char my_ipv4_addr[NI_MAXHOST];

public:
	Network();
	virtual ~Network();

	// send single udp datagram
	void sendUDP(const char* data, const char* ipv4, int port);
	// broadcast single udp datagram
	void broadcastUDP(const char* data, int port);
	// connect and send tcp data
	void sendTCP(const char* data, const char* ipv4, int port, unsigned timeout);

	// open tcp listen socket on any free port
	void listenTCP(const char* data, const char* ipv4, int* port, unsigned timeout);
	// receive one or several udp datagrams
	void listenUDP(const char* data, const char* ipv4, int* port, unsigned timeout);

};

// listener of tcp datagrams
class TCPlistener {
private:
	bool listening = false;
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
	bool listening = false;
	int port;

public:
	UDPlistener();
	virtual ~UDPlistener();

	void init();
	int run();
	void stop();
};

int getIfaceInfo(const char* iface, char* addr, bool braddr=false);

#endif /* NETWORK_NETWORK_H_ */
