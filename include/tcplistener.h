/*
 * tcplistener.h
 *
 *  Created on: 29.05.2017
 *      Author: raqu
 */

#ifndef NETWORK_TCPLISTENER_H_
#define NETWORK_TCPLISTENER_H_

#include <string>

// listener of tcp datagrams
class TCPlistener {
private:

	// temporary file where received data is flushed
	std::string temp_file;

	bool transfering = false;

	// port of listener socket
	int port;

	// listener socket fd
	int tcpsock;

public:
	TCPlistener();
	virtual ~TCPlistener();

	void init(unsigned accpt_timeout);
	int run(unsigned recv_timeout, unsigned long nr_bytes, std::string client_ipv4);
	void stop();

	int getPort() const {
		return port;
	}

	std::string getTempFile() {
		return temp_file;
	}
};

#endif /* NETWORK_TCPLISTENER_H_ */
