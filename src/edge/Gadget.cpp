/*
 * Gadget.cpp
 *
 * Gadget Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Gadget.h"
#include "../base/common/Logger.h"
#include "../util/commands.h"

namespace wanhive {

Gadget::Gadget(unsigned long long uid, const char *path) noexcept :
		Edge { uid, path } {
	clear();
}

Gadget::~Gadget() {

}

void Gadget::configure(void *arg) {
	try {
		Edge::configure(arg);
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

void Gadget::cleanup() noexcept {
	clear();
	Edge::cleanup();
}

void Gadget::maintain() noexcept {
	if (online() && !Agent::connected()) {
		Edge::end();
		Agent::maintain();
	}
}

void Gadget::route(Message *message) noexcept {
	if (online() && !Agent::connected()) {
		Agent::route(message);
		return;
	}

	//Prevents replay (UID is the sink)
	message->setDestination(getUid());
	service(message);
}

bool Gadget::online() const noexcept {
	return ctx.online;
}

bool Gadget::multicast() const noexcept {
	return ctx.multicast;
}

void Gadget::interval(unsigned int value) noexcept {
	ctx.interval = value;
}

unsigned int Gadget::interval() const noexcept {
	return ctx.interval;
}

void Gadget::prepare(MessageHeader &header, unsigned int session) const noexcept {
	if (multicast()) {
		header.setAddress(0, 0);
		header.setControl(Message::HLEN, 0, session);
		header.setContext(2, 0, WH_AQLF_REQUEST);
	} else {
		header.setAddress(0, peer());
		header.setControl(Message::HLEN, 0, session);
		header.setContext(0, 0, WH_AQLF_REQUEST);
	}
}

bool Gadget::service(Message *message) noexcept {
	return true;
}

void Gadget::setup() {
	ctx.multicast = ctx.multicast && ctx.online;
	Edge::pair(ctx.online && !ctx.multicast);

	Period p;
	Hub::period(p);
	if (ctx.online && p.once && p.interval) {
		ctx.interval = p.interval;
	} else {
		ctx.interval = 0;
	}
}

void Gadget::clear() noexcept {
	ctx = { false, false, 0 };
}

} /* namespace wanhive */
