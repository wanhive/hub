/*
 * Timer.h
 *
 * Milliseconds precision timer
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_TIMER_H_
#define WH_BASE_TIMER_H_
#include <cstddef>

namespace wanhive {
/**
 * Milliseconds precision "monotonic" time keeper
 */
class Timer {
public:
	/**
	 * Default constructor: creates a new timer.
	 */
	Timer() noexcept;
	/**
	 * Destructor
	 */
	~Timer();
	/**
	 * Returns the elapsed time since this timer's creation.
	 * @return elapsed time in seconds
	 */
	double elapsed() const noexcept;
	/**
	 * Reset the timer, updates it with current time.
	 */
	void now() noexcept;
	/**
	 * Checks if this timer has outlived the timeout value since the last reset.
	 * @param milliseconds the timeout value in milliseconds
	 * @return true on timeout, false otherwise
	 */
	bool hasTimedOut(unsigned int milliseconds) const noexcept;
	/**
	 * Sleeps for the specified duration (cannot be interrupted by signal).
	 * @param milliseconds milliseconds component of the duration
	 * @param nanoseconds nanoseconds component of the duration
	 */
	static void sleep(unsigned int milliseconds,
			unsigned int nanoseconds = 0) noexcept;
	/**
	 * Converts the current wall-clock time (UTC) to human readable string.
	 * @param buffer output buffer for storing the formatted string
	 * @param size buffer's size in bytes
	 * @param format date and time representation format
	 * @return formatted string's size in bytes on success, 0 on failure
	 */
	static size_t print(char *buffer, size_t size,
			const char *format = nullptr) noexcept;
	/**
	 * Converts unix timestamp to an ISO 8601 formatted string.
	 * @param timestamp unix timestamp with fractional seconds
	 * @param buffer output buffer for storing the formatted string
	 * @param size buffer's size in bytes
	 * @return formatted string's size in bytes on success, 0 on failure
	 */
	static size_t print(double timestamp, char *buffer, size_t size) noexcept;
	/**
	 * Generate a 64-bit value for seeding the non-cryptographic RNGs.
	 * @return 64-byte value based on the current time-stamp.
	 */
	static unsigned long long timeSeed() noexcept;
private:
	static unsigned long long currentTime() noexcept;
	static double difference(unsigned long long start,
			unsigned long long end) noexcept;
public:
	/*! ISO8601 (UTC) format for date and time representation */
	static constexpr const char *ISO8601_UTC_FMT = "%Y-%m-%dT%H:%M:%SZ";
private:
	//Time in microseconds, sufficient for 300,000 years
	unsigned long long t;
};

} /* namespace wanhive */

#endif /* WH_BASE_TIMER_H_ */
