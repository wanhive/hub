/*
 * Topic.cpp
 *
 * Topic-based subscription management
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Topic.h"
#include "../base/ds/Twiddler.h"

namespace wanhive {

Topic::Topic() noexcept {

}

Topic::~Topic() {

}

bool Topic::set(unsigned int id) noexcept {
	if (id < COUNT && !Twiddler::test(bits, id)) {
		Twiddler::set(bits, id);
		_count += 1;
		return true;
	} else {
		return (id < COUNT);
	}
}

void Topic::clear(unsigned int id) noexcept {
	if (id < COUNT && _count && Twiddler::test(bits, id)) {
		Twiddler::clear(bits, id);
		_count -= 1;
	}
}

bool Topic::test(unsigned int id) const noexcept {
	if (id < COUNT && _count) {
		return Twiddler::test(bits, id);
	} else {
		return false;
	}
}

unsigned int Topic::count() const noexcept {
	return _count;
}

} /* namespace wanhive */
