/*
 * MulticastConsumer.cpp
 *
 * Multicast client example
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MulticastConsumer.h"
#include "../../base/common/Logger.h"
#include "../../util/commands.h"

namespace wanhive {

MulticastConsumer::MulticastConsumer(unsigned long long uid, unsigned int topic,
		const char *path) noexcept :
		Receiver { uid, path }, topic { topic } {

}

MulticastConsumer::~MulticastConsumer() {

}

void MulticastConsumer::configure(void *arg) {
	try {
		Receiver::configure(&topic);
		Reactor::setTimeout(2000);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void MulticastConsumer::cleanup() noexcept {
	Receiver::cleanup();
}

void MulticastConsumer::route(Message *message) noexcept {
	if (!connected()) {
		Agent::route(message);
	} else {
		process(message);
	}
}

void MulticastConsumer::maintain() noexcept {
	if (!connected()) {
		Reactor::setTimeout(2000);
		Agent::maintain();
	} else if (!subscribed()) {
		subscribe(2000);
	} else {
		Reactor::setTimeout(-1);
	}
}

void MulticastConsumer::process(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto source = message->getSource();
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();

	//Maintains sanity (prevents replay), do not remove
	message->setDestination(getUid());

	switch (cmd) {
	case WH_CMD_MULTICAST:
		if (origin != 0) {
			discard(message);
			return;
		} else if (qlf == WH_QLF_PUBLISH) {
			print(message);
			return;
		} else if (source == 0 && qlf == WH_QLF_SUBSCRIBE) {
			subscribe(message);
			return;
		} else {
			discard(message);
			return;
		}
	default:
		discard(message);
		break;
	}
}

void MulticastConsumer::print(const Message *msg) noexcept {
	msg->printHeader();
}

void MulticastConsumer::subscribe(unsigned int delay) noexcept {
	if (!timer.expired(delay)) {
		return;
	}

	Receiver::subscribe();
}

void MulticastConsumer::subscribe(const Message *msg) noexcept {
	if (Receiver::subscribe(msg)) {
		WH_LOG_INFO("Subscribed to topic %u", topic);
	} else {
		WH_LOG_INFO("Subscription to topic %u denied", topic);
	}
}

void MulticastConsumer::discard(const Message *msg) noexcept {
	WH_LOG_DEBUG("Invalid message");
}

} /* namespace wanhive */
