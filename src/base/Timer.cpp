/*
 * Timer.cpp
 *
 * Milliseconds precision timer
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Timer.h"
#include "ds/Twiddler.h"
#include "unix/Time.h"
#include <cerrno>
#include <cmath>
#include <cstdio>

namespace wanhive {

Timer::Timer() noexcept {
	now();
}

Timer::~Timer() {

}

double Timer::elapsed() const noexcept {
	return difference(t, currentTime());
}

void Timer::now() noexcept {
	t = currentTime();
}

bool Timer::hasTimedOut(unsigned int milliseconds) const noexcept {
	if (milliseconds) {
		auto diff = ((unsigned long long) milliseconds) * MS_IN_MILS;
		auto mark = currentTime();
		return (mark < t) || ((mark - t) > diff);
	} else {
		return true;
	}
}

void Timer::sleep(unsigned int milliseconds, unsigned int nanoseconds) noexcept {
	auto tv = Time::convert(milliseconds, nanoseconds);
	//Sleep might get interrupted due to signal delivery
	while (nanosleep(&tv, &tv) == -1 && errno == EINTR) {
		//Loop if interrupted by a signal
	}
}

size_t Timer::print(char *buffer, size_t size, const char *format) noexcept {
	try {
		Time t(CLOCK_REALTIME); //Cannot fail
		format = (format == nullptr) ? "%Y_%m_%d-%H_%M_%S-%Z" : format;
		return t.convert(buffer, size, format); //Can fail
	} catch (...) {
		return 0;
	}
}

size_t Timer::print(double timestamp, char *buffer, size_t size) noexcept {
	if (!buffer || !size || !std::isfinite(timestamp)) {
		return 0;
	}

	auto seconds = static_cast<time_t>(timestamp);
	auto milliseconds = static_cast<long>((timestamp - seconds) * 1000);

	tm utc;
	if (gmtime_r(&seconds, &utc) == nullptr) {
		return 0;
	}

	auto offset = strftime(buffer, size, "%Y-%m-%dT%H:%M:%S", &utc);
	if (offset == 0) {
		return 0;
	}

	size = (size - offset);
	auto n = snprintf((buffer + offset), size, ".%03ldZ", milliseconds);
	if (n < 0 || ((size_t) n) >= size) {
		return 0;
	} else {
		return (offset + n);
	}
}

unsigned long long Timer::timeSeed() noexcept {
	Time t(CLOCK_MONOTONIC); //Cannot fail
	decltype(auto) ts = t.get();
	return Twiddler::FVN1aHash(&ts, sizeof(ts));
}

unsigned long long Timer::currentTime() noexcept {
	Time t(CLOCK_MONOTONIC); //Cannot fail
	decltype(auto) ts = t.get();
	return (((unsigned long long) ts.tv_sec * MS_IN_SEC)
			+ (ts.tv_nsec / NS_IN_MS));
}

double Timer::difference(unsigned long long start,
		unsigned long long end) noexcept {
	return ((double) (end - start) / (double) MS_IN_SEC);
}

} /* namespace wanhive */
