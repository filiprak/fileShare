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

class Network {
public:
	Network();
	virtual ~Network();

	// send single udp datagram
	void sendUDP(const char* data, std::string ipv4, int port);
	// broadcast single udp datagram
	void broadcastUDP(const char* data, int port);
	// connect and send tcp data
	void sendTCP(const char* data, std::string ipv4, int port);

	// open tcp listen socket on any free port
	void listenTCP(const char* data, std::string ipv4, int* port);
	// receive one or several udp datagrams
	void listenUDP(const char* data, std::string ipv4, int* port);
};

#endif /* NETWORK_NETWORK_H_ */
