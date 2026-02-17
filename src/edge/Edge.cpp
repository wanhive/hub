/*
 * Edge.cpp
 *
 * Edge Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Edge.h"
#include "../base/common/Logger.h"
#include "../base/unix/Time.h"
#include "../util/commands.h"

namespace wanhive {

Edge::Edge(unsigned long long uid, const char *path) noexcept :
		Agent { uid, path } {
	clear();

}

Edge::~Edge() {

}

void Edge::configure(void *arg) {
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

void Edge::cleanup() noexcept {
	clear();
	Agent::cleanup();
}

void Edge::pair(bool enable) noexcept {
	ctx.pair = enable;
	if (!enable) {
		end();
	}
}

bool Edge::pair() const noexcept {
	return ctx.pair;
}

bool Edge::accept(Message *message, unsigned int interval) noexcept {
	auto success = (message) && message->checkContext(0, 0, WH_AQLF_REQUEST)
			&& (message->getPayloadLength() == sizeof(uint32_t))
			&& accept(message->getSource(), message->getData32(0));
	if (success) {
		WH_LOG_DEBUG("Node %llu requested %u tokens", meta.id, meta.tokens);
		message->setData32(0, interval);
		message->putLength(Message::HLEN + sizeof(uint32_t));
		message->writeDestination(meta.id);
		message->setDestination(0);
		message->putStatus(WH_AQLF_ACCEPTED);
		return true;
	} else {
		return false;
	}
}

bool Edge::accept(unsigned long long id, unsigned int tokens) noexcept {
	if (pair() && (id != getUid())) {
		meta = { id, tokens };
		return true;
	} else {
		end();
		return false;
	}
}

void Edge::end() noexcept {
	meta = { getUid(), 0 };
}

bool Edge::access() noexcept {
	if (paired()) {
		meta.tokens -= 1;
		return true;
	} else {
		end();
		return false;
	}
}

bool Edge::paired() const noexcept {
	return ((meta.id != getUid()) && (meta.tokens != 0));
}

unsigned long long Edge::peer() const noexcept {
	return meta.id;
}

void Edge::setup() {

}

void Edge::clear() noexcept {
	pair(false);
}

} /* namespace wanhive */
