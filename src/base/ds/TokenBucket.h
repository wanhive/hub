/*
 * TokenBucket.h
 *
 * Token Bucket
 *
 *
 * Copyright (C) 2024 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_TOKENBUCKET_H_
#define WH_BASE_DS_TOKENBUCKET_H_
#include "Counter.h"

namespace wanhive {
/**
 * Token bucket implementation
 */
class TokenBucket: private Counter {
public:
	/**
	 * Default constructor: initializes an inactive and empty bucket.
	 */
	TokenBucket() noexcept;
	/**
	 * Constructor: initializes an active bucket.
	 * @param tokens initial tokens count
	 */
	TokenBucket(unsigned long long tokens) noexcept;
	/**
	 * Destructor
	 */
	~TokenBucket() noexcept;
	/**
	 * Refills and activates the bucket.
	 * @param tokens new tokens count
	 */
	void fill(unsigned long long tokens) noexcept;
	/**
	 * Adds a given number of tokens into the bucket and activates it. On
	 * overflow, the additional tokens are silently discarded.
	 * @param count additional tokens count
	 */
	void add(unsigned int count) noexcept;
	/**
	 * Checks and cashes in the given number of tokens.
	 * @param count number of tokens to cash in
	 * @return true if either the bucket is inactive or the given number of
	 * tokens have been cashed in, false otherwise
	 */
	bool take(unsigned int count = 1) noexcept;
private:
	bool active;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_TOKENBUCKET_H_ */
