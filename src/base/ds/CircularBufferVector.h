/*
 * CircularBufferVector.h
 *
 * Circular buffer segments
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_CIRCULARBUFFERVECTOR_H_
#define WH_BASE_DS_CIRCULARBUFFERVECTOR_H_
#include <cstddef>
/**
 * Definitions of structures which are required by the array-based
 * circular buffers to perform vectored read/write operations.
 */
namespace wanhive {
/*
 * Can safely cast this to iovec.
 * Particularly useful if <X> is unsigned/signed char.
 */
template<typename X> struct BufferVector {
	X *base;
	size_t length;
};

template<typename X> struct CircularBufferVector {
	BufferVector<X> part[2]; //two segments
};

}  // namespace wanhive

#endif /* WH_BASE_DS_CIRCULARBUFFERVECTOR_H_ */
