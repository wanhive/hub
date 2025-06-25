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
 * @tparam X data type
 */
template<typename X> class Source {
public:
	/**
	 * Virtual destructor
	 */
	virtual ~Source() = default;
	/**
	 * Transfers data into a given buffer.
	 * @param buffer output buffer
	 * @param count buffer's capacity
	 * @return number of elements transferred to the buffer
	 */
	virtual size_t emit(X *buffer, size_t count) = 0;
	/**
	 * Returns the maximum number of elements available for transfer.
	 * @return available elements count
	 */
	virtual size_t available() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_SOURCE_H_ */
