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

#include <common.h>
#include <json/json.h>
#include <string>

class Datagram;

using namespace std;

class Message {
public:
	// ipv4 address of sender
	std::string sender_ipv4;
	// sender nickname
	std::string sender_nick;
	// message type: GREETING, RESPFILE, ...
	MSG_TYPE type;
	// json fast writer
	static Json::FastWriter fast_writer;


	Message(MSG_TYPE type, const char* sender_ipv4, std::string sender_nick);
	virtual ~Message();

	virtual std::string jsonify() = 0;

	const string& getSenderIpv4() const {
		return sender_ipv4;
	}

	MSG_TYPE getType() const {
		return type;
	}

	const std::string& getNick() const {
		return sender_nick;
	}
};

class MessageGREETING: public Message {
private:
	unsigned resp_port;

public:

	MessageGREETING(const char* sender_ipv4, std::string sender_nick, unsigned rport=0);
	MessageGREETING(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageGREETING();

	// generates json format of message
	virtual std::string jsonify();

	unsigned getRespPort() const {
		return resp_port;
	}
};

class MessageRESPLIST: public Message {
private:
	Json::Value jlist;

public:

	MessageRESPLIST(const char* sender_ipv4, std::string sender_nick, Json::Value& jlist);
	MessageRESPLIST(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageRESPLIST();

	// generates json format of message
	virtual std::string jsonify();

	const Json::Value& getJlist() const {
		return jlist;
	}
};

class MessageREQLIST: public Message {
public:

	MessageREQLIST(const char* sender_ipv4, std::string sender_nick);
	MessageREQLIST(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageREQLIST();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageREQFILE: public Message {
public:

	MessageREQFILE(const char* sender_ipv4, std::string sender_nick);
	MessageREQFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageREQFILE();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageRESPFILE: public Message {
public:

	MessageRESPFILE(const char* sender_ipv4, std::string sender_nick);
	MessageRESPFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageRESPFILE();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageREQFDATA: public Message {
public:

	MessageREQFDATA(const char* sender_ipv4, std::string sender_nick);
	MessageREQFDATA(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageREQFDATA();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageADDFILE: public Message {
public:

	MessageADDFILE(const char* sender_ipv4, std::string sender_nick);
	MessageADDFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageADDFILE();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageDELFILE: public Message {
public:

	MessageDELFILE(const char* sender_ipv4, std::string sender_nick);
	MessageDELFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageDELFILE();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageREVFILE: public Message {
public:

	MessageREVFILE(const char* sender_ipv4, std::string sender_nick);
	MessageREVFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageREVFILE();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageLOCFILE: public Message {
public:

	MessageLOCFILE(const char* sender_ipv4, std::string sender_nick);
	MessageLOCFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageLOCFILE();

	// generates json format of message
	virtual std::string jsonify();
};

class MessageUNLOCFILE: public Message {
public:

	MessageUNLOCFILE(const char* sender_ipv4, std::string sender_nick);
	MessageUNLOCFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageUNLOCFILE();

	// generates json format of message
	virtual std::string jsonify();
};

// function creates message object from
Message* parseJSONtoMessage( Datagram* dgram );

#endif /* NETWORK_MESSAGE_H_ */
