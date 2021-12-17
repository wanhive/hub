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
	Time() noexcept;
	Time(const timespec &ts) noexcept;
	Time(clockid_t id);
	~Time();

	//Wrapper for clock_gettime(2): reads the current time
	const timespec& now(clockid_t id);
	//Wrapper for strftime(3): formats the most recently refreshed time-stamp
	size_t convert(char *s, size_t size, const char *format = nullptr,
			bool utc = false) const;
	//Wrapper for localtime_r(3) and gmtime_r(3): returns the broken down time
	void convert(tm &calendar, bool utc = false) const;
	//Returns the most recently refreshed time-stamp
	const timespec& get() const noexcept;
	//Sets the time-stamp to the given value
	void set(const timespec &ts) noexcept;
private:
	timespec ts;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_TIME_H_ */
