/*
 * message.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "message.h"
#include <string>

Message::Message(MSG_TYPE type, const char* sender_ipv4) {
	this->type = type;
	this->sender_ipv4 = std::string(sender_ipv4);
}

Message::~Message() {
	// TODO Auto-generated destructor stub
}

MessageGREETING::MessageGREETING(const char* sender_ipv4) : Message(GREETING, sender_ipv4){

}

MessageGREETING::~MessageGREETING() {
}

const char* MessageGREETING::jsonify() const {
}

MessageRESPLIST::MessageRESPLIST(const char* sender_ipv4) : Message(RESPLIST, sender_ipv4) {

}

MessageRESPLIST::~MessageRESPLIST() {
}

const char* MessageRESPLIST::jsonify() const {
}

MessageREQLIST::MessageREQLIST(const char* sender_ipv4) : Message(REQLIST, sender_ipv4) {

}

MessageREQLIST::~MessageREQLIST() {
}

const char* MessageREQLIST::jsonify() const {
}

MessageREQFILE::MessageREQFILE(const char* sender_ipv4) : Message(REQFILE, sender_ipv4) {

}

MessageREQFILE::~MessageREQFILE() {
}

const char* MessageREQFILE::jsonify() const {
}

MessageRESPFILE::MessageRESPFILE(const char* sender_ipv4) : Message(RESPFILE, sender_ipv4) {

}

MessageRESPFILE::~MessageRESPFILE() {
}

const char* MessageRESPFILE::jsonify() const {
}

MessageREQFDATA::MessageREQFDATA(const char* sender_ipv4) : Message(REQFDATA, sender_ipv4) {

}

MessageREQFDATA::~MessageREQFDATA() {
}

const char* MessageREQFDATA::jsonify() const {
}

MessageADDFILE::MessageADDFILE(const char* sender_ipv4) : Message(ADDFILE, sender_ipv4) {

}

MessageADDFILE::~MessageADDFILE() {
}

const char* MessageADDFILE::jsonify() const {
}

MessageDELFILE::MessageDELFILE(const char* sender_ipv4) : Message(DELFILE, sender_ipv4) {

}

MessageDELFILE::~MessageDELFILE() {
}

const char* MessageDELFILE::jsonify() const {
}

MessageREVFILE::MessageREVFILE(const char* sender_ipv4) : Message(REVFILE, sender_ipv4) {

}

MessageREVFILE::~MessageREVFILE() {
}

const char* MessageREVFILE::jsonify() const {
}

MessageLOCFILE::MessageLOCFILE(const char* sender_ipv4) : Message(LOCFILE, sender_ipv4) {

}

MessageLOCFILE::~MessageLOCFILE() {
}

const char* MessageLOCFILE::jsonify() const {
}

MessageUNLOCFILE::MessageUNLOCFILE(const char* sender_ipv4) : Message(UNLOCFILE, sender_ipv4) {

}

MessageUNLOCFILE::~MessageUNLOCFILE() {
}

const char* MessageUNLOCFILE::jsonify() const {
}


Message parseJSONtoMessage( const Datagram& dgram ) {
	return MessageGREETING( dgram.getSender() );
}
