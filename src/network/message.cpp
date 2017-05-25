/*
 * message.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <datagram.h>
#include <logger.h>
#include <message.h>
#include <exception>
#include <iostream>

// json property names
#define TAG_TYPE			"type"
#define TAG_SENDER			"sender"
#define TAG_FILENAME		"file"
#define TAG_FILELIST		"flist"
#define TAG_RESPORT			"port-nr"


Json::FastWriter Message::fast_writer;

Message::Message(MSG_TYPE type, const char* sender_ipv4) {
	this->type = type;
	this->sender_ipv4 = std::string(sender_ipv4);
}

Message::~Message() {
	// TODO Auto-generated destructor stub
}

MessageGREETING::MessageGREETING(const char* sender_ipv4, std::string sender, unsigned rport)
	: Message(GREETING, sender_ipv4), sender(sender), resp_port(rport) {
}

MessageGREETING::~MessageGREETING() {
}

std::string MessageGREETING::jsonify() {
	Json::Value root;
	root[TAG_TYPE] = type;
	root[TAG_SENDER] = sender.c_str();
	if (resp_port > 0)
		root[TAG_RESPORT] = resp_port;
	return Message::fast_writer.write(root);
}

MessageRESPLIST::MessageRESPLIST(const char* sender_ipv4) : Message(RESPLIST, sender_ipv4) {

}

MessageRESPLIST::~MessageRESPLIST() {
}

std::string MessageRESPLIST::jsonify() {
}

MessageREQLIST::MessageREQLIST(const char* sender_ipv4) : Message(REQLIST, sender_ipv4) {

}

MessageREQLIST::~MessageREQLIST() {
}

std::string MessageREQLIST::jsonify() {
}

MessageREQFILE::MessageREQFILE(const char* sender_ipv4) : Message(REQFILE, sender_ipv4) {

}

MessageREQFILE::~MessageREQFILE() {
}

std::string MessageREQFILE::jsonify() {
}

MessageRESPFILE::MessageRESPFILE(const char* sender_ipv4) : Message(RESPFILE, sender_ipv4) {

}

MessageRESPFILE::~MessageRESPFILE() {
}

std::string MessageRESPFILE::jsonify() {
}

MessageREQFDATA::MessageREQFDATA(const char* sender_ipv4) : Message(REQFDATA, sender_ipv4) {

}

MessageREQFDATA::~MessageREQFDATA() {
}

std::string MessageREQFDATA::jsonify() {
}

MessageADDFILE::MessageADDFILE(const char* sender_ipv4) : Message(ADDFILE, sender_ipv4) {

}

MessageADDFILE::~MessageADDFILE() {
}

std::string MessageADDFILE::jsonify() {
}

MessageDELFILE::MessageDELFILE(const char* sender_ipv4) : Message(DELFILE, sender_ipv4) {

}

MessageDELFILE::~MessageDELFILE() {
}

std::string MessageDELFILE::jsonify() {
}

MessageREVFILE::MessageREVFILE(const char* sender_ipv4) : Message(REVFILE, sender_ipv4) {

}

MessageREVFILE::~MessageREVFILE() {
}

std::string MessageREVFILE::jsonify() {
}

MessageLOCFILE::MessageLOCFILE(const char* sender_ipv4) : Message(LOCFILE, sender_ipv4) {

}

MessageLOCFILE::~MessageLOCFILE() {
}

std::string MessageLOCFILE::jsonify() {
}

MessageUNLOCFILE::MessageUNLOCFILE(const char* sender_ipv4) : Message(UNLOCFILE, sender_ipv4) {

}

MessageUNLOCFILE::~MessageUNLOCFILE() {
}

std::string MessageUNLOCFILE::jsonify() {
}


Message* parseJSONtoMessage( Datagram* dgram ) {
	static Json::CharReaderBuilder rbuilder;
	Json::Value parsed;
	std::string errs;
	std::istringstream os( dgram->getBytes() );
	bool res;

	try {
		res = Json::parseFromStream(rbuilder, os, &parsed, &errs);
	} catch(const std::exception &e) {
		res = false;
		console->warn("Failed parsing of message: {} from {}, errors: {}",
				dgram->getBytes(), dgram->getSender(), errs );
	}

	Message* message = nullptr;
	console->info("Parsing message results: success: {}, json: {}", res, parsed.toStyledString() );
	if (res) {
		// message parsed successfuly

		switch( parsed[TAG_TYPE].asInt() ) {
			case GREETING:
				message = new MessageGREETING(dgram->getSender(), parsed);
				break;
			case REQLIST:
				message = new MessageREQLIST(dgram->getSender(), parsed);
				break;
			case RESPLIST:
				message = new MessageRESPLIST(dgram->getSender(), parsed);
				break;
			case REQFILE:
				message = new MessageREQFILE(dgram->getSender(), parsed);
				break;
			case RESPFILE:
				message = new MessageRESPFILE(dgram->getSender(), parsed);
				break;
			case REQFDATA:
				message = new MessageREQFDATA(dgram->getSender(), parsed);
				break;
			case ADDFILE:
				message = new MessageADDFILE(dgram->getSender(), parsed);
				break;
			case DELFILE:
				message = new MessageDELFILE(dgram->getSender(), parsed);
				break;
			case REVFILE:
				message = new MessageREVFILE(dgram->getSender(), parsed);
				break;
			case LOCFILE:
				message = new MessageLOCFILE(dgram->getSender(), parsed);
				break;
			case UNLOCFILE:
				message = new MessageUNLOCFILE(dgram->getSender(), parsed);
				break;
			default:
				//ignore other message types
				console->warn("Received message of unknown type: {}", parsed[TAG_TYPE].asInt() );
				break;
		}
	}
	delete dgram;
	return (Message*) message;
}


/* json constructors for messages */
MessageGREETING::MessageGREETING(const char* sender_ipv4, Json::Value& json)
	: Message(GREETING, sender_ipv4) {
	sender = std::string( json[TAG_SENDER].asCString() );
	if ( json.isMember(TAG_RESPORT) )
		resp_port = json[TAG_RESPORT].asUInt();
	else {
		resp_port = 0;
	}
}

MessageRESPLIST::MessageRESPLIST(const char* sender_ipv4, Json::Value& json)
	: Message(RESPLIST, sender_ipv4) {
}

MessageREQLIST::MessageREQLIST(const char* sender_ipv4, Json::Value& json)
	: Message(REQLIST, sender_ipv4) {
}

MessageREQFILE::MessageREQFILE(const char* sender_ipv4, Json::Value& json)
	: Message(REQFILE, sender_ipv4) {
}

MessageRESPFILE::MessageRESPFILE(const char* sender_ipv4, Json::Value& json)
	: Message(RESPFILE, sender_ipv4) {
}

MessageREQFDATA::MessageREQFDATA(const char* sender_ipv4, Json::Value& json)
	: Message(REQFDATA, sender_ipv4) {
}

MessageADDFILE::MessageADDFILE(const char* sender_ipv4, Json::Value& json)
	: Message(ADDFILE, sender_ipv4) {
}

MessageDELFILE::MessageDELFILE(const char* sender_ipv4, Json::Value& json)
	: Message(DELFILE, sender_ipv4) {
}

MessageREVFILE::MessageREVFILE(const char* sender_ipv4, Json::Value& json)
	: Message(REVFILE, sender_ipv4) {
}

MessageLOCFILE::MessageLOCFILE(const char* sender_ipv4, Json::Value& json)
	: Message(LOCFILE, sender_ipv4) {
}

MessageUNLOCFILE::MessageUNLOCFILE(const char* sender_ipv4, Json::Value& json)
	: Message(UNLOCFILE, sender_ipv4) {
}
