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
	struct timespec tv;
	//These many seconds
	tv.tv_sec = milliseconds / MILS_IN_SEC;
	//These many nanoseconds
	auto nsec = (unsigned long long) ((milliseconds % MILS_IN_SEC) * NS_IN_MILS)
			+ nanoseconds;
	//Adjust for nanoseconds overflow
	tv.tv_sec += (nsec / NS_IN_SEC);
	tv.tv_nsec = (nsec % NS_IN_SEC);

	//Sleep might get interrupted due to signal delivery
	while (nanosleep(&tv, &tv) == -1 && errno == EINTR) {
		//Loop if interrupted by a signal
	}
}

size_t Timer::print(char *buffer, size_t size) noexcept {
	try {
		Time t(CLOCK_REALTIME); //Cannot fail
		auto format = "%Y_%m_%d-%H_%M_%S-%Z";
		return t.convert(buffer, size, format); //Can fail
	} catch (...) {
		return 0;
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
