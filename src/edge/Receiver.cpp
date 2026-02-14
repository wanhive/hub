/*
 * Receiver.cpp
 *
 * Receiver Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Receiver.h"
#include "../base/common/Logger.h"

namespace wanhive {

Receiver::Receiver(unsigned long long uid, const char *path) noexcept :
		Monitor { uid, path } {
	clear();
}

Receiver::~Receiver() {

}

void Receiver::expel(Watcher *w) noexcept {
	Agent::expel(w);

	if (!Agent::connected()) {
		ctx.subscribed = false;
	}
}

void Receiver::configure(void *arg) {
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

void Receiver::cleanup() noexcept {
	clear();
	Monitor::cleanup();
}

bool Receiver::subscribe() noexcept {
	if (ctx.subscribed) {
		return true;
	} else {
		return Monitor::subscribe(ctx.topic);
	}
}

bool Receiver::unsubscribe() noexcept {
	if (!ctx.subscribed) {
		return true;
	} else {
		return Monitor::unsubscribe(ctx.topic);
	}
}

bool Receiver::subscribe(const Message *message) noexcept {
	if (ctx.subscribed) {
		return true;
	} else {
		unsigned int topic { Topic::MAX_ID + 1 };
		ctx.subscribed = Monitor::subscribe(message, topic)
				&& (ctx.topic == topic);
		return ctx.subscribed;
	}
}

bool Receiver::unsubscribe(const Message *message) noexcept {
	if (!ctx.subscribed) {
		return true;
	} else {
		unsigned int topic { Topic::MAX_ID + 1 };
		ctx.subscribed = !(Monitor::unsubscribe(message, topic)
				&& (ctx.topic == topic));
		return !ctx.subscribed;
	}
}

unsigned int Receiver::topic() const noexcept {
	return ctx.topic;
}

bool Receiver::subscribed() const noexcept {
	return ctx.subscribed;
}

void Receiver::setup() {

}

void Receiver::clear() noexcept {
	ctx = { 0, false };
}

} /* namespace wanhive */
