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

#include<string>

using namespace std;

class Message {
public:
	// message type: GREETING, RESPFILE, ...
	string type;

	Message(string type);
	virtual ~Message();
};

class MessageGREETING: public Message {
public:

	MessageGREETING();
	virtual ~MessageGREETING();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageRESPLIST: public Message {
public:

	MessageRESPLIST();
	virtual ~MessageRESPLIST();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREQLIST: public Message {
public:

	MessageREQLIST();
	virtual ~MessageREQLIST();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREQFILE: public Message {
public:

	MessageREQFILE();
	virtual ~MessageREQFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageRESPFILE: public Message {
public:

	MessageRESPFILE();
	virtual ~MessageRESPFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREQFDATA: public Message {
public:

	MessageREQFDATA();
	virtual ~MessageREQFDATA();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageADDFILE: public Message {
public:

	MessageADDFILE();
	virtual ~MessageADDFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageDELFILE: public Message {
public:

	MessageDELFILE();
	virtual ~MessageDELFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageREVFILE: public Message {
public:

	MessageREVFILE();
	virtual ~MessageREVFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageLOCFILE: public Message {
public:

	MessageLOCFILE();
	virtual ~MessageLOCFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

class MessageUNLOCFILE: public Message {
public:

	MessageUNLOCFILE();
	virtual ~MessageUNLOCFILE();

	// generates json format of message
	virtual const char* jsonify() const;
};

// function creates message object from
Message* parseJSONtoMessage(string json);

#endif /* NETWORK_MESSAGE_H_ */
