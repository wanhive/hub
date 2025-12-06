/*
 * TurnGate.cpp
 *
 * Turn gate for threads
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "TurnGate.h"
#include "unix/SystemException.h"
#include "unix/Time.h"
#include <cerrno>
#include <cstdlib>

namespace wanhive {

TurnGate::TurnGate() noexcept {
	setup();
}

TurnGate::~TurnGate() {
	cleanup();
}

bool TurnGate::wait() {
	lock();

	int rc = 0;
	while (!flag && rc == 0) {
		rc = pthread_cond_wait(&condition, &mutex);
	}

	auto value = flag;
	flag = false;
	unlock();

	if (rc == 0) {
		return value;
	} else {
		throw SystemException(rc);
	}
}

bool TurnGate::wait(unsigned int timeout) {
	lock();

	int rc = 0;
	if (!flag && timeout) {
		timespec tv;
		if (!Time::now(CLOCK_REALTIME, tv)) {
			unlock();
			throw SystemException();
		}
		Time::future(tv, timeout);

		while (!flag && rc == 0) {
			rc = pthread_cond_timedwait(&condition, &mutex, &tv);
		}
	}

	auto value = flag;
	flag = false;
	unlock();

	if (rc == 0 || rc == ETIMEDOUT) {
		return value;
	} else {
		throw SystemException(rc);
	}
}

void TurnGate::signal() {
	lock();
	flag = true;
	unlock();

	if (auto rc = pthread_cond_signal(&condition); rc != 0) {
		throw SystemException(rc);
	}
}

void TurnGate::lock() {
	if (auto rc = pthread_mutex_lock(&mutex); rc != 0) {
		throw SystemException(rc);
	}
}

void TurnGate::unlock() {
	if (auto rc = pthread_mutex_unlock(&mutex); rc != 0) {
		throw SystemException(rc);
	}
}

void TurnGate::setup() noexcept {
	mutex = PTHREAD_MUTEX_INITIALIZER;
	condition = PTHREAD_COND_INITIALIZER;
}

void TurnGate::cleanup() noexcept {
	if (pthread_cond_destroy(&condition) != 0) {
		abort();
	}

	if (pthread_mutex_destroy(&mutex) != 0) {
		abort();
	}
}

} /* namespace wanhive */
