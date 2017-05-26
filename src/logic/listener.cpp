/*
 * listener.cpp
 *
 *  Created on: 17.05.2017
 *      Author: raqu
 */

#include <common.h>
#include <datagram.h>
#include <listener.h>
#include <logger.h>

Listener::Listener(BlockingQueue< Message* >& q) : messq(q)  {
}

Listener::~Listener(){
}

int Listener::run() {
	console->info("Starting listener...");
	network.listenUDP(LISTENER_TIMEOUT, 0, LISTENER_PORT);
	console->info("Stopped listener...");
	return 0;
}

void Listener::parse() {
	BlockingQueue< Datagram* >& qdgrams = network.getUdplisten().getReceivedUdPs();
	console->info("Starting message parser...");
	parsing = true;
	while( parsing ) {
		Datagram* dgram = qdgrams.take();
		if ( !network.getUdplisten().isListening() ) {
			delete dgram;
			break;
		}
		Message* parsed = parseJSONtoMessage( dgram );
		// if parsing succeded
		if (parsed != nullptr)
			messq.insert(parsed);
	}
	parsing = false;
	console->info("Stopped message parser...");

}

void Listener::stop() {
	network.getUdplisten().stop();
	parsing = false;

	Datagram* poison_pill = new Datagram("pill", "pill", 2);
	network.getUdplisten().getReceivedUdPs().insert(poison_pill);

	MessageGREETING poison_pill2("pill", "pill");
	network.sendUDP(&poison_pill2, Network::getMyIpv4Addr(), LISTENER_PORT);
	//clear message queue
	while( messq.notEmpty() ) {
		delete messq.take();
	}
}

void Listener::clearQueues() {
	network.getUdplisten().clearReceivedUDPs();

	//clear message queue
	while( messq.notEmpty() ) {
		delete messq.take();
	}
}

bool Listener::isListening() {
	return network.getUdplisten().isListening() && parsing;
}

void listenerThread(Listener& listener) {
	try {
		listener.run();
	} catch (const std::exception &e) {
		console->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
}

void parserThread(Listener& listener) {
	try {
		listener.parse();
	} catch (const std::exception &e) {
		console->error( "Exception in: '{}': {}", __FUNCTION__, e.what() );
	}
}

