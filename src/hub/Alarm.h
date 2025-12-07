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
#include "../base/ds/Spatial.h"
#include "../reactor/Watcher.h"

namespace wanhive {
/**
 * Millisecond precision periodic timer
 * @note Abstraction of the Linux's timerfd mechanism (timerfd_create(2))
 */
class Alarm final: public Watcher {
public:
	/**
	 * Constructor: creates a new timer.
	 * @param period timer's period
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 */
	Alarm(const Period &period, bool blocking = false);
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
	 * @param count stores the expiration count
	 * @return number of bytes read (8 bytes) on success, 0 if non-blocking mode
	 * is on and the call would block, -1 if the file descriptor was closed.
	 */
	ssize_t read(unsigned long long &count);
	/**
	 * Resets timer's settings and restarts it.
	 * @param period timer's period
	 */
	void reset(const Period &period);
	/**
	 * Returns timer's current settings.
	 * @return timer's period
	 */
	const Period& getPeriod() const noexcept;
private:
	void create(bool blocking);
	void update(const Period &period);
	void settings(Period &period);
private:
	Period period;
};

} /* namespace wanhive */

#endif /* WH_HUB_ALARM_H_ */
