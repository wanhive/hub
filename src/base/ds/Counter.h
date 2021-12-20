/*
 * Counter.h
 *
 * Up and down counter
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_COUNTER_H_
#define WH_BASE_DS_COUNTER_H_

namespace wanhive {
/**
 * Up and down counter
 */
class Counter {
public:
	//Creates a new counter and sets the limit
	Counter(unsigned long long limit = 0) noexcept;
	~Counter();

	//Returns the current total count
	unsigned long long getCount() const noexcept;
	/*
	 * Sets the total count. Fails if the given value is greater than
	 * the current limit. Returns true on success, false otherwise.
	 */
	bool setCount(unsigned long long count) noexcept;
	//Returns the current limit
	unsigned long long getLimit() const noexcept;
	/*
	 * Sets a new limit, fails if the given value is less than the
	 * current count. Returns true on success, false otherwise.
	 */
	bool setLimit(unsigned long long limit) noexcept;
	//Clears out the total count and sets a new limit
	void reset(unsigned long long limit) noexcept;
	/*
	 * Increments the total count by <step> if the resulting total count
	 * will not violate the current limit and returns true. Fails and
	 * returns false otherwise.
	 */
	bool up(unsigned int step = 1) noexcept;
	/*
	 * Decrements the total count by <step> if the resulting total count will
	 * not become negative and returns true. Fails and returns false otherwise.
	 */
	bool down(unsigned int step = 1) noexcept;
private:
	unsigned long long limit; //Current upper limit (inclusive)
	unsigned long long count; //Current total count
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_COUNTER_H_ */
