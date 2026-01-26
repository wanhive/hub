/**
 * @file Sink.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Sink interface
 * @tparam X data type
 */
template<typename X> class Sink {
public:
	/**
	 * Virtual destructor
	 */
	virtual ~Sink() = default;
	/**
	 * Consumes data from a given buffer.
	 * @param src input buffer
	 * @param count number of elements to consume
	 * @return number of elements consumed from the buffer
	 */
	virtual size_t ingest(const X *src, size_t count) = 0;
	/**
	 * Consumes a single element.
	 * @param src value to consume
	 * @return true on success, false on failure
	 */
	virtual bool ingest(const X &src) = 0;
	/**
	 * Returns the remaining consumption capacity.
	 * @return empty slots count
	 */
	virtual size_t space() const noexcept = 0;
	/**
	 * Empties the container and discards the content.
	 * @return true on success, false on failure
	 */
	virtual bool flush() noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_SINK_H_ */
