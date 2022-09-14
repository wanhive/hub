/*
 * Condition.h
 *
 * Thread signaling
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_CONDITION_H_
#define WH_BASE_CONDITION_H_
#include <pthread.h>

namespace wanhive {
/**
 * Signaling between threads without using the POSIX signals.
 * Uses a condition variable, a mutex, and a boolean field.
 */
class Condition {
public:
	/**
	 * Default constructor: initializes mutex and condition variable.
	 */
	Condition() noexcept;
	/**
	 * Destructor
	 */
	~Condition();
	/**
	 * Waits for a notification. If a notification become pending then exactly
	 * one of the competing threads gets woken up and the notification is cleared.
	 * @return always true
	 */
	bool wait();
	/**
	 * Waits for a notification or timeout. If a notification becomes pending then
	 * exactly one of the competing threads gets woken up and the notification is
	 * cleared.
	 * @param milliseconds timeout value in milliseconds
	 * @return true if a notification was received, false on timeout.
	 */
	bool timedWait(unsigned int milliseconds);
	/**
	 * Delivers a notification to the threads waiting on this object.
	 */
	void notify();
private:
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	size_t waiters { };
	bool flag { false };
};

} /* namespace wanhive */

#endif /* WH_BASE_CONDITION_H_ */
