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
	//Packs the <header> into the frame buffer (fails on invalid length)
	bool packHeader(const MessageHeader &header) noexcept;
	//Same as the above but uses the routing header
	bool packHeader() noexcept;
	//Unpacks the header data from the frame buffer into the <header>
	bool unpackHeader(MessageHeader &header) const noexcept;
	//Same as the above but uses the routing header
	bool unpackHeader() noexcept;
	//-----------------------------------------------------------------
	//Finalizes the packet length (makes it internally consistent)
	bool bind() noexcept;
	//Returns true if the packet is internally consistent
	bool validate() const noexcept;
	//-----------------------------------------------------------------
	//Returns the payload size in bytes, 0 if the message length is invalid
	unsigned int getPayloadLength() const noexcept;
	//Returns true if the routing header's length field is valid
	bool testLength() const noexcept;
	//Returns true if the given packet <length> is valid
	static bool testLength(unsigned int length) noexcept;
	//Returns the number of packets required to transmit <bytes> of data
	static unsigned int packets(unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Verify message header's context
	 */
	static bool checkContext(const MessageHeader &header, uint8_t command,
			uint8_t qualifier) noexcept;
	//Same as the above, but uses this packet's routing header
	bool checkContext(uint8_t command, uint8_t qualifier) const noexcept;
	static bool checkContext(const MessageHeader &header, uint8_t command,
			uint8_t qualifier, uint8_t status) noexcept;
	//Same as the above, but uses this packet's routing header
	bool checkContext(uint8_t command, uint8_t qualifier,
			uint8_t status) const noexcept;
	//-----------------------------------------------------------------
	/*
	 * For debugging purposes, header data is printed to the stderr.
	 * If <deep> is true then the serialized header from IO buffer is
	 * extracted and printed otherwise the routing header is printed.
	 */
	void printHeader(bool deep = false) const noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PACKET_H_ */
