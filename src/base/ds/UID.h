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
 * Unique identifier
 *
 * The 64-bit counter used for generating the next identifier will eventually
 * roll over. However, such an event will take several decades or centuries to
 * occur under normal conditions. For e.g, @10 billion new uid per second, the
 * counter will take more than 200 years to roll over.
 */
class UID {
public:
	UID() noexcept;
	~UID();

	//Sets the identifier (overwrites the automatically generated uid)
	void set(unsigned long long id) noexcept;
	//Returns the identifier
	unsigned long long get() const noexcept;
private:
	//Generates the next uid
	static unsigned long long next() noexcept;
public:
	//[9223372036854775808, 18446744073709551615] are used by the generator
	static constexpr unsigned long long MIN = ((unsigned long long) INT64_MAX)
			+ 1;
	static constexpr unsigned long long MAX = UINT64_MAX;
private:
	unsigned long long uid;
	static unsigned long long _next;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_UID_H_ */
