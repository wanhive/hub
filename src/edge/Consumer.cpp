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
		Monitor { uid, path } {
	clear();
}

Consumer::~Consumer() {

}

void Consumer::expel(Watcher *w) noexcept {
	Agent::expel(w);

	if (!Agent::connected()) {
		ctx.subscribed = false;
	}
}

void Consumer::configure(void *arg) {
	try {
		Monitor::configure(arg);
		WH_LOG_DEBUG("Setting things up...");
		if (arg) {
			ctx.topic = *static_cast<unsigned int*>(arg);
		}
		ctx.topic = ctx.topic > Topic::MAX_ID ? 0 : ctx.topic;
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
	Monitor::cleanup();
}

bool Consumer::subscribe() noexcept {
	if (ctx.subscribed) {
		return true;
	} else {
		return Monitor::subscribe(ctx.topic);
	}
}

bool Consumer::unsubscribe() noexcept {
	if (!ctx.subscribed) {
		return true;
	} else {
		return Monitor::unsubscribe(ctx.topic);
	}
}

bool Consumer::subscribe(const Message *message) noexcept {
	if (ctx.subscribed) {
		return true;
	} else {
		unsigned int topic { Topic::MAX_ID + 1 };
		ctx.subscribed = Monitor::subscribe(message, topic)
				&& (ctx.topic == topic);
		return ctx.subscribed;
	}
}

bool Consumer::unsubscribe(const Message *message) noexcept {
	if (!ctx.subscribed) {
		return true;
	} else {
		unsigned int topic { Topic::MAX_ID + 1 };
		ctx.subscribed = !(Monitor::unsubscribe(message, topic)
				&& (ctx.topic == topic));
		return !ctx.subscribed;
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
	ctx = { 0, false };
}

} /* namespace wanhive */
