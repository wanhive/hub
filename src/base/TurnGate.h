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
#include <pthread.h>

namespace wanhive {
/**
 * Turn gate for threads: allows a single thread to pass through.
 */
class TurnGate {
public:
	/**
	 * Default constructor: initializes the object
	 */
	TurnGate() noexcept;
	/**
	 * Destructor
	 */
	~TurnGate();
	/**
	 * Waits for a notification. If a notification become pending then exactly
	 * one of the competing threads is unblocked.
	 * @return always true
	 */
	bool wait();
	/**
	 * Waits for a notification or timeout. If a notification becomes pending
	 * then exactly one of the competing threads is unblocked. Waiting thread
	 * is unblocked on timeout.
	 * @param milliseconds timeout value in milliseconds
	 * @return true if a notification was received, false on timeout
	 */
	bool wait(unsigned int milliseconds);
	/**
	 * Delivers a notification to the waiting threads.
	 */
	void signal();
private:
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	size_t count { };
	bool flag { false };
};

} /* namespace wanhive */

#endif /* WH_BASE_TURNGATE_H_ */
