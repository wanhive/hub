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
#include "../base/SystemException.h"
#include <sys/eventfd.h>

namespace wanhive {

EventNotifier::EventNotifier(bool semaphore, bool blocking) :
		count(0) {
	int flags = (semaphore ? EFD_SEMAPHORE : 0) | (blocking ? 0 : EFD_NONBLOCK);
	int fd = eventfd(0, flags);
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
	uint64_t eventCount;
	ssize_t nRead = Watcher::read(&eventCount, sizeof(uint64_t));
	count = eventCount;
	if (nRead == sizeof(uint64_t)) {
		count = eventCount;
		return nRead;
	} else if (nRead == 0 || nRead == -1) {
		count = 0;
		return nRead;
	} else {
		//Something broke
		count = 0;
		throw Exception(EX_INVALIDSTATE);
	}
}

ssize_t EventNotifier::write(unsigned long long events) {
	uint64_t eventCount = events;
	return Watcher::write(&eventCount, sizeof(uint64_t));
}

unsigned long long EventNotifier::getCount() const noexcept {
	return count;
}

} /* namespace wanhive */
