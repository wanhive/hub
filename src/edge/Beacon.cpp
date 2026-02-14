/*
 * Beacon.cpp
 *
 * Beacon Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Beacon.h"
#include "../base/common/Logger.h"
#include "../base/ds/Twiddler.h"

namespace wanhive {

Beacon::Beacon(unsigned long long uid, const char *path) noexcept :
		Gadget { uid, path } {
	clear();
}

Beacon::~Beacon() {

}

void Beacon::configure(void *arg) {
	try {
		Gadget::configure(arg);
		ctx.channel = getOptions().getNumber("EDGE", "beacon", CHANNEL);
		ctx.channel = Twiddler::min(Topic::MAX_ID, ctx.channel);
		WH_LOG_DEBUG("\nCHANNEL=%u\n", ctx.channel);
		setup();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		throw Exception(EX_OPERATION);
	}
}

void Beacon::cleanup() noexcept {
	clear();
	Gadget::cleanup();
}

void Beacon::onAlarm(unsigned long long uid, unsigned long long ticks) noexcept {
	transmit();
}

bool Beacon::service(Message *message) noexcept {
	if (!(message->getSession() == 0
			&& Edge::accept(message, Gadget::interval()))) {
		WH_LOG_DEBUG("Invalid request");
	}
	return true;
}

bool Beacon::transmit() noexcept {
	if (!Gadget::share()) {
		return false;
	}

	auto message = Message::create();
	if (!message) {
		return false;
	}

	MessageHeader header;
	prepare(header, ctx.channel);
	message->putHeader(header);
	message->appendDouble(Edge::timestamp());
	message->setDestination(0);
	return forward(message);
}

void Beacon::setup() {

}

void Beacon::clear() noexcept {
	ctx.channel = CHANNEL;
}

} /* namespace wanhive */
