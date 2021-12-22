/*
 * Condition.cpp
 *
 * Thread signaling
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Condition.h"
#include "unix/SystemException.h"
#include <cerrno>
#include <cstdlib>

namespace wanhive {

Condition::Condition() noexcept {
	mutex = PTHREAD_MUTEX_INITIALIZER;
	condition = PTHREAD_COND_INITIALIZER;
	flag = false;
}

Condition::~Condition() {
	if (pthread_cond_destroy(&condition) != 0) {
		abort();
	}

	if (pthread_mutex_destroy(&mutex) != 0) {
		abort();
	}
}

bool Condition::wait() {
	auto status = pthread_mutex_lock(&mutex);
	if (status == 0) {
		int rc = 0;
		while (!flag && rc == 0) {
			rc = pthread_cond_wait(&condition, &mutex);
		}
		auto value = flag;
		flag = false;
		pthread_mutex_unlock(&mutex);

		if (rc == 0) {
			return value;
		} else {
			throw SystemException(rc);
		}
	} else {
		throw SystemException(status);
	}
}

bool Condition::timedWait(unsigned int milliseconds) {
	auto status = pthread_mutex_lock(&mutex);
	if (status == 0) {
		int rc = 0;
		if (!flag && milliseconds) {
			struct timespec tv;
			clock_gettime(CLOCK_REALTIME, &tv);

			tv.tv_sec += milliseconds / 1000;
			tv.tv_nsec += (milliseconds % 1000) * 1000000L;
			//Adjust for overflow
			tv.tv_sec += tv.tv_nsec / 1000000000L;
			tv.tv_nsec %= 1000000000L;

			rc = pthread_cond_timedwait(&condition, &mutex, &tv);
		}
		auto value = flag;
		flag = false;
		pthread_mutex_unlock(&mutex);

		if (rc == 0 || rc == ETIMEDOUT) {
			return value;
		} else {
			throw SystemException(rc);
		}
	} else {
		throw SystemException(status);
	}
}

void Condition::notify() {
	auto status = pthread_mutex_lock(&mutex);
	if (status == 0) {
		flag = true;
		pthread_mutex_unlock(&mutex);
		auto rc = pthread_cond_signal(&condition);
		if (rc != 0) {
			throw SystemException(rc);
		} else {
			return;
		}
	} else {
		throw SystemException(status);
	}
}

} /* namespace wanhive */
