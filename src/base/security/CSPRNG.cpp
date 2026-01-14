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
#include "../common/BaseException.h"
#include <openssl/rand.h>

namespace wanhive {
class CSPRNG::RandomDevice {
public:
	RandomDevice() {
		try {
			random = Storage::open("/dev/random", O_RDONLY);
			urandom = Storage::open("/dev/urandom", O_RDONLY);
		} catch (const BaseException &e) {
			reset();
			throw;
		}
	}

	~RandomDevice() {
		reset();
	}

	size_t read(void *data, size_t size, bool strong) const {
		return Storage::read(strong ? random : urandom, data, size, true);
	}
private:
	void reset() noexcept {
		Storage::close(random);
		Storage::close(urandom);
		random = -1;
		urandom = -1;
	}
private:
	int random { -1 };
	int urandom { -1 };
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
	device.read(buffer, count, strong);
}

} /* namespace wanhive */
