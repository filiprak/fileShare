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

using namespace std;

class Message {
public:
	// ipv4 address of sender
	string sender_ipv4;
	// message type: GREETING, RESPFILE, ...
	MSG_TYPE type;

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
public:

	MessageGREETING(const char* sender_ipv4);
	virtual ~MessageGREETING();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageRESPLIST: public Message {
public:

	MessageRESPLIST(const char* sender_ipv4);
	virtual ~MessageRESPLIST();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREQLIST: public Message {
public:

	MessageREQLIST(const char* sender_ipv4);
	virtual ~MessageREQLIST();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREQFILE: public Message {
public:

	MessageREQFILE(const char* sender_ipv4);
	virtual ~MessageREQFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageRESPFILE: public Message {
public:

	MessageRESPFILE(const char* sender_ipv4);
	virtual ~MessageRESPFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREQFDATA: public Message {
public:

	MessageREQFDATA(const char* sender_ipv4);
	virtual ~MessageREQFDATA();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageADDFILE: public Message {
public:

	MessageADDFILE(const char* sender_ipv4);
	virtual ~MessageADDFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageDELFILE: public Message {
public:

	MessageDELFILE(const char* sender_ipv4);
	virtual ~MessageDELFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREVFILE: public Message {
public:

	MessageREVFILE(const char* sender_ipv4);
	virtual ~MessageREVFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageLOCFILE: public Message {
public:

	MessageLOCFILE(const char* sender_ipv4);
	virtual ~MessageLOCFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageUNLOCFILE: public Message {
public:

	MessageUNLOCFILE(const char* sender_ipv4);
	virtual ~MessageUNLOCFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

// function creates message object from
Message parseJSONtoMessage( const Datagram& dgram );

#endif /* NETWORK_MESSAGE_H_ */
