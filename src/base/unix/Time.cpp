/*
 * Time.cpp
 *
 * Time keeper
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Time.h"
#include "SystemException.h"

namespace wanhive {

Time::Time() noexcept :
		ts { } {

}

Time::Time(const timespec &ts) noexcept :
		ts { ts } {

}

Time::Time(clockid_t id) {
	now(id);
}

Time::~Time() {

}

const timespec& Time::now(clockid_t id) {
	if (now(id, ts)) {
		return ts;
	} else {
		throw SystemException();
	}
}

size_t Time::convert(char *s, size_t size, const char *format, bool utc) const {
	tm ct;
	convert(ct, utc);
	format = format ? format : "%Y-%m-%d %H:%M:%S %Z";
	return ::strftime(s, size, format, &ct);
}

void Time::convert(tm &calendar, bool utc) const {
	auto ctp =
			utc ? ::gmtime_r(&ts.tv_sec, &calendar) : ::localtime_r(&ts.tv_sec,
							&calendar);
	if (!ctp) {
		throw SystemException();
	} else {
		return;
	}
}

const timespec& Time::get() const noexcept {
	return ts;
}

void Time::set(const timespec &ts) noexcept {
	this->ts = ts;
}

bool Time::now(clockid_t id, timespec &ts) noexcept {
	if (::clock_gettime(id, &ts) == 0) {
		return true;
	} else {
		return false;
	}
}

bool Time::now(clockid_t id, double &seconds) noexcept {
	timespec ts;
	if (now(id, ts)) {
		seconds = Time::seconds(ts);
		return true;
	} else {
		return false;
	}
}

bool Time::now(clockid_t id, long long &milliseconds) noexcept {
	timespec ts;
	if (now(id, ts)) {
		milliseconds = Time::milliseconds(ts);
		return true;
	} else {
		return false;
	}
}

void Time::future(timespec &ts, unsigned int offset) noexcept {
	ts.tv_sec += offset / 1000;
	ts.tv_nsec += (offset % 1000) * 1000000L;
	//Adjust for overflow
	ts.tv_sec += ts.tv_nsec / 1000000000L;
	ts.tv_nsec %= 1000000000L;
}

timespec Time::convert(unsigned int milliseconds,
		unsigned int nanoseconds) noexcept {
	struct timespec ts;
	//These many seconds
	ts.tv_sec = milliseconds / 1000;
	//These many nanoseconds
	auto nsec = ((milliseconds % 1000) * 1000000LL) + nanoseconds;
	//Adjust for nanoseconds overflow
	ts.tv_sec += (nsec / 1000000000L);
	ts.tv_nsec = (nsec % 1000000000L);
	return ts;
}

double Time::seconds(const timespec &ts) noexcept {
	return (ts.tv_sec + (ts.tv_nsec / 1000000000.0));
}

long long Time::milliseconds(const timespec &ts) noexcept {
	return ((ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL));
}

} /* namespace wanhive */
