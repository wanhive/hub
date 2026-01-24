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
#include "../base/unix/Time.h"
#include <sys/timerfd.h>

namespace wanhive {

Alarm::Alarm(const Period &period, bool blocking) :
		period { period } {
	create(blocking);
}

Alarm::~Alarm() {

}

void Alarm::start() {
	update(period);
}

void Alarm::stop() noexcept {
	try {
		update( { 0, 0 });
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

ssize_t Alarm::read(unsigned long long &count) {
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
		throw Exception(EX_STATE);
	}
}

void Alarm::reset(const Period &period) {
	update(period);
	//Update the settings only if the system call succeeded
	this->period = period;
}

const Period& Alarm::getPeriod() const noexcept {
	return period;
}

void Alarm::create(bool blocking) {
	auto fd = timerfd_create(CLOCK_MONOTONIC, blocking ? 0 : TFD_NONBLOCK);
	if (fd != -1) {
		Descriptor::set(fd);
	} else {
		throw SystemException();
	}
}

void Alarm::update(const Period &period) {
	struct itimerspec time;
	time.it_value = Time::convert(period.once);
	time.it_interval = Time::convert(period.interval);
	if (timerfd_settime(Descriptor::get(), 0, &time, nullptr)) {
		throw SystemException();
	}
}

void Alarm::settings(Period &period) {
	struct itimerspec time;
	if (timerfd_gettime(Descriptor::get(), &time) == 0) {
		period.once = Time::milliseconds(time.it_value);
		period.interval = Time::milliseconds(time.it_interval);
	} else {
		throw SystemException();
	}
}

} /* namespace wanhive */
