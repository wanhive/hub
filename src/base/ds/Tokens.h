/*
 * Tokens.h
 *
 * Token Bucket
 *
 *
 * Copyright (C) 2024 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_TOKENS_H_
#define WH_BASE_DS_TOKENS_H_
#include "Counter.h"

namespace wanhive {
/**
 * Token bucket implementation.
 */
class Tokens: private Counter {
public:
	/**
	 * Default constructor: creates an inactive and empty token bucket.
	 */
	Tokens() noexcept;
	/**
	 * Constructor: creates an active token bucket with the given initial count.
	 * @param count initial tokens count
	 */
	Tokens(unsigned int count) noexcept;
	/**
	 * Destructor
	 */
	~Tokens();
	/**
	 * Fills up and activates the token bucket.
	 * @param count new tokens count
	 */
	void fill(unsigned int count) noexcept;
	/**
	 * Acquires and returns all the tokens that are immediately available.
	 * @return acquired tokens
	 */
	unsigned int drain() noexcept;
	/**
	 * Acquires the given number of tokens. The call always succeeds for an
	 * inactive token bucket.
	 * @param count number of tokens to acquire
	 * @return true on success, false on failure (insufficient tokens)
	 */
	bool take(unsigned int count = 1) noexcept;
	/**
	 * Releases the given number of tokens into the bucket and activates it.
	 * @param count number of tokens to release
	 * @return true on success, false on error (overflow)
	 */
	bool add(unsigned int count) noexcept;
	/**
	 * Returns the current number of available tokens.
	 * @return number of currently available tokens
	 */
	unsigned int available() const noexcept;
private:
	bool active;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_TOKENS_H_ */
