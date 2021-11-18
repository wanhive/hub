/*
 * Timer.h
 *
 * Milliseconds precision stop-watch
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
	Timer() noexcept;
	~Timer();

	//Returns this timer's age since the last reset in seconds
	double elapsed() const noexcept;
	//Reset the timer, load it with current time
	void now() noexcept;
	//Returns true if a timeout was detected since the last reset
	bool hasTimedOut(unsigned int milliseconds) const noexcept;
	//Sleep for specified duration (cannot be interrupted by signal)
	static void sleep(unsigned int milliseconds,
			unsigned int nanoseconds = 0) noexcept;
	/*
	 * Wrapper for strftime (man strftime)
	 * Returns the number of bytes (excl. null terminator) placed in <buffer>
	 * BUFFER: <size> = 20 sufficient for YYYY-MM-DD HH:MM:SS and terminator
	 */
	static size_t refractorTime(char *buffer, size_t size, const char *format =
			nullptr) noexcept;
	//Generate a 64-bit seed from current time for seeding the RNGs
	static unsigned long long timeSeed() noexcept;
private:
	//Wrapper for clock_gettime, returns current time
	static unsigned long long currentTime() noexcept;
	//Returns the difference of the microsecond precision values in seconds
	static double difference(unsigned long long start,
			unsigned long long end) noexcept;
public:
	//Nanoseconds in a Second
	static constexpr long NS_IN_SEC = 1000000000L;
	//Microseconds in a Second
	static constexpr long MS_IN_SEC = 1000000L;
	//Milliseconds in a Second
	static constexpr long MILS_IN_SEC = 1000L;
	//Nanoseconds in a Microsecond
	static constexpr long NS_IN_MS = (NS_IN_SEC / MS_IN_SEC);
	//Nanoseconds in a Millisecond
	static constexpr long NS_IN_MILS = (NS_IN_SEC / MILS_IN_SEC);
	//Microseconds in a Millisecond
	static constexpr long MS_IN_MILS = (MS_IN_SEC / MILS_IN_SEC);
private:
	//Time in microseconds, sufficient for 300,000 years
	unsigned long long t;
};

} /* namespace wanhive */

#endif /* WH_BASE_TIMER_H_ */
