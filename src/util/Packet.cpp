/*
 * Packet.cpp
 *
 * Message buffer for blocking IO
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Packet.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

Packet::Packet() noexcept: Frame{0} {

}

Packet::~Packet() {

}



} /* namespace wanhive */
