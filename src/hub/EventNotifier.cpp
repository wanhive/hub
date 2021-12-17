/*
 * EventNotifier.cpp
 *
 * Event notification mechanism for Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "EventNotifier.h"
#include "Hub.h"
#include "../base/unix/SystemException.h"
#include <sys/eventfd.h>

namespace wanhive {

EventNotifier::EventNotifier(bool semaphore, bool blocking) :
		count(0) {
	auto flags = (semaphore ? EFD_SEMAPHORE : 0)
			| (blocking ? 0 : EFD_NONBLOCK);
	auto fd = eventfd(0, flags);
	if (fd != -1) {
		setHandle(fd);
	} else {
		throw SystemException();
	}
}

EventNotifier::~EventNotifier() {

}

void EventNotifier::start() {

}

void EventNotifier::stop() noexcept {

}

bool EventNotifier::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<EventNotifier> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool EventNotifier::publish(void *arg) noexcept {
	return false;
}

ssize_t EventNotifier::read() {
	count = 0; //Reset the count
	uint64_t eventCount;
	auto nRead = Descriptor::read(&eventCount, sizeof(eventCount));
	if (nRead == sizeof(eventCount)) {
		count = eventCount;
		return nRead;
	} else if (nRead == 0 || nRead == -1) {
		return nRead;
	} else {
		//Something broke
		throw Exception(EX_INVALIDSTATE);
	}
}

ssize_t EventNotifier::write(unsigned long long events) {
	uint64_t eventCount = events;
	return Descriptor::write(&eventCount, sizeof(eventCount));
}

unsigned long long EventNotifier::getCount() const noexcept {
	return count;
}

} /* namespace wanhive */
