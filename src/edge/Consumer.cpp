/*
 * Consumer.cpp
 *
 * Consumer Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Consumer.h"
#include "../base/common/Logger.h"
#include "../util/commands.h"

namespace wanhive {

Consumer::Consumer(unsigned long long uid, const char *path) noexcept :
		Agent { uid, path } {
	clear();
}

Consumer::~Consumer() {

}

void Consumer::expel(Watcher *w) noexcept {
	if (w->getUid() == 0) {
		ctx.subscribed = false;
	}

	Agent::expel(w);
}

void Consumer::configure(void *arg) {
	try {
		Agent::configure(arg);
		if (arg) {
			ctx.topic = *static_cast<unsigned int*>(arg);
		} else {
			ctx.topic = getOptions().getNumber("CONSUMER", "topic");
		}
		ctx.topic = ctx.topic > Topic::MAX_ID ? 0 : ctx.topic;
		WH_LOG_DEBUG("\nTOPIC=%u\n", ctx.topic);
		setup();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		throw Exception(EX_OPERATION);
	}
}

void Consumer::cleanup() noexcept {
	clear();
	Agent::cleanup();
}

bool Consumer::subscribe() noexcept {
	if (ctx.subscribed) {
		return true;
	}

	auto message = Message::create();
	if (message) {
		MessageHeader header;
		header.setAddress(0, 0);
		header.setControl(Message::HLEN, 0, ctx.topic);
		header.setContext(WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE, WH_AQLF_REQUEST);
		message->putHeader(header);
		return forward(message);
	} else {
		return false;
	}
}

bool Consumer::subscribe(const Message *message) noexcept {
	if (ctx.subscribed) {
		return true;
	} else {
		ctx.subscribed = (message)
				&& (message->checkContext(WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE,
						WH_AQLF_ACCEPTED))
				&& (message->getSession() == ctx.topic);
		return ctx.subscribed;
	}
}

unsigned int Consumer::topic() const noexcept {
	return ctx.topic;
}

bool Consumer::subscribed() const noexcept {
	return ctx.subscribed;
}

void Consumer::setup() {

}

void Consumer::clear() noexcept {
	ctx = { 0, 0 };
}

} /* namespace wanhive */
