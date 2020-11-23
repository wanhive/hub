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
	bool hasTimedOut(unsigned int milliseconds,
			unsigned int nanoseconds = 0) const noexcept;
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

	//Create a timer file descriptor
	static int openTimerfd(bool blocking = false);
	//Set expiration and periodic interval in milliseconds, set <expiration> to 0 to disable
	static void setTimerfd(int fd, unsigned int expiration,
			unsigned int interval);
	//Returns current settings of a timer file descriptor in milliseconds
	static void getTimerfdSettings(int fd, unsigned int &expiration,
			unsigned int &interval);
	//Closes a timer file descriptor
	static int closeTimerfd(int fd) noexcept;
private:
	//Wrapper for clock_gettime, returns current time
	static unsigned long long currentTime() noexcept;
	//Returns the difference of microsecond precision <start> and <end> values in seconds
	static double difference(unsigned long long start,
			unsigned long long end) noexcept;
private:
	//Microseconds in a Second
	static constexpr unsigned long MS_IN_SEC = 1000000;
	//Nanoseconds in a Microsecond
	static constexpr unsigned int NS_IN_MS = 1000;
	//Microseconds in Milliseconds
	static constexpr unsigned int MS_IN_MILS = 1000;
	//Holds the current time with microseconds resolution, sufficient for 300,000 years :-)
	unsigned long long t;
};

} /* namespace wanhive */

#endif /* WH_BASE_TIMER_H_ */
