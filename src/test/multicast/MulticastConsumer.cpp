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
#include "../../base/ds/Twiddler.h"
#include "../../util/commands.h"

namespace wanhive {

MulticastConsumer::MulticastConsumer(unsigned long long uid, unsigned int topic,
		const char *path) noexcept :
		ClientHub(uid, path), topic(topic), subscribed(false) {
	Reactor::setTimeout(2000);
}

MulticastConsumer::~MulticastConsumer() {

}

void MulticastConsumer::cleanup() noexcept {
	subscribed = false;
	ClientHub::cleanup();
}

void MulticastConsumer::stop(Watcher *w) noexcept {
	if (w->getUid() == 0) {
		//Subscriptions are lost on disconnection
		subscribed = false;
	}

	ClientHub::stop(w);
}

void MulticastConsumer::route(Message *message) noexcept {
	if (!isConnected()) {
		ClientHub::route(message);
	} else {
		process(message);
	}
}

void MulticastConsumer::maintain() noexcept {
	if (!isConnected()) {
		ClientHub::maintain();
	} else if (!subscribed && (topic <= Topic::MAX_ID)
			&& timer.hasTimedOut(2000)) {
		timer.now();
		subscribe(topic);
	} else {
		//Nothing
	}
}

void MulticastConsumer::processMulticastMessage(const Message *msg) noexcept {
	msg->printHeader();
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
			handleInvalidMessage(message);
			return;
		} else if (qlf == WH_QLF_PUBLISH) {
			processMulticastMessage(message);
			return;
		} else if (source == 0 && qlf == WH_QLF_SUBSCRIBE) {
			processSubscribeResponse(message);
			return;
		} else {
			handleInvalidMessage(message);
			return;
		}
	default:
		handleInvalidMessage(message);
		break;
	}
}

void MulticastConsumer::processSubscribeResponse(const Message *msg) noexcept {
	if (msg->getStatus() == WH_AQLF_ACCEPTED) {
		subscribed = true;
		WH_LOG_INFO("Subscribed to %u", msg->getSession());
	} else if (msg->getStatus() == WH_AQLF_REJECTED) {
		WH_LOG_INFO("Subscription to %u denied", msg->getSession());
	} else {
		handleInvalidMessage(msg);
	}
}

void MulticastConsumer::handleInvalidMessage(const Message *msg) noexcept {
	WH_LOG_DEBUG("Invalid message");
}

void MulticastConsumer::subscribe(unsigned int topic) noexcept {
	auto message = Message::create();
	if (message) {
		message->putHeader(0, 0, Message::HEADER_SIZE, 0, topic,
				WH_CMD_MULTICAST, WH_QLF_SUBSCRIBE, WH_AQLF_REQUEST);
		sendMessage(message);
	}
}

} /* namespace wanhive */
