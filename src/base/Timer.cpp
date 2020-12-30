/*
 * Timer.cpp
 *
 * Milliseconds precision stopwatch
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Timer.h"
#include "SystemException.h"
#include <cerrno>
#include <climits>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timerfd.h>

namespace {
void milsToSpec(unsigned int milliseconds, timespec &ts) {
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds - (ts.tv_sec * 1000)) * 1000000L;
}

long long specToMils(timespec &ts) {
	return (((long long) ts.tv_sec * 1000) + (ts.tv_nsec / 1000000L));
}

}  // namespace

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

bool Timer::hasTimedOut(unsigned int milliseconds,
		unsigned int nanoseconds) const noexcept {
	auto mark = currentTime();
	auto diff = ((unsigned long long) milliseconds) * MS_IN_MILS
			+ nanoseconds / NS_IN_MS;
	return (mark < t) || ((mark - t) > diff);
}

void Timer::sleep(unsigned int milliseconds, unsigned int nanoseconds) noexcept {
	struct timespec tv;
	//These many seconds
	tv.tv_sec = milliseconds / 1000;
	//These many nanoseconds
	auto nsec = (unsigned long long) ((milliseconds % 1000) * 1000000UL)
			+ nanoseconds;
	//Adjust for nanoseconds overflow
	tv.tv_sec += (nsec / 1000000000L);
	tv.tv_nsec = (nsec % 1000000000L);

	//Sleep might get interrupted due to signal delivery
	while (nanosleep(&tv, &tv) == -1 && errno == EINTR) {
		//Loop if interrupted by a signal
	}
}

size_t Timer::refractorTime(char *buffer, size_t size,
		const char *format) noexcept {
	time_t t = time(nullptr);
	format = format ? format : "%Y-%m-%d %H:%M:%S";
	return strftime(buffer, size, format, localtime(&t));
}

unsigned long long Timer::timeSeed() noexcept {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	//FVN-1a hash (Fowler–Noll–Vo hash)
	unsigned char *p = (unsigned char*) &ts;
	unsigned long long hash = 14695981039346656037ULL;
	for (size_t i = 0; i < sizeof(ts); ++i) {
		hash = 1099511628211ULL * (hash ^ p[i]);
	}
	return hash;
}

int Timer::openTimerfd(bool blocking) {
	int fd = timerfd_create(CLOCK_MONOTONIC, blocking ? 0 : TFD_NONBLOCK);
	if (fd != -1) {
		return fd;
	} else {
		throw SystemException();
	}
}

void Timer::setTimerfd(int fd, unsigned int expiration, unsigned int interval) {
	struct itimerspec time;
	milsToSpec(expiration, time.it_value);
	milsToSpec(interval, time.it_interval);
	if (timerfd_settime(fd, 0, &time, nullptr)) {
		throw SystemException();
	}
}

void Timer::getTimerfdSettings(int fd, unsigned int &expiration,
		unsigned int &interval) {
	struct itimerspec time;
	if (timerfd_gettime(fd, &time) == 0) {
		expiration = specToMils(time.it_value);
		interval = specToMils(time.it_interval);
	} else {
		throw SystemException();
	}
}

int Timer::closeTimerfd(int fd) noexcept {
	return close(fd);
}

unsigned long long Timer::currentTime() noexcept {
	timespec ts;
	//Cannot fail, return value ignored
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (((unsigned long long) ts.tv_sec * MS_IN_SEC)
			+ (ts.tv_nsec / NS_IN_MS));
}

double Timer::difference(unsigned long long start,
		unsigned long long end) noexcept {
	return ((double) (end - start) / (double) MS_IN_SEC);
}

} /* namespace wanhive */
