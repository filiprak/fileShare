/*
 * tcplistener.cpp
 *
 *  Created on: 29.05.2017
 *      Author: raqu
 */

#include "tcplistener.h"

#include <arpa/inet.h>
#include <logger.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <utilFunctions.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <common.h>
#include "localFileList.h"

TCPlistener::TCPlistener() : port(-1), tcpsock(-1) {
}

TCPlistener::~TCPlistener() {
}

void TCPlistener::init(long accpt_timeout) {
	if (transfering)
		throw std::runtime_error( strError("TCP socket is listening now, attepmt to reinitialize existing socket",
				__FUNCTION__) );

	struct timeval tv = { accpt_timeout , 0};

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 0;

	if ( ( tcpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) == -1 )
		throw std::runtime_error(strError("Failed to create TCPlistener socket",
				__FUNCTION__) );

	if ( setsockopt(tcpsock, SOL_SOCKET, SO_RCVTIMEO,
			(const char*) &tv, sizeof(struct timeval)) == -1) {
		close(tcpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	if (bind(tcpsock, (struct sockaddr*) &addr, sizeof addr) == -1) {
		close(tcpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	unsigned length = sizeof addr;
	if ( getsockname(tcpsock, (struct sockaddr*) &addr, &length) == -1 ) {
		close(tcpsock);
		throw std::runtime_error( strError(strerror(errno), __FUNCTION__) );
	}

	// save port number
	port = ntohs(addr.sin_port);
}

int TCPlistener::run(long recv_timeout, unsigned long nr_bytes, std::string client_ipv4) {
	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof their_addr;
	unsigned numbytes;

	// wait for one client
	listen(tcpsock, 1);
	int readsock = accept(tcpsock, (struct sockaddr*) &their_addr, &addr_len);
	logger->info("TCP: Waiting for incoming connection on port: {}", port);
	if (readsock == -1) {
		logger->warn("TCP: Error when accepting tcp client: {}", strerror(errno));
		close(tcpsock);
		return -1;
	}

	// check if accepted expected client
	std::string their_ipv4_str(inet_ntoa( ((struct sockaddr_in*)&their_addr)->sin_addr ));
	if (client_ipv4 != their_ipv4_str) {
		logger->warn("TCP: Accepted not expected client: exp:{} != {}",
				client_ipv4, their_ipv4_str);
		close(tcpsock);
		close(readsock);
		return -1;
	}

	// set recv timeout
	struct timeval tv = { recv_timeout , 0};
	if ( setsockopt(readsock, SOL_SOCKET, SO_RCVTIMEO,
			(const char*) &tv, sizeof(struct timeval)) == -1) {
		close(tcpsock);
		close(readsock);
		return -1;
	}

	// open temp file to flush received data
	std::string temppath = local_dirname + "/" + std::string(TEMP_FILE_TEMPLATE);
	char* tmpname = (char*) temppath.c_str();
	int tmpfd = mkstemp(tmpname);
	if (tmpfd == -1) {
		logger->warn("TCP: Error creating temp file for data: {}", strerror(errno));
		close(tcpsock);
		close(readsock);
		return -1;
	}

	// save temp file name
	temp_file = std::string(tmpname);

	// read data from client
	int nr_byt_to_read = nr_bytes >= MAX_CHUNK_SIZE ? MAX_CHUNK_SIZE : nr_bytes;
	char buffer[MAX_CHUNK_SIZE];
	transfering = true;
	while(nr_byt_to_read > 0) {
		if (!transfering) {
			logger->warn("TCP: listener stop forced");
			close(tcpsock);
			close(readsock);
			close(tmpfd);
			return -1;
		}

		// check if all bytes was received
		int rest_data = nr_byt_to_read;
		do {
			memset(buffer, 0, MAX_CHUNK_SIZE);
			numbytes = recv(readsock, buffer, rest_data, 0);
			if (numbytes < 0 || !transfering) {
				logger->error("TCP: recv error: {}, numbytes: {}", strerror(errno), numbytes);
				close(tcpsock);
				close(readsock);
				close(tmpfd);
				return -1;
			}
			write(tmpfd, buffer, numbytes);
			rest_data -= numbytes;
		} while (rest_data > 0 && transfering);

		//update nr bytes to receive next
		nr_bytes -= nr_byt_to_read;
		nr_byt_to_read = nr_bytes >= MAX_CHUNK_SIZE ? MAX_CHUNK_SIZE : nr_bytes;
	}
	transfering = false;
	close(tcpsock);
	close(readsock);
	close(tmpfd);
	logger->info("TCP: listener, received all requested file bytes from {}",
			their_ipv4_str);
	return 0;
}

void TCPlistener::stop() {
	transfering = false;
}
