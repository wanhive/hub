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
		ctx.online = getOptions().getBoolean("EDGE", "online");
		ctx.multicast = getOptions().getBoolean("EDGE", "multicast");
		WH_LOG_DEBUG("\nONLINE=%s, MULTICAST=%s\n", WH_BOOLF(ctx.online),
				WH_BOOLF(ctx.multicast));
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

bool Edge::online() const noexcept {
	return ctx.online;
}

bool Edge::multicast() const noexcept {
	return (ctx.online && ctx.multicast);
}

bool Edge::accept(Message *message, unsigned int interval) noexcept {
	if (!message || message->getPayloadLength() < sizeof(uint32_t)) {
		return false;
	} else if (accept(message->getSource(), message->getData32(0))) {
		WH_LOG_DEBUG("Node %llu requested %u tokens", slot.id, slot.tokens);
		message->setData32(0, interval);
		message->putLength(Message::HLEN + sizeof(uint32_t));
		message->writeDestination(slot.id);
		message->setDestination(0);
		message->putStatus(WH_AQLF_ACCEPTED);
		return true;
	} else {
		return false;
	}
}

bool Edge::accept(unsigned long long id, unsigned int tokens) noexcept {
	if (ctx.online && !ctx.multicast) {
		slot = { id, tokens };
		return live();
	} else {
		return false;
	}
}

void Edge::revoke() noexcept {
	slot = { getUid(), 0 };
}

bool Edge::live() const noexcept {
	return ((slot.id != getUid()) && (slot.tokens != 0));
}

bool Edge::report() noexcept {
	if (live()) {
		slot.tokens -= 1;
		return true;
	} else {
		return false;
	}
}
unsigned long long Edge::peer() const noexcept {
	if (live()) {
		return slot.id;
	} else {
		return getUid();
	}
}

double Edge::timestamp() noexcept {
	double seconds { };
	Time::now(CLOCK_REALTIME, seconds);
	return seconds;
}

void Edge::setup() {

}

void Edge::clear() noexcept {
	revoke();
	ctx = { false, false };
}

} /* namespace wanhive */
