/*
 * Packet.h
 *
 * Message buffer for blocking IO
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_PACKET_H_
#define WH_UTIL_PACKET_H_
#include "Frame.h"

namespace wanhive {
/**
 * Message buffer for blocking IO
 */
class Packet: private Frame {
public:
	Packet() noexcept;
	~Packet();

	using Frame::header;
	using Frame::buffer;
	using Frame::payload;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PACKET_H_ */
