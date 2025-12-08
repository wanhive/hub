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
	 * Default constructor: assigns zero time value.
	 */
	Time() noexcept;
	/**
	 * Constructor: sets a time value.
	 * @param ts time value
	 */
	Time(const timespec &ts) noexcept;
	/**
	 * Constructor: reads current time of a clock.
	 * @param id clock's identifier
	 */
	Time(clockid_t id);
	/**
	 * Destructor
	 */
	~Time();
	//-----------------------------------------------------------------
	/**
	 * Wrapper for clock_gettime(2): retrieves time of a clock.
	 * @param id clock's identifier
	 * @return time value
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
	//-----------------------------------------------------------------
	/**
	 * Wrapper for clock_gettime(2): retrieves time of a clock.
	 * @param id clock's identifier
	 * @param ts stores the time value
	 * @return true on success, false on error
	 */
	static bool now(clockid_t id, timespec &ts) noexcept;
	/**
	 * Wrapper for clock_gettime(2): retrieves time of a clock.
	 * @param id clock's identifier
	 * @param seconds stores the seconds value
	 * @return true on success, false on error
	 */
	static bool now(clockid_t id, double &seconds) noexcept;
	/**
	 * Wrapper for clock_gettime(2): retrieves time of a clock.
	 * @param id clock's identifier
	 * @param milliseconds stores the milliseconds value
	 * @return true on success, false on error
	 */
	static bool now(clockid_t id, long long &milliseconds) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Calculates a future time value.
	 * @param ts original time value as input, future time value as output
	 * @param offset future offset in milliseconds
	 */
	static void future(timespec &ts, unsigned int offset) noexcept;
	/**
	 * Converts milliseconds to time value.
	 * @param milliseconds milliseconds value
	 * @param nanoseconds optional nanoseconds value
	 * @return time value
	 */
	static timespec convert(unsigned int milliseconds,
			unsigned int nanoseconds = 0) noexcept;
	/**
	 * Converts a time value to seconds.
	 * @param ts time value
	 * @return seconds value
	 */
	static double seconds(const timespec &ts) noexcept;
	/**
	 * Converts a time value to milliseconds.
	 * @param ts time value
	 * @return milliseconds value
	 */
	static long long milliseconds(const timespec &ts) noexcept;
private:
	timespec ts;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_TIME_H_ */
