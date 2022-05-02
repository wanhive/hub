/*
 * Random.h
 *
 * Cryptographically secure pseudo random number generator
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_RANDOM_H_
#define WH_UTIL_RANDOM_H_

namespace wanhive {
/**
 * Cryptographically secure PRNG.
 * Thread safe at class level.
 */
class Random {
public:
	/**
	 * Constructor
	 */
	Random() noexcept;

	/**
	 * Destructor
	 */
	~Random();

	/**
	 * Generates pseudo random bytes
	 *
	 * @param block buffer for storing the output
	 * @param count number of bytes to generate
	 */
	void bytes(void *block, unsigned int count);
};

} /* namespace wanhive */

#endif /* WH_UTIL_RANDOM_H_ */
