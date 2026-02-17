/**
 * @file Alarm.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * @brief Millisecond precision periodic timer
 * @details Abstraction of the Linux's timerfd mechanism (timerfd_create(2))
 */
class Alarm final: public Watcher {
public:
	/**
	 * Constructor: creates a new periodic timer.
	 * @param period timer's settings
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
	 * Reads the periodic timer's expiration count.
	 * @param count stores the expiration count
	 * @return bytes read (8 bytes) on success, 0 if non-blocking mode is
	 * enabled and the call would block, -1 if the file descriptor is closed.
	 */
	ssize_t read(unsigned long long &count);
	/**
	 * Resets and restarts the periodic timer with new settings.
	 * @param period new settings
	 */
	void reset(const Period &period);
	/**
	 * Gets the periodic timer's current settings.
	 * @return current settings
	 */
	const Period& settings() const noexcept;
private:
	void create(bool blocking);
	void update(const Period &period);
	void retrieve(Period &period);
private:
	Period period;
};

} /* namespace wanhive */

#endif /* WH_HUB_ALARM_H_ */
