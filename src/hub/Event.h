/*
 * Event.h
 *
 * Events counter
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_EVENT_H_
#define WH_HUB_EVENT_H_
#include "../reactor/Watcher.h"

namespace wanhive {
/**
 * Events counter
 * @note Abstraction of linux's event wait/notify mechanism (see eventfd(2))
 */
class Event final: public Watcher {
public:
	/**
	 * Constructor: creates a new events counter.
	 * @param semaphore true for semaphore-like semantics, false otherwise
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 */
	Event(bool semaphore, bool blocking = false);
	/**
	 * Destructor
	 */
	~Event();
	//-----------------------------------------------------------------
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Reads the events count. If semaphore semantics is active then each call
	 * decrements the counter by 1 (reads one single event), otherwise the
	 * counter is reset (reads all events).
	 * @param count stores the events count
	 * @return number of bytes read (8 bytes) on success, 0 if non-blocking mode
	 * is on and the call would block, -1 if the file descriptor was closed.
	 */
	ssize_t read(unsigned long long &count);
	/**
	 * Adds an 8-byte integer value to the counter.
	 * @param count value to add
	 * @return number of bytes written (8 bytes) on success, 0 if non-blocking
	 * mode is on and the call would block.
	 */
	ssize_t write(unsigned long long count);
};

} /* namespace wanhive */

#endif /* WH_HUB_EVENT_H_ */
