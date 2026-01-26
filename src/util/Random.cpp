/*
 * Random.cpp
 *
 * Cryptographic pseudo random number generator
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

void Random::bytes(void *buffer, unsigned int count) {
	if (!CSPRNG::bytes(buffer, count)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
