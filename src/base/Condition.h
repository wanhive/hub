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
	Condition() noexcept;
	~Condition();
	/*
	 * Calling threads wait for notification. In case of pending notification
	 * exactly one of the competing threads is woken up and the notifications
	 * are cleared. Always returns true.
	 */
	bool wait();
	/*
	 * Calling threads wait until at most <milliseconds> duration for a
	 * notification. In case of pending notification competing threads may
	 * be spuriously woken up, however exactly one of them sees <true> as
	 * the returned value and the notifications are cleared. Returns false
	 * on timeout.
	 */
	bool timedWait(unsigned int milliseconds);
	//Delivers a notification to this object.
	void notify();
private:
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	bool flag;
};

} /* namespace wanhive */

#endif /* WH_BASE_CONDITION_H_ */
