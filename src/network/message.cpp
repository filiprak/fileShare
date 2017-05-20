/*
 * message.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include "message.h"

Message::Message(string type) {
	this->type = type;
}

Message::~Message() {
	// TODO Auto-generated destructor stub
}

MessageGREETING::MessageGREETING() : Message("GREETING"){
}

MessageGREETING::~MessageGREETING() {
}

const char* MessageGREETING::jsonify() const {
}

MessageRESPLIST::MessageRESPLIST() : Message("RESPLIST") {
}

MessageRESPLIST::~MessageRESPLIST() {
}

const char* MessageRESPLIST::jsonify() const {
}

MessageREQLIST::MessageREQLIST() : Message("REQLIST") {
}

MessageREQLIST::~MessageREQLIST() {
}

const char* MessageREQLIST::jsonify() const {
}

MessageREQFILE::MessageREQFILE() : Message("REQFILE") {
}

MessageREQFILE::~MessageREQFILE() {
}

const char* MessageREQFILE::jsonify() const {
}

MessageRESPFILE::MessageRESPFILE() : Message("RESPFILE") {
}

MessageRESPFILE::~MessageRESPFILE() {
}

const char* MessageRESPFILE::jsonify() const {
}

MessageREQFDATA::MessageREQFDATA() : Message("REQFDATA") {
}

MessageREQFDATA::~MessageREQFDATA() {
}

const char* MessageREQFDATA::jsonify() const {
}

MessageADDFILE::MessageADDFILE() : Message("ADDFILE") {
}

MessageADDFILE::~MessageADDFILE() {
}

const char* MessageADDFILE::jsonify() const {
}

MessageDELFILE::MessageDELFILE() : Message("DELFILE") {
}

MessageDELFILE::~MessageDELFILE() {
}

const char* MessageDELFILE::jsonify() const {
}

MessageREVFILE::MessageREVFILE() : Message("REVFILE") {
}

MessageREVFILE::~MessageREVFILE() {
}

const char* MessageREVFILE::jsonify() const {
}

MessageLOCFILE::MessageLOCFILE() : Message("LOCFILE") {
}

MessageLOCFILE::~MessageLOCFILE() {
}

const char* MessageLOCFILE::jsonify() const {
}

MessageUNLOCFILE::MessageUNLOCFILE() : Message("UNLOCFILE") {
}

MessageUNLOCFILE::~MessageUNLOCFILE() {
}

const char* MessageUNLOCFILE::jsonify() const {
}

Message* parseJSONtoMessage(string json) {
}
