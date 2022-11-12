/*
 * Handle.h
 *
 * Generic data handle
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
 * Generic handle that selectively provides data's access to a module.
 * @tparam F friend class' name
 * @tparam V data type
 */
template<typename F, typename V = int> class Handle {
public:
	/**
	 * Constructor: assigns a given value.
	 * @param value the value
	 */
	Handle(const V &value) noexcept :
			value { value } {

	}
	/**
	 * Destructor
	 */
	~Handle() = default;
private:
	operator V() const noexcept {
		return value;
	}
private:
	WH_POD_ASSERT(V);
	V value;
	friend F;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_HANDLE_H_ */
