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
#define TAG_FILE			"file"
#define TAG_FILELIST		"flist"
#define TAG_RESPORT			"resp-port"


Json::FastWriter Message::fast_writer;

Message::Message(MSG_TYPE type, const char* sender_ipv4, std::string sender_nick) {
	this->type = type;
	this->sender_ipv4 = std::string(sender_ipv4);
	this->sender_nick = sender_nick;
}

Message::~Message() {
	// TODO Auto-generated destructor stub
}

MessageGREETING::MessageGREETING(const char* sender_ipv4, std::string sender_nick, unsigned rport)
	: Message(GREETING, sender_ipv4, sender_nick), resp_port(rport) {

}

MessageGREETING::~MessageGREETING() {
}

std::string MessageGREETING::jsonify() {
	Json::Value root;
	if (resp_port > 0)
			root[TAG_RESPORT] = resp_port;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	return Message::fast_writer.write(root);
}

MessageRESPLIST::MessageRESPLIST(const char* sender_ipv4, std::string sender_nick, Json::Value& jlist)
	: Message(RESPLIST, sender_ipv4, sender_nick), jlist(jlist) {
}

MessageRESPLIST::~MessageRESPLIST() {
}

std::string MessageRESPLIST::jsonify() {
	Json::Value root;
	root[TAG_FILELIST] = jlist;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	return Message::fast_writer.write(root);
}

MessageREQLIST::MessageREQLIST(const char* sender_ipv4, std::string sender_nick)
	: Message(REQLIST, sender_ipv4, sender_nick) {

}

MessageREQLIST::~MessageREQLIST() {
}

std::string MessageREQLIST::jsonify() {
	Json::Value root;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	return Message::fast_writer.write(root);
}

MessageREQFILE::MessageREQFILE(const char* sender_ipv4, std::string sender_nick)
	: Message(REQFILE, sender_ipv4, sender_nick) {

}

MessageREQFILE::~MessageREQFILE() {
}

std::string MessageREQFILE::jsonify() {
}

MessageRESPFILE::MessageRESPFILE(const char* sender_ipv4, std::string sender_nick)
	: Message(RESPFILE, sender_ipv4, sender_nick) {

}

MessageRESPFILE::~MessageRESPFILE() {
}

std::string MessageRESPFILE::jsonify() {
}

MessageREQFDATA::MessageREQFDATA(const char* sender_ipv4, std::string sender_nick)
	: Message(REQFDATA, sender_ipv4, sender_nick) {

}

MessageREQFDATA::~MessageREQFDATA() {
}

std::string MessageREQFDATA::jsonify() {
}

MessageADDFILE::MessageADDFILE(const char* sender_ipv4,
		std::string sender_nick, FileInfo file)
	: Message(ADDFILE, sender_ipv4, sender_nick) {
	added_file = file;
}

MessageADDFILE::~MessageADDFILE() {
}

std::string MessageADDFILE::jsonify() {
	Json::Value root;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	root[TAG_FILE] = added_file.jsonify();
	return Message::fast_writer.write(root);
}

MessageDELFILE::MessageDELFILE(const char* sender_ipv4,
		std::string sender_nick, FileInfo file)
	: Message(DELFILE, sender_ipv4, sender_nick) {
	deleted_file = file;
}

MessageDELFILE::~MessageDELFILE() {
}

std::string MessageDELFILE::jsonify() {
	Json::Value root;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	root[TAG_FILE] = deleted_file.jsonify();
	return Message::fast_writer.write(root);
}

MessageREVFILE::MessageREVFILE(const char* sender_ipv4,
		std::string sender_nick, std::string rev_file)
	: Message(REVFILE, sender_ipv4, sender_nick), rev_file(rev_file) {

}

MessageREVFILE::~MessageREVFILE() {
}

std::string MessageREVFILE::jsonify() {
	Json::Value root;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	root[TAG_FILE] = rev_file.c_str();
	return Message::fast_writer.write(root);
}

MessageLOCFILE::MessageLOCFILE(const char* sender_ipv4,
		std::string sender_nick, std::string lck_file)
	: Message(LOCFILE, sender_ipv4, sender_nick), lck_file(lck_file) {

}

MessageLOCFILE::~MessageLOCFILE() {
}

std::string MessageLOCFILE::jsonify() {
	Json::Value root;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	root[TAG_FILE] = lck_file.c_str();
	return Message::fast_writer.write(root);
}

MessageUNLOCFILE::MessageUNLOCFILE(const char* sender_ipv4,
		std::string sender_nick, std::string unlck_file)
	: Message(UNLOCFILE, sender_ipv4, sender_nick), unlck_file(unlck_file) {
}

MessageUNLOCFILE::~MessageUNLOCFILE() {
}

std::string MessageUNLOCFILE::jsonify() {
	Json::Value root;
	root[TAG_SENDER] = sender_nick.c_str();
	root[TAG_TYPE] = type;
	root[TAG_FILE] = unlck_file.c_str();
	return Message::fast_writer.write(root);
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
		logger->warn("Failed parsing of message: {} from {}, errors: {}",
				dgram->getBytes(), dgram->getSender(), errs );
	}

	Message* message = nullptr;
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
				logger->warn("Received message of unknown type: {}", parsed[TAG_TYPE].asInt() );
				break;
		}
	}
	delete dgram;
	return (Message*) message;
}


/* json constructors for messages */
MessageGREETING::MessageGREETING(const char* sender_ipv4, Json::Value& json)
	: Message(GREETING, sender_ipv4, json[TAG_SENDER].asString()) {
	if ( json.isMember(TAG_RESPORT) )
		resp_port = json[TAG_RESPORT].asUInt();
	else {
		resp_port = 0;
	}
}

MessageRESPLIST::MessageRESPLIST(const char* sender_ipv4, Json::Value& json)
	: Message(RESPLIST, sender_ipv4, json[TAG_SENDER].asString()) {
	jlist = json[TAG_FILELIST];
}

MessageREQLIST::MessageREQLIST(const char* sender_ipv4, Json::Value& json)
	: Message(REQLIST, sender_ipv4, json[TAG_SENDER].asString()) {
}

MessageREQFILE::MessageREQFILE(const char* sender_ipv4, Json::Value& json)
	: Message(REQFILE, sender_ipv4, json[TAG_SENDER].asString()) {
}

MessageRESPFILE::MessageRESPFILE(const char* sender_ipv4, Json::Value& json)
	: Message(RESPFILE, sender_ipv4, json[TAG_SENDER].asString()) {
}

MessageREQFDATA::MessageREQFDATA(const char* sender_ipv4, Json::Value& json)
	: Message(REQFDATA, sender_ipv4, json[TAG_SENDER].asString()) {
}

MessageADDFILE::MessageADDFILE(const char* sender_ipv4, Json::Value& json)
	: Message(ADDFILE, sender_ipv4, json[TAG_SENDER].asString()) {
	added_file = FileInfo(json[TAG_FILE]);
}

MessageDELFILE::MessageDELFILE(const char* sender_ipv4, Json::Value& json)
	: Message(DELFILE, sender_ipv4, json[TAG_SENDER].asString()) {
	deleted_file = FileInfo(json[TAG_FILE]);
}

MessageREVFILE::MessageREVFILE(const char* sender_ipv4, Json::Value& json)
	: Message(REVFILE, sender_ipv4, json[TAG_SENDER].asString()) {
	rev_file = json[TAG_FILE].asString();
}

MessageLOCFILE::MessageLOCFILE(const char* sender_ipv4, Json::Value& json)
	: Message(LOCFILE, sender_ipv4, json[TAG_SENDER].asString()) {
	lck_file = json[TAG_FILE].asString();
}

MessageUNLOCFILE::MessageUNLOCFILE(const char* sender_ipv4, Json::Value& json)
	: Message(UNLOCFILE, sender_ipv4, json[TAG_SENDER].asString()) {
	unlck_file = json[TAG_FILE].asString();
}
