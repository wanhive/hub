/*
 * BufferVector.h
 *
 * Buffer vector
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_BUFFERVECTOR_H_
#define WH_BASE_DS_BUFFERVECTOR_H_
#include <cstddef>

namespace wanhive {
/**
 * Data structure for vectored read/write operations on
 * linear (array-based) buffers.
 * @tparam X storage type
 */
template<typename X> struct BufferVector {
	X *base;
	size_t length;
};

}  // namespace wanhive

#endif /* WH_BASE_DS_BUFFERVECTOR_H_ */
