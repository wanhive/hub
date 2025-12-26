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
 * Data-unit enhancement: adds error-checking to the managed frame.
 */
class Packet: public Frame {
public:
	/**
	 * Default constructor: sets origin to zero(0).
	 */
	Packet() noexcept;
	/**
	 * Constructor: sets packet's origin.
	 * @param origin origin's identifier
	 */
	Packet(uint64_t origin) noexcept;
	/**
	 * Destructor
	 */
	~Packet();
	//-----------------------------------------------------------------
	/**
	 * Serializes and stores header data into the frame buffer (doesn't modify
	 * the routing header). This call will fail if header's length field has an
	 * invalid value (see Packet::testLength()).
	 * @param header message header
	 * @return true on success, false on error (invalid length)
	 */
	bool packHeader(const MessageHeader &header) noexcept;
	/**
	 * Serializes and stores the routing header into the frame buffer. This call
	 * is equivalent to Packet::packHeader(Frame::header()). This call will fail
	 * if routing header's length field is invalid (see Packet::testLength()).
	 * @return true on success, false on error (invalid length)
	 */
	bool packHeader() noexcept;
	/**
	 * Unpacks the frame buffer's header data.
	 * @param header stores the header data
	 * @return always true
	 */
	bool unpackHeader(MessageHeader &header) const noexcept;
	/**
	 * Unpacks frame buffer's header data into the routing header. This call is
	 * similar to Packet::unpackHeader(Frame::header()). This call will fail if
	 * the serialized header data contains invalid value(s).
	 * @return true on success, false on error (invalid data)
	 */
	bool unpackHeader() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Sets frame buffer's length (doesn't modify the routing header).
	 * @param length frame buffer's length in bytes
	 * @return true on success, false on error (invalid length)
	 */
	bool bind(unsigned int length) noexcept;
	/**
	 * Sets frame buffer's length using the routing header's length field. This
	 * call is equivalent to Packet::bind(Frame::header().getLength()).
	 * @return true on success, false on error (invalid length)
	 */
	bool bind() noexcept;
	/**
	 * Validates this packet. A packet is considered valid if and only if:
	 * [1]: Frame buffer has a valid length (see Packet::bind()).
	 * [2]: Frame buffer's length equals the routing header's length field.
	 * @return true if the packet is valid, false otherwise
	 */
	bool validate() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the payload size.
	 * @return payload size in bytes on success, 0 on error (invalid packet)
	 */
	unsigned int getPayloadLength() const noexcept;
	/**
	 * Checks whether the routing header's length field contains a valid value
	 * in the range [Frame::HEADER_SIZE, Frame::MTU].
	 * @return true if the length is valid, false otherwise
	 */
	bool testLength() const noexcept;
	/**
	 * Checks whether the given value is a valid packet length. The minimum
	 * value is Frame::HEADER_SIZE while the maximum value is Frame::MTU.
	 * @param length packet length's value
	 * @return true if the given packet length is valid, false otherwise
	 */
	static bool testLength(unsigned int length) noexcept;
	/**
	 * Calculates the number of packets required for carrying payload data of
	 * the given size.
	 * @param bytes payload's size in bytes
	 * @return packets count
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
	 * @param pki signing key
	 * @return true on success (or key is nullptr), false otherwise
	 */
	bool sign(PKI *pki) noexcept;
	/**
	 * Verifies this packet's signature.
	 * @param pki verifying key
	 * @return true on success (or key is nullptr), false otherwise
	 */
	bool verify(PKI *pki) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging: prints the header data to stderr.
	 * @param deep true to print the frame buffer's serialized header, false to
	 * print the routing header.
	 */
	void printHeader(bool deep = false) const noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PACKET_H_ */
