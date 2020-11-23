/*
 * CSPRNG.h
 *
 * Cryptographically secure pseudo random number generator
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_CSPRNG_H_
#define WH_BASE_SECURITY_CSPRNG_H_

namespace wanhive {
/**
 * Cryptographically secure PRNG
 * (Thread safe)
 */
class CSPRNG {
public:
	/*
	 * Uses libcrypto's CSPRNG to fill <block> with <count> bytes of randomness
	 * Returns true on success, false otherwise
	 */
	static bool bytes(void *block, unsigned int count) noexcept;
	/*
	 * Seeds libcrypto's CSPRNG with <count> bytes of random data from <buf>
	 * Returns true if the PRNG has been seeded with enough data, falsew otherwise
	 */
	static bool seed(const void *buf, int count) noexcept;
	/*
	 * Fills <block> with <count> bytes of randomness. If <strong> is true then
	 * reads from /dev/random, otherwise /dev/urandom is used.
	 */
	static void random(void *block, unsigned int count, bool strong = false);
private:
	class RandomDevice;  //Forward declaration
	static const RandomDevice device;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_CSPRNG_H_ */
