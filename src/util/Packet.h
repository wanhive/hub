/**
 * @file Packet.h
 *
 * Message packet
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Data unit enhancement with basic error-checking
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
	 * Writes message header data to the frame buffer. The operation fails if
	 * the length field contains an invalid value.
	 * @param header message header
	 * @return true on success, false on error
	 */
	bool packHeader(const MessageHeader &header) noexcept;
	/**
	 * Writes routing header's data to the frame buffer. The operation fails if
	 * the routing header's length field contains an invalid value.
	 * @return true on success, false on error
	 */
	bool packHeader() noexcept;
	/**
	 * Reads frame buffer's message header data.
	 * @param header stores the header data
	 * @return always true
	 */
	bool unpackHeader(MessageHeader &header) const noexcept;
	/**
	 * Reads message header data from the frame buffer into the routing header.
	 * @return true on success, false on error (invalid data)
	 */
	bool unpackHeader() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Sets frame buffer's length.
	 * @param length length in bytes
	 * @return true on success, false on error (invalid length)
	 */
	bool bind(unsigned int length) noexcept;
	/**
	 * Sets frame buffer's length from the routing header's length field.
	 * @return true on success, false on error (invalid length)
	 */
	bool bind() noexcept;
	/**
	 * Validates this packet. The packet is valid if the frame buffer's length
	 * is valid and it equals the value of the routing header's length field.
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
	 * Validates the routing header's length field's value.
	 * @return true if the length is valid, false otherwise
	 */
	bool testLength() const noexcept;
	/**
	 * Checks whether the given value is a valid packet length.
	 * @param length packet length
	 * @return true if valid, false otherwise
	 */
	static bool testLength(unsigned int length) noexcept;
	/**
	 * Calculates the number of packets needed for a given data size.
	 * @param bytes data size in bytes
	 * @return packets count
	 */
	static unsigned int packets(unsigned int bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Validates a message header's context.
	 * @param header message header
	 * @param command expected command
	 * @param qualifier expected qualifier
	 * @return true on positive match, false otherwise
	 */
	static bool checkContext(const MessageHeader &header, uint8_t command,
			uint8_t qualifier) noexcept;
	/**
	 * Validates the routing header's context.
	 * @param command expected command
	 * @param qualifier expected qualifier.
	 * @return true on positive match, false otherwise
	 */
	bool checkContext(uint8_t command, uint8_t qualifier) const noexcept;

	/**
	 * Validates a message header's context.
	 * @param header message header
	 * @param command expected command
	 * @param qualifier expected qualifier
	 * @param status expected status code
	 * @return true on positive match, false otherwise
	 */
	static bool checkContext(const MessageHeader &header, uint8_t command,
			uint8_t qualifier, uint8_t status) noexcept;
	/**
	 * Validates the routing header's context.
	 * @param command expected command
	 * @param qualifier expected qualifier
	 * @param status expected status code
	 * @return true on positive match, false otherwise
	 */
	bool checkContext(uint8_t command, uint8_t qualifier,
			uint8_t status) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Signs this packet.
	 * @param pki signing key
	 * @return true on success (or the key is nullptr), false otherwise
	 */
	bool sign(PKI *pki) noexcept;
	/**
	 * Verifies this packet's signature.
	 * @param pki verifying key
	 * @return true on success (or the key is nullptr), false otherwise
	 */
	bool verify(PKI *pki) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging: prints the header data to stderr.
	 * @param deep true to print the frame buffer's header data, false to
	 * print the routing header's data.
	 */
	void printHeader(bool deep = false) const noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PACKET_H_ */
