/*
 * reflect.h
 *
 * Data structure inspection
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_REFLECT_H_
#define WH_BASE_COMMON_REFLECT_H_
#include "pod.h"
#include <cstddef>

//-----------------------------------------------------------------
namespace wanhive {

/**
 * Returns an array's size (elements count).
 * @tparam T array's type
 * @tparam N size probe
 * @param array's reference
 * @return number of elements
 */
template<typename T, size_t N> constexpr auto ArraySize(T (&)[N]) {
	return N;
}

/**
 * Returns an enumeration's integral value.
 * @tparam E enumeration's type
 * @tparam sfinae parameter
 * @param e enumeration's value
 * @return integral value
 */
template<typename E, typename = typename std::enable_if<std::is_enum<E>::value,
		typename std::decay<E>::type>::type> constexpr auto EnumValue(E e) {
	return static_cast<typename std::underlying_type<E>::type>(e);
}

}  // namespace wanhive

#endif /* WH_BASE_COMMON_REFLECT_H_ */
