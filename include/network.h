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

#include <blockingQueue.h>
#include <datagram.h>
#include <message.h>
#include <netdb.h>
#include <queue>
#include <string>

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
	BlockingQueue< Datagram* > receivedUDPs;
	bool listening = false;
	// port of listener socket
	int port;

	// listener socket fd
	int udpsock;

public:
	UDPlistener(unsigned qsize);
	virtual ~UDPlistener();

	void init(unsigned timeout, int forceport=0);
	int run(int exp_dgrams=0);
	bool isListening();
	void stop();

	// receive messages functions
	Message* receiveMessage();
	std::queue< Message* > receiveMessages();

	int getPort() {
		return port;
	}

	BlockingQueue< Datagram* >& getReceivedUdPs() {
		return receivedUDPs;
	}

	void clearReceivedUDPs() {
		while( receivedUDPs.notEmpty() ) {
			delete receivedUDPs.take();
		}
	}
};



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
	void sendTCP(const char* data, const char* ipv4, int port, unsigned timeout);

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
