/**
 * @file CircularBufferVector.h
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
#include "BufferVector.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Data structure for vectored read/write operations on the circular buffer
 * segments.
 * @tparam X storage type
 */
template<typename X> struct CircularBufferVector {
	/*! Two segments */
	BufferVector<X> part[2];
};

}  // namespace wanhive

#endif /* WH_BASE_DS_CIRCULARBUFFERVECTOR_H_ */
