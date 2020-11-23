/*
 * Random.cpp
 *
 * Cryptographically secure pseudo random number generator for Wanhive
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Random.h"
#include "../base/common/Exception.h"
#include "../base/security/CSPRNG.h"

namespace wanhive {

Random::Random() noexcept {

}

Random::~Random() {

}

void Random::bytes(void *block, unsigned int count) {
	if (!CSPRNG::bytes(block, count)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
