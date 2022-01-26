/*
 * Packet.h
 *
 * The message packet
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
 * The message packet
 */
class Packet: public Frame {
public:
	Packet(uint64_t origin = 0) noexcept;
	~Packet();

	//-----------------------------------------------------------------
	//Finalizes the packet length (makes it internally consistent)
	bool bind() noexcept;
	//Unpacks the header data from the frame buffer into the <header>
	bool unpackHeader(MessageHeader &header) const noexcept;
	//Same as the above but uses the routing header
	bool unpackHeader() noexcept;
	//Packs the <header> into the frame buffer (fails on invalid length)
	bool packHeader(const MessageHeader &header) noexcept;
	//Same as the above but uses the routing header
	bool packHeader() noexcept;
	//Returns true if the packet is internally consistent
	bool validate() const noexcept;
	//Returns true is the packet length in the routing header is valid
	bool testLength() const noexcept;
	//-----------------------------------------------------------------
	//Returns true if the given packet <length> is valid
	static bool testLength(unsigned int length) noexcept;
	//Returns the number of packets required to transmit <bytes> of data
	static unsigned int packets(unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/*
	 * For debugging purposes, header is printed to stderr.
	 * If <deep> is true then the serialized header from IO buffer is
	 * extracted and printed otherwise the routing header is printed.
	 */
	void printHeader(bool deep = false) const noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PACKET_H_ */
