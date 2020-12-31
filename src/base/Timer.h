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

	//Creates a timer file descriptor
	static int openTimerfd(bool blocking = false);
	/*
	 * Sets expiration and periodic interval in milliseconds.
	 * Setting <expiration> to zero(0) will disable the timer.
	 */
	static void setTimerfd(int fd, unsigned int expiration,
			unsigned int interval);
	//Returns the settings of a timer file descriptor in milliseconds
	static void getTimerfdSettings(int fd, unsigned int &expiration,
			unsigned int &interval);
	//Closes a timer file descriptor
	static int closeTimerfd(int fd) noexcept;
private:
	//Wrapper for clock_gettime, returns current time
	static unsigned long long currentTime() noexcept;
	//Returns the difference of the microsecond precision values in seconds
	static double difference(unsigned long long start,
			unsigned long long end) noexcept;
private:
	//Time in microseconds, sufficient for 300,000 years
	unsigned long long t;
};

} /* namespace wanhive */

#endif /* WH_BASE_TIMER_H_ */
