/*
 * Sink.h
 *
 * Base class for sinks
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_SINK_H_
#define WH_BASE_COMMON_SINK_H_
#include <cstddef>

namespace wanhive {
/**
 * Sink interface
 * @tparam X working data type
 */
template<typename X> class Sink {
public:
	/**
	 * Virtual destructor
	 */
	virtual ~Sink() = default;
	/**
	 * Consumes data from the given buffer.
	 * @param buffer pointer to the data buffer
	 * @param count number of elements to consume
	 * @return number of elements actually consumed
	 */
	virtual size_t put(X *buffer, size_t count) = 0;
	/**
	 * Returns the remaining capacity.
	 * @return maximum number of elements which can be consumed
	 */
	virtual size_t space() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_SINK_H_ */
