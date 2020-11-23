/*
 * Clock.h
 *
 * Millisecond resolution time-keeper for Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_CLOCK_H_
#define WH_HUB_CLOCK_H_
#include "../reactor/Watcher.h"

namespace wanhive {
/**
 * Millisecond resolution time keeper
 * Abstraction of Linux's timerfd mechanism (timerfd_create(2))
 */
class Clock: public Watcher {
public:
	//Creates however doesn't arm the clock
	Clock(unsigned int expiration, unsigned int interval,
			bool blocking = false);
	virtual ~Clock();
	//-----------------------------------------------------------------
	//Start the clock with the current settings
	void start() override final;
	//Disarm the clock, current settings are preserved
	void stop() noexcept override final;
	//Handle the periodic timer expiration notification
	bool callback(void *arg) noexcept override final;
	//Always returns false
	bool publish(void *arg) noexcept override final;
	//-----------------------------------------------------------------
	/*
	 * Returns the number of bytes read, possibly zero (buffer full or would block),
	 * or -1 if the descriptor got closed. Fresh call overwrites the expiration count.
	 */
	ssize_t read();
	/*
	 * Updates clock's settings and restarts it.
	 * Setting <expiration> to zero (0) will disarm the clock.
	 */
	void reset(unsigned int expiration, unsigned int interval);
	//-----------------------------------------------------------------
	//Initial expiration in milliseconds
	unsigned int getExpiration() const noexcept;
	//Period in milliseconds
	unsigned int getInterval() const noexcept;
	//Return the number of times the periodic timer expired
	unsigned long long getCount() const noexcept;
private:
	unsigned int expiration;	//Initial expiration in miliseconds
	unsigned int interval;	//Interval for periodic timer in miliseconds
	unsigned long long count; //Timer expirations count
};

} /* namespace wanhive */

#endif /* WH_HUB_CLOCK_H_ */
