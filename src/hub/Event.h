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
 * Abstraction of linux's event wait/notify mechanism (see eventfd(2))
 */
class Event final: public Watcher {
public:
	/**
	 * Constructor: creates a new events counter.
	 * @param semaphore true for semaphore-like behavior, false otherwise (see
	 * EventNotifier::read()).
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
	 * Reads the events count. If semaphore-like behavior was enabled then each
	 * call decrements the counter by 1 (reads one single event), otherwise the
	 * counter is reset (reads all events). Call EventNotifier::getCount() to get
	 * the result.
	 * @return the number of bytes read (8 bytes) on success, 0 if non-blocking
	 * mode is on and the call would block, -1 if the underlying file descriptor
	 * was closed.
	 */
	ssize_t read();
	/**
	 * Adds the given value to the events counter.
	 * @param events the value to add to the events counter
	 * @return the number of bytes written (8 bytes) on success, 0 if non-blocking
	 * mode is on and the call would block.
	 */
	ssize_t write(unsigned long long events);
	/**
	 * Returns the events count (Each new EventNotifier::read() call overwrites
	 * the old value).
	 * @return the events count
	 */
	unsigned long long getCount() const noexcept;
private:
	unsigned long long count;	//Events count
};

} /* namespace wanhive */

#endif /* WH_HUB_EVENT_H_ */
