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
 * The sink interface
 */
class Sink {
public:
	virtual ~Sink() = default;
	virtual size_t put(const void *buffer, size_t count) = 0;
	virtual size_t space() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_SINK_H_ */
