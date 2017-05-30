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
#include <fileinfo.h>

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
private:
	std::string requested_file;
	int resp_port;

public:

	MessageREQFILE(const char* sender_ipv4, std::string sender_nick,
			std::string filename, int resp_port);
	MessageREQFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageREQFILE();

	// generates json format of message
	virtual std::string jsonify();

	std::string getRequestedFile() {
		return requested_file;
	}

	int getRespPort() {
		return resp_port;
	}
};

class MessageRESPFILE: public Message {
private:
	std::string have_file;

public:

	MessageRESPFILE(const char* sender_ipv4, std::string sender_nick,
			std::string have_file);
	MessageRESPFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageRESPFILE();

	// generates json format of message
	virtual std::string jsonify();

	std::string getHaveFile() {
		return have_file;
	}
};

class MessageREQFDATA: public Message {
private:
	std::string requested_file;
	unsigned long offset, size;
	int wait_tcp_port;

public:

	MessageREQFDATA(const char* sender_ipv4,
			std::string sender_nick,
			std::string req_file,
			int wait_tcp_port,
			unsigned long offset,
			unsigned long size);
	MessageREQFDATA(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageREQFDATA();

	// generates json format of message
	virtual std::string jsonify();

	unsigned long getOffset() {
		return offset;
	}

	std::string getRequestedFile() {
		return requested_file;
	}

	unsigned long getSize() {
		return size;
	}

	int getWaitTcpPort() {
		return wait_tcp_port;
	}
};

class MessageADDFILE: public Message {
private:
	FileInfo added_file;

public:

	MessageADDFILE(const char* sender_ipv4, std::string sender_nick,
			FileInfo file);
	MessageADDFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageADDFILE();

	// generates json format of message
	virtual std::string jsonify();

	FileInfo getAddedFile() {
		return added_file;
	}
};

class MessageDELFILE: public Message {
private:
	FileInfo deleted_file;

public:

	MessageDELFILE(const char* sender_ipv4, std::string sender_nick,
			FileInfo file);
	MessageDELFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageDELFILE();

	// generates json format of message
	virtual std::string jsonify();

	FileInfo getDeletedFile() {
		return deleted_file;
	}
};

class MessageREVFILE: public Message {
private:
	std::string rev_file;

public:

	MessageREVFILE(const char* sender_ipv4,
			std::string sender_nick, std::string rev_file);
	MessageREVFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageREVFILE();

	// generates json format of message
	virtual std::string jsonify();

	std::string getRevFile() {
		return rev_file;
	}
};

class MessageLOCFILE: public Message {
private:
	std::string lck_file;

public:

	MessageLOCFILE(const char* sender_ipv4,
			std::string sender_nick, std::string lck_file);
	MessageLOCFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageLOCFILE();

	// generates json format of message
	virtual std::string jsonify();

	std::string getLckFile() {
		return lck_file;
	}
};

class MessageUNLOCFILE: public Message {
private:
	std::string unlck_file;

public:

	MessageUNLOCFILE(const char* sender_ipv4,
			std::string sender_nick, std::string unlck_file);
	MessageUNLOCFILE(const char* sender_ipv4, Json::Value& json);
	virtual ~MessageUNLOCFILE();

	// generates json format of message
	virtual std::string jsonify();

	std::string getUnlckFile() {
		return unlck_file;
	}
};

// function creates message object from
Message* parseJSONtoMessage( Datagram* dgram );

#endif /* NETWORK_MESSAGE_H_ */
