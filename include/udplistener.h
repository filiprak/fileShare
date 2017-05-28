/*
 * udplistener.h
 *
 *  Created on: 29.05.2017
 *      Author: raqu
 */

#ifndef NETWORK_UDPLISTENER_H_
#define NETWORK_UDPLISTENER_H_

#include <blockingQueue.h>
#include <datagram.h>
#include <message.h>
#include <queue>

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

#endif /* NETWORK_UDPLISTENER_H_ */
