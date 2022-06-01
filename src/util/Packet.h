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
#include "PKI.h"

namespace wanhive {
/**
 * The basic data-unit implementation. Provides error-checking routines over the
 * bare-bone implementation.
 */
class Packet: public Frame {
public:
	/**
	 * Constructor: sets this packet's origin.
	 * @param origin the origin identifier
	 */
	Packet(uint64_t origin = 0) noexcept;
	/**
	 * Destructor
	 */
	~Packet();
	//-----------------------------------------------------------------
	/**
	 * Serializes and stores the given header data into the frame buffer. This
	 * function doesn't modify the routing header. This call will fail if the
	 * header's length field has invalid value (see Packet::testLength()).
	 * @param header the header to write into the frame buffer.
	 * @return true on success, false on error (invalid length)
	 */
	bool packHeader(const MessageHeader &header) noexcept;
	/**
	 * Serializes and stores the routing header data into the frame buffer. This
	 * call is equivalent to (equivalent to packHeader(Frame::header())). This
	 * call will fail if the routing header's length field contains an invalid
	 * value (see Packet::testLength()).
	 * @return true on success, false on error (invalid length)
	 */
	bool packHeader() noexcept;
	/**
	 * Unpacks header data from the frame buffer.
	 * @param header object for storing the header data
	 * @return always true
	 */
	bool unpackHeader(MessageHeader &header) const noexcept;
	/**
	 * Unpacks header data from frame buffer into the routing header. This call
	 * is equivalent to Packet::unpackHeader(Frame::header()). This call will
	 * fail if the serialized header data contains invalid value(s).
	 * @return true on success, false on error (invalid data)
	 */
	bool unpackHeader() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Sets the frame buffer's length (doesn't modify the routing header).
	 * @param length frame buffer's length in bytes
	 * @return true on success, false on error (invalid length)
	 */
	bool bind(unsigned int length) noexcept;
	/**
	 * Sets the frame buffer's length using the routing header's data. This call
	 * is equivalent to Packet::bind(Frame::header().getLength()).
	 * @return true on success, false on error (invalid length)
	 */
	bool bind() noexcept;
	/**
	 * Validates this packet. A packet is considered valid if and only if:
	 * [1]: The frame buffer has a valid length (see Packet::bind()).
	 * [2]: Frame buffer's length equals the value in the routing header's
	 * length field.
	 * @return true if the packet is valid, false otherwise.
	 */
	bool validate() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the payload length. This call will fail if the packet is
	 * invalid (see Packet::validate()).
	 * @return payload length on success, 0 on error (invalid packet)
	 */
	unsigned int getPayloadLength() const noexcept;
	/**
	 * Checks whether the routing header's length field has a valid value in the
	 * range [Frame::HEADER_SIZE, Frame::MTU].
	 * @return true if the length is valid, false otherwise
	 */
	bool testLength() const noexcept;
	/**
	 * Checks if the given packet length is valid. The minimum packet length
	 * is Frame::HEADER_SIZE while the maximum packet length is Frame::MTU.
	 * @param length the packet length to check
	 * @return true if the given packet length is valid, false otherwise
	 */
	static bool testLength(unsigned int length) noexcept;
	/**
	 * Calculates the number of packets required to carry the given bytes of
	 * data in payload.
	 * @param bytes data size in bytes
	 * @return the packets count
	 */
	static unsigned int packets(unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Validates the given header's context.
	 * @param header the object to validate
	 * @param command the expected command
	 * @param qualifier the expected qualifier
	 * @return true if header's command and qualifier fields match the given
	 * values, false otherwise
	 */
	static bool checkContext(const MessageHeader &header, uint8_t command,
			uint8_t qualifier) noexcept;
	/**
	 * Validates the routing header's context. This call is equivalent to
	 * checkContext(Frame::header(), command, qualifier).
	 * @param command the expected command
	 * @param qualifier the expected qualifier.
	 * @return true if the routing header's command and qualifier fields match
	 * the given values, false otherwise
	 */
	bool checkContext(uint8_t command, uint8_t qualifier) const noexcept;

	/**
	 * Validates the given header's context.
	 * @param header the object to validate
	 * @param command the expected command
	 * @param qualifier the expected qualifier
	 * @param status the expected status code
	 * @return true if header's command, qualifier and status fields match
	 * the given values, false otherwise
	 */
	static bool checkContext(const MessageHeader &header, uint8_t command,
			uint8_t qualifier, uint8_t status) noexcept;
	/**
	 * Validates the routing header's context. This call is equivalent to
	 * checkContext(Frame::header(), command, qualifier, status).
	 * @param command the expected command
	 * @param qualifier the expected qualifier
	 * @param status the expected status code
	 * @return true if the routing header's command, qualifier and status fields
	 * match the given values, false otherwise
	 */
	bool checkContext(uint8_t command, uint8_t qualifier,
			uint8_t status) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Signs this packet.
	 * @param pki the signing key
	 * @return true on success (or pki is nullptr), false otherwise
	 */
	bool sign(const PKI *pki) noexcept;
	/**
	 * Verifies this packet's signature.
	 * @param pki the verification key
	 * @return true on success (or pki is nullptr), false otherwise
	 */
	bool verify(const PKI *pki) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging purposes, prints the header data to stderr.
	 * @param deep if true then the frame buffer's serialized header will be
	 * printed, otherwise the routing header will printed.
	 */
	void printHeader(bool deep = false) const noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PACKET_H_ */
