/*
 * Alarm.h
 *
 * Periodic timer
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_ALARM_H_
#define WH_HUB_ALARM_H_
#include "../reactor/Watcher.h"

namespace wanhive {
/**
 * Millisecond resolution periodic timer
 * Abstraction of the Linux's timerfd mechanism (timerfd_create(2))
 */
class Alarm final: public Watcher {
public:
	/**
	 * Constructor: creates a new timer (doesn't start it).
	 * @param expiration initial expiration in miliseconds (set 0 to disarm)
	 * @param interval interval for periodic timer in milliseconds
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 */
	Alarm(unsigned int expiration, unsigned int interval,
			bool blocking = false);
	/**
	 * Destructor
	 */
	~Alarm();
	//-----------------------------------------------------------------
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Reads the timer expiration count.
	 * @param count object for storing the expiration count
	 * @return the number of bytes read (8 bytes) on success, 0 if non-blocking
	 * mode is on and the call would block, -1 if the underlying file descriptor
	 * was closed.
	 */
	ssize_t read(unsigned long long &count);
	/**
	 * Updates the settings and restarts the timer.
	 * @param expiration initial expiration in miliseconds (set 0 to disarm)
	 * @param interval interval for periodic timer in milliseconds
	 */
	void reset(unsigned int expiration, unsigned int interval);
	/**
	 * Returns the initial expiration in milliseconds.
	 * @return initial expiration in miliseconds
	 */
	unsigned int getExpiration() const noexcept;
	/**
	 * Returns the periodic timer interval (repeated timer expiration after the
	 * initial expiration) in milliseconds.
	 * @return interval of periodic timer in milliseconds
	 */
	unsigned int getInterval() const noexcept;
private:
	//Creates a new Timer file descriptor
	void create(bool blocking);
	//Updates the settings
	void update(unsigned int expiration, unsigned int interval);
	//Fetches the settings
	void settings(unsigned int &expiration, unsigned int &interval);
private:
	unsigned int expiration;	//Initial expiration in miliseconds
	unsigned int interval;	//Interval for periodic timer in miliseconds
};

} /* namespace wanhive */

#endif /* WH_HUB_ALARM_H_ */
