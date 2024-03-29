/*
 * Time.h
 *
 * Time keeper
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_TIME_H_
#define WH_BASE_UNIX_TIME_H_
#include <ctime>

namespace wanhive {
/**
 * Time keeper
 */
class Time {
public:
	/**
	 * Default constructor: sets zero timestamp.
	 */
	Time() noexcept;
	/**
	 * Constructor: assigns a timestamp.
	 * @param ts timestamp's value
	 */
	Time(const timespec &ts) noexcept;
	/**
	 * Constructor: reads current time from the given clock.
	 * @param id clock's identifier
	 */
	Time(clockid_t id);
	/**
	 * Destructor
	 */
	~Time();
	//-----------------------------------------------------------------
	/**
	 * Wrapper for clock_gettime(2): updates the timestamp.
	 * @param id clock's identifier
	 * @return current time
	 */
	const timespec& now(clockid_t id);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for strftime(3): formats the most recently refreshed timestamp.
	 * @param s buffer to store the formatted string
	 * @param size buffer's size in bytes
	 * @param format output's format
	 * @param utc true for UTC, false for local time
	 * @return output string size in bytes
	 */
	size_t convert(char *s, size_t size, const char *format = nullptr,
			bool utc = true) const;
	/**
	 * Wrapper for localtime_r(3) and gmtime_r(3): returns the broken down time.
	 * @param calendar stores the broken-down time
	 * @param utc true for UTC, false for local time
	 */
	void convert(tm &calendar, bool utc = true) const;
	//-----------------------------------------------------------------
	/**
	 * Returns the most recently refreshed timestamp.
	 * @return timestamp's value
	 */
	const timespec& get() const noexcept;
	/**
	 * Sets timestamp's value.
	 * @param ts new timestamp value
	 */
	void set(const timespec &ts) noexcept;
private:
	timespec ts;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_TIME_H_ */
