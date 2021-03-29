/*
 * Clock.cpp
 *
 * Millisecond resolution time-keeper for Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Clock.h"
#include "Hub.h"
#include "../base/Timer.h"

namespace wanhive {

Clock::Clock(unsigned int expiration, unsigned int interval, bool blocking) :
		expiration(expiration), interval(interval), count(0) {
	setHandle(Timer::openTimerfd(blocking));
}

Clock::~Clock() {

}

void Clock::start() {
	Timer::setTimerfd(getHandle(), expiration, interval);
}

void Clock::stop() noexcept {
	try {
		Timer::setTimerfd(getHandle(), 0, 0);
	} catch (const BaseException &e) {
	}
}

bool Clock::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Clock> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Clock::publish(void *arg) noexcept {
	return false;
}

ssize_t Clock::read() {
	count = 0; //Reset the count
	uint64_t expiryCount;
	auto nRead = Descriptor::read(&expiryCount, sizeof(expiryCount));
	if (nRead == sizeof(expiryCount)) {
		count = expiryCount;
		return nRead;
	} else if (nRead == 0 || nRead == -1) {
		return nRead;
	} else {
		//Something broke
		throw Exception(EX_INVALIDSTATE);
	}
}

void Clock::reset(unsigned int expiration, unsigned int interval) {
	Timer::setTimerfd(getHandle(), expiration, interval);
	//Update the settings only if the system call succeeded
	this->expiration = expiration;
	this->interval = interval;
}

unsigned int Clock::getExpiration() const noexcept {
	return expiration;
}

unsigned int Clock::getInterval() const noexcept {
	return interval;
}

unsigned long long Clock::getCount() const noexcept {
	return count;
}

} /* namespace wanhive */
