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

bool Monitor::ping(unsigned int interval, unsigned int sqn,
		unsigned int tokens) noexcept {
	if (edge.latency == 0) {
		return call(edge.id, sqn, tokens);
	} else {
		tokens = ((interval * 1.25) / edge.latency) + 2;
		return call(edge.id, sqn, tokens);
	}
}

bool Monitor::call(unsigned long long id, unsigned int sqn,
		unsigned int tokens) noexcept {
	if (id == getUid()) {
		return false;
	}

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

bool Monitor::connect(const Message *message) noexcept {
	if (!((message) && message->checkContext(0, 0, WH_AQLF_ACCEPTED)
			&& message->getPayloadLength() == sizeof(uint32_t))) {
		return false;
	}

	auto id = message->getSource();
	auto sqn = message->getSequenceNumber();
	auto latency = message->getData32(0);
	if ((id != getUid()) && (id == edge.id) && (sqn == edge.sqn)) {
		edge.latency = latency;
		return true;
	} else {
		return false;
	}
}

bool Monitor::target(unsigned long long id, unsigned int latency) noexcept {
	if (id != getUid()) {
		edge = { id, latency, 0 };
		return true;
	} else {
		end();
		return false;
	}
}

unsigned long long Monitor::target() const noexcept {
	return edge.id;
}

unsigned int Monitor::latency() const noexcept {
	return edge.latency;
}

void Monitor::end() noexcept {
	edge = { getUid(), 0, 0 };
}

bool Monitor::subscribe(unsigned int topic) noexcept {
	if (topic > Topic::MAX_ID) {
		return false;
	}

	auto message = Message::create();
	if (message) {
		MessageHeader header;
		header.setAddress(0, 0);
		header.setControl(Message::HLEN, 0, topic);
		header.setContext(WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE, WH_AQLF_REQUEST);
		message->putHeader(header);
		return forward(message);
	} else {
		return false;
	}
}

bool Monitor::unsubscribe(unsigned int topic) noexcept {
	if (topic > Topic::MAX_ID) {
		return true;
	}

	auto message = Message::create();
	if (message) {
		MessageHeader header;
		header.setAddress(0, 0);
		header.setControl(Message::HLEN, 0, topic);
		header.setContext(WH_CMD_MULTICAST, WH_QLF_UNSUBSCRIBE,
				WH_AQLF_REQUEST);
		message->putHeader(header);
		return forward(message);
	} else {
		return false;
	}
}

bool Monitor::subscribe(const Message *message,
		unsigned int &topic) const noexcept {
	if (message
			&& message->checkContext(WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE,
					WH_AQLF_ACCEPTED)) {
		topic = message->getSession();
		return true;
	} else {
		return false;
	}
}

bool Monitor::unsubscribe(const Message *message,
		unsigned int &topic) const noexcept {
	if (message
			&& message->checkContext(WH_CMD_MULTICAST, WH_QLF_UNSUBSCRIBE,
					WH_AQLF_ACCEPTED)) {
		topic = message->getSession();
		return true;
	} else {
		return false;
	}
}

void Monitor::setup() {

}

void Monitor::clear() noexcept {
	end();
}

} /* namespace wanhive */
