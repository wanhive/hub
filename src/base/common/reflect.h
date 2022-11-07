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
#include "defines.h"
#include <cstddef>
#include <type_traits>

//-----------------------------------------------------------------
#define WH_IS_POD(X) (std::is_trivial<X>::value && std::is_standard_layout<X>::value)
#define WH_IS_INTEGRAL(X) (std::is_integral<X>::value)
#define WH_IS_POINTER(X) (std::is_pointer<X>::value)
#define WH_IS_SCALAR(X) (WH_IS_INTEGRAL(X) || WH_IS_POINTER(X))

#define WH_POD_ASSERT(X) static_assert(WH_IS_POD(X), #X" should be POD (plain old data) type.")
#define WH_INTEGRAL_ASSERT(X) static_assert(WH_IS_INTEGRAL(X), #X" should be integral type.")
#define WH_POINTER_ASSERT(X) static_assert(WH_IS_POINTER(X), #X" should be pointer type.")
#define WH_SCALAR_ASSERT(X) static_assert(WH_IS_SCALAR(X), #X" should be integral scalar or pointer type.")
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
