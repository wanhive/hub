/*
 * CSPRNG.cpp
 *
 * Cryptographically secure pseudo random number generator
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "CSPRNG.h"
#include "../Storage.h"
#include "../common/Exception.h"
#include <openssl/rand.h>

namespace wanhive {
class CSPRNG::RandomDevice {
public:
	RandomDevice() {
		random = -1;
		urandom = -1;
		try {
			random = Storage::open("/dev/random", O_RDONLY, S_IRUSR, false);
			urandom = Storage::open("/dev/urandom", O_RDONLY, S_IRUSR, false);
		} catch (const BaseException &e) {
			Storage::close(random);
			Storage::close(urandom);
			random = -1;
			urandom = -1;
			throw;
		}
	}

	~RandomDevice() {
		Storage::close(random);
		Storage::close(urandom);
	}

	size_t readData(void *block, size_t size, bool strong) const {
		return Storage::read(strong ? random : urandom, block, size, true);
	}
private:
	int random;
	int urandom;
};

const CSPRNG::RandomDevice CSPRNG::device;

bool CSPRNG::bytes(void *buffer, unsigned int count) noexcept {
	return (RAND_bytes((unsigned char*) buffer, count) == 1);
}

bool CSPRNG::seed(const void *data, int count) noexcept {
	RAND_seed(data, count);
	return (RAND_status() == 1);
}

void CSPRNG::random(void *buffer, unsigned int count, bool strong) {
	device.readData(buffer, count, strong);
}

} /* namespace wanhive */
