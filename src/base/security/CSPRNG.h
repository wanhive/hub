/**
 * @file CSPRNG.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Cryptographically secure PRNG
 */
class CSPRNG {
public:
	/**
	 * Uses libcrypto's CSPRNG to fill the given buffer with the given bytes
	 * of randomness.
	 * @param buffer output buffer
	 * @param count random bytes count
	 * @return true on success, false on failure
	 */
	static bool bytes(void *buffer, unsigned int count) noexcept;
	/**
	 * Seeds libcrypto's CSPRNG with the given bytes of random data.
	 * @param data data for seeding the generator
	 * @param count number of bytes of random data
	 * @return true if PRNG has been seeded with enough data, false otherwise
	 */
	static bool seed(const void *data, int count) noexcept;
	/**
	 * Reads the given bytes of randomness from operating systems's CSPRNG.
	 * @param buffer output buffer
	 * @param count random bytes count
	 * @param strong true for extra robustness, false otherwise
	 */
	static void random(void *buffer, unsigned int count, bool strong = false);
private:
	class RandomDevice;
	static const RandomDevice device;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_CSPRNG_H_ */
