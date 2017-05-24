/*
 * message.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "message.h"
#include <string>
#include <json/json.h>
#include <iostream>

// json property names
#define TAG_TYPE			"type"
#define TAG_SENDER			"sender"
#define TAG_FILENAME		"file"
#define TAG_FILELIST		"flist"


Message::Message(MSG_TYPE type, const char* sender_ipv4) {
	this->type = type;
	this->sender_ipv4 = std::string(sender_ipv4);
}

Message::~Message() {
	// TODO Auto-generated destructor stub
}

MessageGREETING::MessageGREETING(const char* sender_ipv4, std::string sender)
	: Message(GREETING, sender_ipv4), sender(sender) {
}

MessageGREETING::~MessageGREETING() {
}

std::string MessageGREETING::jsonify() {
	Json::Value root;
	root[TAG_TYPE] = type;
	root[TAG_SENDER] = sender.c_str();

	return fast_writer.write(root);
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


Message* parseJSONtoMessage( const Datagram* dgram ) {
	return new MessageGREETING("12312", "hostname");
}
