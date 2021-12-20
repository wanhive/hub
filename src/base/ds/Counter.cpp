/*
 * Counter.cpp
 *
 * Up and down counter
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Counter.h"

namespace wanhive {

Counter::Counter(unsigned long long limit) noexcept :
		limit(limit), count(0) {

}

Counter::~Counter() {

}

unsigned long long Counter::getCount() const noexcept {
	return count;
}

bool Counter::setCount(unsigned long long count) noexcept {
	if (count <= limit) {
		this->count = count;
		return true;
	} else {
		return false;
	}
}

unsigned long long Counter::getLimit() const noexcept {
	return limit;
}

bool Counter::setLimit(unsigned long long limit) noexcept {
	if (limit >= count) {
		this->limit = limit;
		return true;
	} else {
		return false;
	}
}

void Counter::reset(unsigned long long limit) noexcept {
	this->limit = limit; //:-)
	count = 0;
}

bool Counter::up(unsigned int step) noexcept {
	if ((step <= limit) && (count <= (limit - step))) {
		count += step;
		return true;
	} else {
		return false;
	}
}

bool Counter::down(unsigned int step) noexcept {
	if (step <= count) {
		count -= step;
		return true;
	} else {
		return false;
	}
}
} /* namespace wanhive */
