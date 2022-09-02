/*
 * pod.h
 *
 * Resolution of POD types in C++
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_POD_H_
#define WH_BASE_COMMON_POD_H_
#include "defines.h"
#include <type_traits>

#define WH_IS_POD(X) (std::is_trivial<X>::value && std::is_standard_layout<X>::value)
#define WH_POD_ASSERT(X) WH_STATIC_ASSERT(WH_IS_POD(X), #X" should be POD (Plain Old Data) type")

#define WH_IS_INTEGRAL(X) (std::is_integral<X>::value)
#define WH_IS_POINTER(X) (std::is_pointer<X>::value)
#define WH_IS_SCALAR(X) (WH_IS_INTEGRAL(X) || WH_IS_POINTER(X))

#define WH_INTEGRAL_ASSERT(X) WH_STATIC_ASSERT(WH_IS_INTEGRAL(X), #X" should be integral type")
#define WH_POINTER_ASSERT(X) WH_STATIC_ASSERT(WH_IS_POINTER(X), #X" should be pointer type")
#define WH_SCALAR_ASSERT(X) WH_STATIC_ASSERT(WH_IS_SCALAR(X), #X" should be scalar (integral/pointer) type")
#endif /* WH_BASE_COMMON_POD_H_ */
