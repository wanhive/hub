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

bool Monitor::heartbeat(unsigned int interval, unsigned int sqn,
		unsigned int tokens) noexcept {
	if (!interval) {
		return false;
	} else if (edge.latency == 0) {
		return invite(edge.id, sqn, tokens);
	} else {
		tokens = ((interval * 1.25) / edge.latency) + 2;
		return invite(edge.id, sqn, tokens);
	}
}

bool Monitor::invite(unsigned long long id, unsigned int sqn,
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
	if (!((message) && message->checkContext(0, 0, WH_AQLF_ACCEPTED)
			&& message->getPayloadLength() == sizeof(uint32_t))) {
		return false;
	}

	auto id = message->getSource();
	auto sqn = message->getSequenceNumber();
	auto latency = message->getData32(0);
	if (id == edge.id && sqn == edge.sqn) {
		edge.latency = latency;
		return true;
	} else {
		return false;
	}
}

bool Monitor::target(unsigned long long id, unsigned int latency) noexcept {
	edge = { id, latency, 0 };
	return true;
}

unsigned long long Monitor::host() const noexcept {
	return edge.id;
}

unsigned int Monitor::latency() const noexcept {
	return edge.latency;
}

void Monitor::teardown() noexcept {
	edge = { getUid(), 0, 0 };
}

void Monitor::setup() {

}

void Monitor::clear() noexcept {
	teardown();
}

} /* namespace wanhive */
