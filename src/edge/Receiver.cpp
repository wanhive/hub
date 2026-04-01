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

constexpr int TIMEOUT = 3500;

}  // namespace

namespace wanhive {

Receiver::Receiver(unsigned long long uid, const char *path) noexcept :
		Monitor { uid, path } {
	clear();
}

Receiver::~Receiver() {

}

unsigned int Receiver::channel() const noexcept {
	return ctx.channel;
}

bool Receiver::subscribed() const noexcept {
	return ctx.subscribed;
}

bool Receiver::multicast() const noexcept {
	return ctx.multicast;
}

bool Receiver::subscribe() noexcept {
	if (multicast() && !subscribed()) {
		return Monitor::subscribe(channel());
	} else {
		return true;
	}
}

bool Receiver::unsubscribe() noexcept {
	if (subscribed()) {
		return Monitor::unsubscribe(channel());
	} else {
		return true;
	}
}

bool Receiver::subscribe(const Message *message) noexcept {
	if (subscribed()) {
		return true;
	} else {
		auto topic { Topic::MAX_ID + 1 };
		auto status = Monitor::subscribe(message, topic)
				&& (channel() == topic);
		subscribed(status);
		return subscribed();
	}
}

bool Receiver::unsubscribe(const Message *message) noexcept {
	if (!subscribed()) {
		return true;
	} else {
		auto topic { Topic::MAX_ID + 1 };
		auto status = Monitor::unsubscribe(message, topic)
				&& (channel() == topic);
		subscribed(!status);
		return subscribed();
	}
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
		ctx.multicast = getOptions().getBoolean("EDGE", "multicast");
		ctx.channel = getOptions().getNumber("EDGE", "channel");
		ctx.channel = (ctx.channel > Topic::MAX_ID) ? 0 : ctx.channel;
		Monitor::target(getOptions().getNumber("EDGE", "target", getUid()));
		WH_LOG_DEBUG("\nMULTICAST=%s, CHANNEL=%u, TARGET=%llu\n",
				WH_BOOLF(ctx.multicast), ctx.channel, Monitor::target());
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
	} else if (multicast() && !subscribed() && timer.expired(TIMEOUT)) {
		subscribe();
	} else {
		return;
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
		Monitor::ping(Agent::cycle(), 0);
	}
}

bool Receiver::answer(Message *message) noexcept {
	message->header().print();
	return true;
}

bool Receiver::onboard(Message *message) noexcept {
	return Monitor::join(message);
}

bool Receiver::receive(Message *message) noexcept {
	auto session = message->getSession();
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();
	auto status = message->getStatus();
	switch (cmd) {
	case WH_CMD_NULL:
		if ((session == 0) && (qlf == 0) && (status != WH_AQLF_REQUEST)) {
			return onboard(message);
		} else {
			return answer(message);
		}
	case WH_CMD_MULTICAST:
		switch (qlf) {
		case WH_QLF_PUBLISH:
			return answer(message);
		case WH_QLF_SUBSCRIBE:
			return subscribe(message);
		case WH_QLF_UNSUBSCRIBE:
			return unsubscribe(message);
		default:
			return false;
		}
	default:
		return false;
	}
}

void Receiver::subscribed(bool status) noexcept {
	ctx.subscribed = status;
	if (!status && multicast()) {
		Reactor::setTimeout(Agent::cycle() ? -1 : TIMEOUT);
	} else {
		Reactor::setTimeout(-1);
	}
}

void Receiver::setup() {
	if (!Agent::cycle()) {
		Monitor::end();
	}

	subscribed(false);
}

void Receiver::clear() noexcept {
	ctx = { 0, false, false };
}

} /* namespace wanhive */
