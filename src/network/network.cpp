/*
 * network.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "network.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

Network::Network() {
	// TODO Auto-generated constructor stub

}

Network::~Network() {
	// TODO Auto-generated destructor stub
}

void Network::sendUDP(const char* data, std::string ipv4, int port) {
}

void Network::broadcastUDP(const char* data, int port) {
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
