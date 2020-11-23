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
#include <cstring>

namespace wanhive {

Topic::Topic() noexcept :
		n(0) {
	memset(&map, 0, sizeof(map));
}

Topic::~Topic() {

}

bool Topic::set(unsigned int id) noexcept {
	if (id < COUNT && !Twiddler::test(map, id)) {
		Twiddler::set(map, id);
		++n;
		return true;
	} else {
		return (id < COUNT);
	}
}

void Topic::clear(unsigned int id) noexcept {
	if (id < COUNT && n && Twiddler::test(map, id)) {
		Twiddler::clear(map, id);
		--n;
	}
}

bool Topic::test(unsigned int id) const noexcept {
	if (id < COUNT && n) {
		return Twiddler::test(map, id);
	} else {
		return false;
	}
}

unsigned int Topic::count() const noexcept {
	return n;
}

} /* namespace wanhive */
