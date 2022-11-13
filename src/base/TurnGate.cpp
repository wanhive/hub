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
#include <cerrno>
#include <cstdlib>

namespace wanhive {

TurnGate::TurnGate() noexcept {
	mutex = PTHREAD_MUTEX_INITIALIZER;
	condition = PTHREAD_COND_INITIALIZER;
}

TurnGate::~TurnGate() {
	if (pthread_cond_destroy(&condition) != 0) {
		abort();
	}

	if (pthread_mutex_destroy(&mutex) != 0) {
		abort();
	}
}

bool TurnGate::wait() {
	auto status = pthread_mutex_lock(&mutex);
	if (status == 0) {
		++count;
		int rc = 0;
		while (!flag && rc == 0) {
			rc = pthread_cond_wait(&condition, &mutex);
		}
		--count;
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

bool TurnGate::wait(unsigned int milliseconds) {
	auto status = pthread_mutex_lock(&mutex);
	if (status == 0) {
		++count;
		int rc = 0;
		if (!flag && milliseconds) {
			struct timespec tv;
			clock_gettime(CLOCK_REALTIME, &tv);

			tv.tv_sec += milliseconds / 1000;
			tv.tv_nsec += (milliseconds % 1000) * 1000000L;
			//Adjust for overflow
			tv.tv_sec += tv.tv_nsec / 1000000000L;
			tv.tv_nsec %= 1000000000L;
			while (!flag && rc == 0) {
				rc = pthread_cond_timedwait(&condition, &mutex, &tv);
			}
		}
		--count;
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

void TurnGate::signal() {
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
