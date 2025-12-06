/*
 * TurnGate.h
 *
 * Turn gate for threads
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_TURNGATE_H_
#define WH_BASE_TURNGATE_H_
#include "common/NonCopyable.h"
#include <pthread.h>

namespace wanhive {
/**
 * Turn gate for threads
 */
class TurnGate: private NonCopyable {
public:
	/**
	 * Constructor: initializes the synchronization mechanism.
	 */
	TurnGate() noexcept;
	/**
	 * Destructor: calls abort on system error.
	 */
	~TurnGate();
	/**
	 * Waits for a notification. If a notification become pending then exactly
	 * one of the competing threads gets unblocked.
	 * @return always true
	 */
	bool wait();
	/**
	 * Waits for a notification or timeout. If a notification becomes pending
	 * then exactly one of the competing threads gets unblocked. Unblocks the
	 * waiting threads on timeout.
	 * @param timeout wait period in milliseconds
	 * @return true if a notification arrived, false if timeout expired
	 */
	bool wait(unsigned int timeout);
	/**
	 * Delivers a notification to the waiting threads.
	 */
	void signal();
private:
	void lock();
	void unlock();
	void setup() noexcept;
	void cleanup() noexcept;
private:
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	size_t count { };
	bool flag { };
};

} /* namespace wanhive */

#endif /* WH_BASE_TURNGATE_H_ */
