/*
 * Handle.h
 *
 * Selective data access handle
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_HANDLE_H_
#define WH_BASE_DS_HANDLE_H_
#include "../common/reflect.h"

namespace wanhive {
/**
 * Generic handle for controlled data access.
 * @tparam F friend class
 * @tparam X data type
 */
template<typename F, typename X = int> class Handle {
public:
	/**
	 * Constructor: assigns a given value.
	 * @param value handle's value
	 */
	Handle(const X &value) noexcept :
			value { value } {

	}
	/**
	 * Destructor
	 */
	~Handle() = default;
private:
	operator X() const noexcept {
		return value;
	}
private:
	WH_POD_ASSERT(X);
	X value;
	friend F;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_HANDLE_H_ */
