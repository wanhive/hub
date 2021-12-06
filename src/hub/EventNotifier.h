/*
 * EventNotifier.h
 *
 * Event notification mechanism for Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_EVENTNOTIFIER_H_
#define WH_HUB_EVENTNOTIFIER_H_
#include "../reactor/Watcher.h"

namespace wanhive {
/**
 * Abstraction of linux's event wait/notify mechanism
 * REF: eventfd(2)
 */
class EventNotifier: public Watcher {
public:
	EventNotifier(bool semaphore, bool blocking = false);
	virtual ~EventNotifier();
	//-----------------------------------------------------------------
	//Start the notifier (no-op)
	void start() override final;
	//Disarm the notifier (no-op)
	void stop() noexcept override final;
	//Handle the event notification
	bool callback(void *arg) noexcept override final;
	//Always returns false
	bool publish(void *arg) noexcept override final;
	//-----------------------------------------------------------------
	/*
	 * Returns the number of bytes read, possibly zero (buffer full or would
	 * block), or -1 if the descriptor was closed. Each new call overwrites
	 * the event count.
	 */
	ssize_t read();
	/*
	 * Returns the number of bytes written, possibly zero (buffer full or
	 * would block). Clears out the write IO event reported on this watcher
	 * if the call would block.
	 */
	ssize_t write(unsigned long long events);
	//-----------------------------------------------------------------
	//Returns the number of events
	unsigned long long getCount() const noexcept;
private:
	unsigned long long count;	//Events count
};

} /* namespace wanhive */

#endif /* WH_HUB_EVENTNOTIFIER_H_ */
