/*
 * UID.cpp
 *
 * Unique identifier
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "UID.h"
#include "../common/Atomic.h"

namespace wanhive {

unsigned long long UID::_next = UID::MIN;

UID::UID() noexcept :
		uid(next()) {

}

UID::~UID() {

}

void UID::set(unsigned long long id) noexcept {
	this->uid = id;
}

unsigned long long UID::get() const noexcept {
	return uid;
}

unsigned long long UID::next() noexcept {
	//For all the practical purposes this is sufficient
	return Atomic<unsigned long long>::fetchAndAdd(&_next, 1);
}

} /* namespace wanhive */
