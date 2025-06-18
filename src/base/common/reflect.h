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
#define WH_IS_VOID(X) (std::is_void_v<X>)
#define WH_IS_POD(X) (std::is_trivial_v<X> && std::is_standard_layout_v<X>)
#define WH_IS_INTEGRAL(X) (std::is_integral_v<X>)
#define WH_IS_POINTER(X) (std::is_pointer_v<X>)
#define WH_IS_ARITHMETIC(X) (std::is_arithmetic_v<X>)
#define WH_IS_FLOATING_POINT(X) (std::is_floating_point_v<X>)

#define WH_POD_ASSERT(X) static_assert(WH_IS_POD(X), #X" should be POD (plain old data) type.")
#define WH_INTEGRAL_ASSERT(X) static_assert(WH_IS_INTEGRAL(X), #X" should be integral type.")
#define WH_POINTER_ASSERT(X) static_assert(WH_IS_POINTER(X), #X" should be pointer type.")
#define WH_ARITHMETIC_ASSERT(X) static_assert(WH_IS_ARITHMETIC(X), #X" should be arithmetic type")
#define WH_FLOATING_POINT_ASSERT(X) static_assert(WH_IS_FLOATING_POINT(X), #X" should be floating point type")
#define WH_ATOMIC_ASSERT(X) static_assert(((WH_IS_INTEGRAL(X) || WH_IS_POINTER(X)) && (sizeof(X) <= 16)), #X" should be integral scalar or pointer type.")
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
