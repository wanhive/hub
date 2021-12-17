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
#include <cstring>

namespace wanhive {

Time::Time() noexcept {
	::memset(&ts, 0, sizeof(ts));
}

Time::Time(const timespec &ts) noexcept {
	set(ts);
}

Time::Time(clockid_t id) {
	now(id);
}

Time::~Time() {

}

const timespec& Time::now(clockid_t id) {
	if (::clock_gettime(id, &ts) == 0) {
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

} /* namespace wanhive */
