/*
 * Source.h
 *
 * Base class for sources
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_SOURCE_H_
#define WH_BASE_COMMON_SOURCE_H_
#include <cstddef>

namespace wanhive {
/**
 * The source interface
 */
template<typename X> class Source {
public:
	/**
	 * Virtual destructor
	 */
	virtual ~Source() = default;
	/**
	 * Transfers data into the given buffer.
	 * @param buffer the buffer for storing data
	 * @param count buffer's capacity
	 * @return number of data units transferred into the buffer
	 */
	virtual size_t take(X *buffer, size_t count) = 0;
	/**
	 * Returns the maximum number of data units which can be read from
	 * this source.
	 * @return the remaining capacity
	 */
	virtual size_t available() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_SOURCE_H_ */
