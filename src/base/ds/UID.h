/*
 * UID.h
 *
 * Unique identifier
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_UID_H_
#define WH_BASE_DS_UID_H_
#include <cstdint>

namespace wanhive {
/**
 * Thread-safe unique identifier generator
 * The 64-bit counter used for generating the next identifier will eventually
 * roll over. However, such an event will take from several decades to centuries
 * to occur under normal circumstances. For e.g, @10 billion new uid per second,
 * the counter will take more than 200 years to roll over.
 */
class UID {
public:
	/**
	 * Default constructor: creates a new unique identifier.
	 */
	UID() noexcept;
	/**
	 * Destructor
	 */
	~UID();
	/**
	 * Sets a new numerical identifier which may or may not be unique (overwrites
	 * the automatically generated identifier which is guaranteed to be unique).
	 * @param id the new identifier
	 */
	void set(unsigned long long id) noexcept;
	/**
	 * Returns the 64-bit numerical identifier.
	 * @return 64-bit identifier value
	 */
	unsigned long long get() const noexcept;
private:
	//Generates the next value
	static unsigned long long next() noexcept;
public:
	/** Minimum value of automatically generated identifier */
	static constexpr unsigned long long MIN = ((unsigned long long) INT64_MAX)
			+ 1;
	/** Maximum value of automatically generated identifier */
	static constexpr unsigned long long MAX = UINT64_MAX;
private:
	unsigned long long uid;
	static unsigned long long _next;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_UID_H_ */
