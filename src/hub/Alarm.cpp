/*
 * Alarm.cpp
 *
 * Periodic timer
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Alarm.h"
#include "Hub.h"
#include "../base/unix/SystemException.h"
#include <sys/timerfd.h>

namespace {

void milsToSpec(unsigned int milliseconds, timespec &ts) {
	ts.tv_sec = milliseconds / wanhive::Timer::MILS_IN_SEC;
	ts.tv_nsec = (milliseconds - (ts.tv_sec * wanhive::Timer::MILS_IN_SEC))
			* wanhive::Timer::NS_IN_MILS;
}

long long specToMils(timespec &ts) {
	return (((long long) ts.tv_sec * wanhive::Timer::MILS_IN_SEC)
			+ (ts.tv_nsec / wanhive::Timer::NS_IN_MILS));
}

}  // namespace

namespace wanhive {

Alarm::Alarm(unsigned int expiration, unsigned int interval, bool blocking) :
		expiration(expiration), interval(interval), count(0) {
	create(blocking);
}

Alarm::~Alarm() {

}

void Alarm::start() {
	update(expiration, interval);
}

void Alarm::stop() noexcept {
	try {
		update(0, 0);
	} catch (const BaseException &e) {
	}
}

bool Alarm::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Alarm> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Alarm::publish(void *arg) noexcept {
	return false;
}

ssize_t Alarm::read() {
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

void Alarm::reset(unsigned int expiration, unsigned int interval) {
	update(expiration, interval);
	//Update the settings only if the system call succeeded
	this->expiration = expiration;
	this->interval = interval;
}

unsigned long long Alarm::getCount() const noexcept {
	return count;
}

unsigned int Alarm::getExpiration() const noexcept {
	return expiration;
}

unsigned int Alarm::getInterval() const noexcept {
	return interval;
}

void Alarm::create(bool blocking) {
	auto fd = timerfd_create(CLOCK_MONOTONIC, blocking ? 0 : TFD_NONBLOCK);
	if (fd != -1) {
		setHandle(fd);
	} else {
		throw SystemException();
	}
}

void Alarm::update(unsigned int expiration, unsigned int interval) {
	struct itimerspec time;
	milsToSpec(expiration, time.it_value);
	milsToSpec(interval, time.it_interval);
	if (timerfd_settime(getHandle(), 0, &time, nullptr)) {
		throw SystemException();
	}
}

void Alarm::settings(unsigned int &expiration, unsigned int &interval) {
	struct itimerspec time;
	if (timerfd_gettime(getHandle(), &time) == 0) {
		expiration = specToMils(time.it_value);
		interval = specToMils(time.it_interval);
	} else {
		throw SystemException();
	}
}

} /* namespace wanhive */
