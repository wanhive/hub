/*
 * Event.cpp
 *
 * Events counter
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Event.h"
#include "Hub.h"
#include "../base/unix/SystemException.h"
#include <sys/eventfd.h>

namespace wanhive {

Event::Event(bool semaphore, bool blocking) {
	auto flags = (semaphore ? EFD_SEMAPHORE : 0)
			| (blocking ? 0 : EFD_NONBLOCK);
	auto fd = eventfd(0, flags);
	if (fd != -1) {
		Descriptor::setHandle(fd);
	} else {
		throw SystemException();
	}
}

Event::~Event() {

}

void Event::start() {

}

void Event::stop() noexcept {

}

bool Event::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Event> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Event::publish(void *arg) noexcept {
	return false;
}

ssize_t Event::read(unsigned long long &count) {
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

ssize_t Event::write(unsigned long long count) {
	uint64_t eventCount = count;
	return Descriptor::write(&eventCount, sizeof(eventCount));
}

} /* namespace wanhive */
