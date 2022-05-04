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
 * The data unit for message implementation.
 */
class Packet: public Frame {
public:
	/**
	 * Constructor: sets this packet's origin.
	 * @param origin the local origin identifier
	 */
	Packet(uint64_t origin = 0) noexcept;

	/**
	 * Destructor
	 */
	~Packet();

	//-----------------------------------------------------------------
	/**
	 * Sets the frame buffer's header. Serializes the given header and stores it
	 * into the frame buffer. The routing header is not modified.
	 * @param header the object to write into the frame buffer. Header's length
	 * field should have a value in the range [Frame::HEADER_SIZE, Frame::MTU].
	 * @return true on success (valid length), false otherwise.
	 */
	bool packHeader(const MessageHeader &header) noexcept;
	/**
	 * Sets the frame buffer's header. Serializes the routing header and stores
	 * it into the frame buffer (equivalent to packHeader(Frame::header())). This
	 * call will fail if routing header's length field contains value outside the
	 * range [Frame::HEADER_SIZE, Frame::MTU].
	 * @return true on success (valid length), false otherwise.
	 */
	bool packHeader() noexcept;
	/**
	 * Unpacks the frame buffer's header data into the given object.
	 * @param header object for storing the data
	 * @return always true
	 */
	bool unpackHeader(MessageHeader &header) const noexcept;
	/**
	 * Unpacks the frame buffer's header data into the routing header (equivalent
	 * to unpackHeader(Frame::header())). This call will fail if as a result of
	 * the operation, the routing header's length field might end up with an
	 * invalid value.
	 * @return true on success (valid length), false otherwise
	 */
	bool unpackHeader() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Sets the frame buffer's length. Updates the length field inside the
	 * frame buffer's serialized header. Doesn't modify the routing header.
	 * @param length the value to write into the frame buffer. This value should
	 * be in the range [Frame::HEADER_SIZE, Frame::MTU].
	 * @return true on success (valid length), false otherwise.
	 */
	bool bind(unsigned int length) noexcept;
	/**
	 * Sets the frame buffer's length. Serializes the value in the routing
	 * header's length field into the frame buffer. This call is equivalent to
	 * bind(Frame::header().getLength()). The length field of the routing header
	 * should contain a value in the range [Frame::HEADER_SIZE, Frame::MTU] for
	 * the call to succeed.
	 *
	 * @return true on success (valid length), false otherwise
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
	 * Calculates the payload length using the routing header's length field. This
	 * call is the same as (Frame::header().getLength() - Frame::HEADER_SIZE).
	 * @return the payload length, zero (0) if the length is not valid
	 */
	unsigned int getPayloadLength() const noexcept;
	/**
	 * Tests the validity of routing header's length field. This value should be
	 * in the range [Frame::HEADER_SIZE, Frame::MTU] for the call to succeed.
	 * @return true if the length is valid, false otherwise
	 */
	bool testLength() const noexcept;
	/**
	 * Tests the given packet length.
	 * @param length the packet length for evaluation
	 * @return true if the length is a valid packet length, false otherwise
	 */
	static bool testLength(unsigned int length) noexcept;
	/**
	 * Returns the number of packets required for carrying the given bytes of
	 * data as payload.
	 * @param bytes the bytes of data to carry by the packets
	 * @return the packet count
	 */
	static unsigned int packets(unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Validates the given header object's context.
	 * @param header the object to validate
	 * @param command the expected command
	 * @param qualifier the expected qualifier
	 * @return true if the header's command and qualifier fields match the given
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
	 * Validates the given header object's context.
	 * @param header the object to validate
	 * @param command the expected command
	 * @param qualifier the expected qualifier
	 * @param status the expected status code
	 * @return true if the header's command, qualifier and status fields match
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
	 * @param pki the object containing the signing key
	 * @return true on success (or pki is nullptr), false otherwise
	 */
	bool sign(const PKI *pki) noexcept;

	/**
	 * Verifies this packet's signature.
	 * @param pki the object containing the verification key
	 * @return true on success (or pki is nullptr), false otherwise
	 */
	bool verify(const PKI *pki) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging purposes, prints the header data to stderr.
	 * @param deep if true then the serialized header from IO buffer is
	 * extracted and printed otherwise the routing header is printed.
	 */
	void printHeader(bool deep = false) const noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PACKET_H_ */
