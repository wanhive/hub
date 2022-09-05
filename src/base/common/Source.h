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
 * Source interface
 * @tparam X working data type
 */
template<typename X> class Source {
public:
	/**
	 * Virtual destructor
	 */
	virtual ~Source() = default;
	/**
	 * Transfers data into the given buffer.
	 * @param buffer pointer to the data buffer
	 * @param count buffer's capacity
	 * @return number of elements transferred to the buffer
	 */
	virtual size_t take(X *buffer, size_t count) = 0;
	/**
	 * Returns the available elements count.
	 * @return maximum number of elements which can be read
	 */
	virtual size_t available() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_SOURCE_H_ */
