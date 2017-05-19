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

	// generates json format of message
	virtual const char* jsonify() const = 0;
};

class MessageGREETING: public Message {
public:

	MessageGREETING();
	virtual ~MessageGREETING();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageRESPLIST: public Message {
public:

	MessageRESPLIST();
	virtual ~MessageRESPLIST();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageREQLIST: public Message {
public:

	MessageREQLIST();
	virtual ~MessageREQLIST();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageREQFILE: public Message {
public:

	MessageREQFILE();
	virtual ~MessageREQFILE();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageRESPFILE: public Message {
public:

	MessageRESPFILE();
	virtual ~MessageRESPFILE();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageREQFDATA: public Message {
public:

	MessageREQFDATA();
	virtual ~MessageREQFDATA();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageADDFILE: public Message {
public:

	MessageADDFILE();
	virtual ~MessageADDFILE();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageDELFILE: public Message {
public:

	MessageDELFILE();
	virtual ~MessageDELFILE();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageREVFILE: public Message {
public:

	MessageREVFILE();
	virtual ~MessageREVFILE();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageLOCFILE: public Message {
public:

	MessageLOCFILE();
	virtual ~MessageLOCFILE();

	// generates json format of message
	virtual const char* jsonify() override;
};

class MessageUNLOCFILE: public Message {
public:

	MessageUNLOCFILE();
	virtual ~MessageUNLOCFILE();

	// generates json format of message
	virtual const char* jsonify() override;
};

#endif /* NETWORK_MESSAGE_H_ */
