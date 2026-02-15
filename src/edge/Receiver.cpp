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
#include "../util/commands.h"

namespace {

constexpr int TIMEOUT = 2000;

}  // namespace

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
		Monitor::target(Monitor::target());
		subscribed(false);
	}
}

void Receiver::configure(void *arg) {
	try {
		Monitor::configure(arg);
		ctx.channel = getOptions().getNumber("EDGE", "channel");
		ctx.channel = (ctx.channel > Topic::MAX_ID) ? 0 : ctx.channel;
		Monitor::target(getOptions().getNumber("EDGE", "target", getUid()));
		WH_LOG_DEBUG("\nCHANNEL=%u, TARGET=%llu\n", ctx.channel,
				Monitor::target());
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

void Receiver::maintain() noexcept {
	if (!Agent::connected()) {
		Agent::maintain();
	} else {
		subscribe(TIMEOUT);
	}
}

void Receiver::route(Message *message) noexcept {
	if (!Agent::connected()) {
		Agent::route(message);
		return;
	}

	//Prevents replay (UID is the sink)
	message->setDestination(getUid());
	receive(message);
}

void Receiver::onAlarm(unsigned long long uid,
		unsigned long long ticks) noexcept {
	if (Agent::connected()) {
		Monitor::ping(ctx.interval, 0);
	}
}

unsigned int Receiver::channel() const noexcept {
	return ctx.channel;
}

bool Receiver::subscribed() const noexcept {
	return ctx.subscribed;
}

bool Receiver::subscribe(unsigned int delay) noexcept {
	if (subscribed()) {
		return true;
	} else if (channel() && timer.expired(delay)) {
		return Monitor::subscribe(channel());
	} else {
		return false;
	}
}

bool Receiver::unsubscribe(unsigned int delay) noexcept {
	if (!subscribed()) {
		return true;
	} else if (timer.expired(delay)) {
		return Monitor::unsubscribe(channel());
	} else {
		return false;
	}
}

bool Receiver::subscribe(const Message *message) noexcept {
	if (subscribed()) {
		return true;
	} else {
		unsigned int topic { Topic::MAX_ID + 1 };
		auto status = Monitor::subscribe(message, topic)
				&& (ctx.channel == topic);
		subscribed(status);
		return subscribed();
	}
}

bool Receiver::unsubscribe(const Message *message) noexcept {
	if (!subscribed()) {
		return true;
	} else {
		unsigned int topic { Topic::MAX_ID + 1 };
		auto status = Monitor::unsubscribe(message, topic)
				&& (ctx.channel == topic);
		subscribed(!status);
		return subscribed();
	}
}

bool Receiver::service(Message *message) noexcept {
	message->header().print();
	return true;
}

bool Receiver::receive(Message *message) noexcept {
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();
	auto session = message->getSession();

	switch (cmd) {
	case WH_CMD_NULL:
		if (session == 0) {
			return Monitor::connect(message);
		} else {
			return service(message);
		}
	case WH_CMD_MULTICAST:
		switch (qlf) {
		case WH_QLF_PUBLISH:
			return service(message);
		case WH_QLF_SUBSCRIBE:
			return subscribe(message);
		case WH_QLF_UNSUBSCRIBE:
			return unsubscribe(message);
		default:
			WH_LOG_INFO("Invalid message");
			return false;
		}
	default:
		WH_LOG_INFO("Unsupported message");
		return false;
	}
}

void Receiver::subscribed(bool status) noexcept {
	ctx.subscribed = status;
	if (!status && channel()) {
		Reactor::setTimeout(ctx.interval ? -1 : TIMEOUT);
	} else {
		Reactor::setTimeout(-1);
	}
}

void Receiver::setup() {
	Period p;
	Hub::period(p);
	if (p.once && p.interval) {
		ctx.interval = p.interval;
	} else {
		ctx.interval = 0;
		Monitor::end();
	}

	subscribed(false);
}

void Receiver::clear() noexcept {
	ctx = { 0, 0, false };
}

} /* namespace wanhive */
