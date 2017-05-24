/*
 * message.h
 *
 *	Message types that are program representation
 *	of comunicates exchanged beetwen hosts
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#ifndef NETWORK_MESSAGE_H_
#define NETWORK_MESSAGE_H_

#include <string>
#include "common.h"
#include "datagram.h"
#include <json/json.h>

using namespace std;

class Message {
public:
	// ipv4 address of sender
	string sender_ipv4;
	// message type: GREETING, RESPFILE, ...
	MSG_TYPE type;
	// json fast writer
	Json::FastWriter fast_writer;


	Message(MSG_TYPE type, const char* sender_ipv4);
	virtual ~Message();

	const string& getSenderIpv4() const {
		return sender_ipv4;
	}

	MSG_TYPE getType() const {
		return type;
	}
};

class MessageGREETING: public Message {
private:
	std::string sender;

public:

	MessageGREETING(const char* sender_ipv4, std::string sender);
	virtual ~MessageGREETING();

	// generates json format of message
	std::string jsonify();

	std::string& getSender() {
		return sender;
	}
};

class MessageRESPLIST: public Message {
public:

	MessageRESPLIST(const char* sender_ipv4);
	virtual ~MessageRESPLIST();

	// generates json format of message
	std::string jsonify();
};

class MessageREQLIST: public Message {
public:

	MessageREQLIST(const char* sender_ipv4);
	virtual ~MessageREQLIST();

	// generates json format of message
	std::string jsonify();
};

class MessageREQFILE: public Message {
public:

	MessageREQFILE(const char* sender_ipv4);
	virtual ~MessageREQFILE();

	// generates json format of message
	std::string jsonify();
};

class MessageRESPFILE: public Message {
public:

	MessageRESPFILE(const char* sender_ipv4);
	virtual ~MessageRESPFILE();

	// generates json format of message
	std::string jsonify();
};

class MessageREQFDATA: public Message {
public:

	MessageREQFDATA(const char* sender_ipv4);
	virtual ~MessageREQFDATA();

	// generates json format of message
	std::string jsonify();
};

class MessageADDFILE: public Message {
public:

	MessageADDFILE(const char* sender_ipv4);
	virtual ~MessageADDFILE();

	// generates json format of message
	std::string jsonify();
};

class MessageDELFILE: public Message {
public:

	MessageDELFILE(const char* sender_ipv4);
	virtual ~MessageDELFILE();

	// generates json format of message
	std::string jsonify();
};

class MessageREVFILE: public Message {
public:

	MessageREVFILE(const char* sender_ipv4);
	virtual ~MessageREVFILE();

	// generates json format of message
	std::string jsonify();
};

class MessageLOCFILE: public Message {
public:

	MessageLOCFILE(const char* sender_ipv4);
	virtual ~MessageLOCFILE();

	// generates json format of message
	std::string jsonify();
};

class MessageUNLOCFILE: public Message {
public:

	MessageUNLOCFILE(const char* sender_ipv4);
	virtual ~MessageUNLOCFILE();

	// generates json format of message
	std::string jsonify();
};

// function creates message object from
Message* parseJSONtoMessage( const Datagram* dgram );

#endif /* NETWORK_MESSAGE_H_ */
