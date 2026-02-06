/*
 * Monitor.cpp
 *
 * Monitoring Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Monitor.h"
#include "../base/common/Logger.h"
#include "../util/commands.h"

namespace wanhive {

Monitor::Monitor(unsigned long long uid, const char *path) noexcept :
		Agent { uid, path } {

}

Monitor::~Monitor() {

}

void Monitor::configure(void *arg) {
	try {
		Agent::configure(arg);
		WH_LOG_DEBUG("Setting things up...");
		setup();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		throw Exception(EX_OPERATION);
	}
}

void Monitor::cleanup() noexcept {
	clear();
	Agent::cleanup();
}

bool Monitor::engage(unsigned long long id, unsigned int sqn,
		unsigned int tokens) noexcept {
	auto message = Message::create();
	if (message) {
		edge.sqn = sqn;
		edge.latency = (edge.id == id) ? edge.latency : 0;
		edge.id = id;
		MessageHeader header;
		header.setAddress(0, id);
		header.setControl(Message::HLEN, edge.sqn, 0);
		header.setContext(0, 0, WH_AQLF_REQUEST);
		message->putHeader(header);
		message->appendData32(tokens);
		message->setDestination(0);
		return forward(message);
	} else {
		return false;
	}
}

bool Monitor::connect(Message *message) noexcept {
	if (message && message->getPayloadLength() >= sizeof(uint32_t)) {
		auto id = message->getSource();
		auto sqn = message->getSequenceNumber();
		auto latency = message->getData32(0);
		return connect(id, sqn, latency);
	} else {
		return false;
	}
}

void Monitor::revoke() noexcept {
	edge = { getUid(), 0, 0 };
}

unsigned long long Monitor::getPeer() const noexcept {
	return edge.id;
}

void Monitor::setPeer(unsigned long long peer) noexcept {
	edge.id = peer;
}

unsigned int Monitor::getLatency() const noexcept {
	return edge.latency;
}

void Monitor::setLatency(unsigned int latency) noexcept {
	edge.latency = latency;
}

bool Monitor::connect(unsigned long long id, unsigned int sqn,
		unsigned int latency) noexcept {
	if (id == edge.id && sqn == edge.sqn) {
		edge.latency = latency;
		return true;
	} else {
		return false;
	}
}

void Monitor::setup() {

}

void Monitor::clear() noexcept {
	revoke();
}

} /* namespace wanhive */
