/*
 * Duplex.h
 *
 * Base class for bidirectional channels
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_DUPLEX_H_
#define WH_BASE_COMMON_DUPLEX_H_
#include "Sink.h"
#include "Source.h"

namespace wanhive {
/**
 * Two-way channel interface
 * @tparam X data type
 */
template<typename X> class Duplex: public Sink<X>, public Source<X> {

};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_DUPLEX_H_ */
