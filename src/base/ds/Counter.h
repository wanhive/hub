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
	/**
	 * Constructor: creates a new counter with the given limit.
	 * @param limit counter's upper limit
	 */
	Counter(unsigned long long limit = 0) noexcept;
	/**
	 * Destructor
	 */
	~Counter();
	/**
	 * Returns the current total count.
	 * @return total count
	 */
	unsigned long long getCount() const noexcept;
	/**
	 * Overrides the total count. This call will fail if given value is greater
	 * than the current limit.
	 * @param count the new total count
	 * @return true on success, false otherwise (invalid count)
	 */
	bool setCount(unsigned long long count) noexcept;
	/**
	 * Returns the current upper limit.
	 * @return current limit
	 */
	unsigned long long getLimit() const noexcept;
	/**
	 * Sets a new upper limit. This call will fail if the given value is less
	 * than the current count.
	 * @param limit the new upper limit
	 * @return true on success, false otherwise (invalid limit)
	 */
	bool setLimit(unsigned long long limit) noexcept;
	/**
	 * Resets the counter: clears out the total count and sets a new limit.
	 * @param limit the new upper limit
	 */
	void reset(unsigned long long limit) noexcept;
	/**
	 * Increments the total count. This call will fail if as a result of the
	 * operation the total count will become larger than the current limit.
	 * @param step value to add to the total count
	 * @return true on success, false otherwise
	 */
	bool up(unsigned int step = 1) noexcept;
	/**
	 * Decrements the total count. This call will fail if as a result of the
	 * operation the total count will wrap around (underflow).
	 * @param step value to subtract from the total count
	 * @return true on success, false otherwise
	 */
	bool down(unsigned int step = 1) noexcept;
private:
	unsigned long long limit; //upper limit (inclusive)
	unsigned long long count; //total count
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_COUNTER_H_ */
