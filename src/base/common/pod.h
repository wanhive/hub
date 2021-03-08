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

#define WH_IS_POD(X) (std::is_pod<X>::value)
#define WH_POD_ASSERT(X) WH_STATIC_ASSERT(WH_IS_POD(X), #X" should be POD (Plain Old Data) type")

#endif /* WH_BASE_COMMON_POD_H_ */
