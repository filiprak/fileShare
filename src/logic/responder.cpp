/*
 * responder.cpp
 *
 *  Created on: 24.05.2017
 *      Author: raqu
 */

#include "responder.h"

Responder::Responder(BlockingQueue<Message>& q) : messq(q) {
}

Responder::~Responder() {
	// TODO Auto-generated destructor stub
}

void Responder::run() {
}

void Responder::stop() {
}

void responseGREETINGThread(MessageGREETING& mess) {
}

void responseREQUFILEThread(MessageREQFILE& mess) {
}

void responseREQFDATAThread(MessageREQFDATA& mess) {
}

void responseREQLISTThread(MessageREQLIST& mess) {
}

void responseADDFILEThread(MessageADDFILE& mess) {
}

void responseDELFILEThread(MessageDELFILE& mess) {
}

void responseREVFILEThread(MessageREVFILE& mess) {
}

void responseLOCFILEThread(MessageLOCFILE& mess) {
}

void responseUNLOCFILEThread(MessageUNLOCFILE& mess) {
}

void responderThread(Responder& resp) {
}
