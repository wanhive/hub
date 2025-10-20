/*
 * Mean.h
 *
 * Arithmetic mean calculator
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_MEAN_H_
#define WH_BASE_DS_MEAN_H_
#include "../common/reflect.h"

namespace wanhive {
/**
 * Arithmetic mean calculator.
 * @tparam X arithmetic data type
 */
template<typename X> class Mean {
public:
	/**
	 * Constructor: clears the accumulator.
	 */
	Mean() noexcept;
	/**
	 * Destructor
	 */
	~Mean();
	/**
	 * Accumulates an observed numerical value.
	 * @param value numerical value
	 * @param count observations count
	 */
	void accumulate(X value, unsigned int count = 1) noexcept;
	/**
	 * Clears the accumulator.
	 */
	void clear() noexcept;
	/**
	 * Checks if the accumulator contains sufficient number of observations.
	 * @param threshold minimum observations count
	 * @return true on sufficient count, false otherwise
	 */
	bool available(unsigned int threshold = 0) const noexcept;
	/**
	 * Calculates the arithmetic mean of the accumulated data.
	 * @param value stores the calculated arithmetic mean
	 * @param threshold minimum observations count
	 * @return true on success (sufficient data), false on failure
	 */
	bool calculate(X &value, unsigned int threshold = 0) const noexcept;
protected:
	/**
	 * Returns the sum of observed numerical values.
	 * @return sum of observations
	 */
	X sum() const noexcept;
	/**
	 * Returns the total number of observations.
	 * @return observations count
	 */
	unsigned long long count() const noexcept;
private:
	WH_ARITHMETIC_ASSERT(X);
	struct {
		X sum;
		unsigned long long count;
	} ctx;
};

} /* namespace wanhive */

template<typename X>
wanhive::Mean<X>::Mean() noexcept {
	clear();
}

template<typename X>
wanhive::Mean<X>::~Mean() {

}

template<typename X>
void wanhive::Mean<X>::accumulate(X value, unsigned int count) noexcept {
	ctx.sum += value;
	ctx.count += count;
}

template<typename X>
void wanhive::Mean<X>::clear() noexcept {
	ctx.sum = 0;
	ctx.count = 0;
}

template<typename X>
bool wanhive::Mean<X>::available(unsigned int threshold) const noexcept {
	return (ctx.count && ctx.count >= threshold);
}

template<typename X>
bool wanhive::Mean<X>::calculate(X &value,
		unsigned int threshold) const noexcept {
	if (available(threshold)) {
		value = (ctx.sum / ctx.count);
		return true;
	} else {
		return false;
	}
}

template<typename X>
X wanhive::Mean<X>::sum() const noexcept {
	return ctx.sum;
}

template<typename X>
unsigned long long wanhive::Mean<X>::count() const noexcept {
	return ctx.count;
}

#endif /* WH_BASE_DS_MEAN_H_ */
