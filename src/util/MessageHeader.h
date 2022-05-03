/*
 * MessageHeader.h
 *
 * Message header implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGEHEADER_H_
#define WH_UTIL_MESSAGEHEADER_H_
#include "MessageAddress.h"
#include "MessageContext.h"
#include "MessageControl.h"

namespace wanhive {
//-----------------------------------------------------------------

//-----------------------------------------------------------------
/**
 * The message header
 * Message structure: [{FIXED LENGTH HEADER}{VARIABLE LENGTH PAYLOAD}]
 */
class MessageHeader final: public MessageAddress,
		public MessageControl,
		public MessageContext {
public:
	/**
	 * Constructor: zeroes out all the fields
	 */
	MessageHeader() noexcept;
	MessageHeader(const unsigned char *buffer) noexcept;

	/**
	 * Destructor
	 */
	~MessageHeader();

	/**
	 * Zeroes out all all the fields
	 */
	void clear() noexcept;

	/**
	 * Initializes the header data, all at once.
	 *
	 * @param source the source identifier
	 * @param destination the destination identifier
	 * @param length the message length
	 * @param sequenceNumber the sequence number
	 * @param session the session identifier
	 * @param command the command classifier
	 * @param qualifier the command qualifier
	 * @param status the status code
	 * @param label the label (optional)
	 */
	void load(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Deserializes the header data from a buffer into this object.
	 *
	 * @param buffer pointer to memory containing the serialized header
	 * @return serialized header size in bytes
	 */
	unsigned int read(const unsigned char *buffer) noexcept;

	/**
	 * Serializes and stores this object into a buffer.
	 *
	 * @param buffer the buffer for storing the serialized data. The Minimum
	 * buffer size (in bytes) should be MessageHeader::SIZE.
	 * @return serialized header size in bytes
	 */
	unsigned int write(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	void readAddress(const unsigned char *buffer) noexcept;
	void writeAddress(unsigned char *buffer) const noexcept;
	void readControl(const unsigned char *buffer) noexcept;
	void writeControl(unsigned char *buffer) const noexcept;
	void readContext(const unsigned char *buffer) noexcept;
	void writeContext(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging, prints the object to stderr.
	 */
	void print() const noexcept;
	//-----------------------------------------------------------------
	static uint64_t readLabel(const unsigned char *buffer) noexcept;
	static void writeLabel(unsigned char *buffer, uint64_t label) noexcept;
	static uint64_t readSource(const unsigned char *buffer) noexcept;
	static void writeSource(unsigned char *buffer, uint64_t source) noexcept;
	static uint64_t readDestination(const unsigned char *buffer) noexcept;
	static void writeDestination(unsigned char *buffer,
			uint64_t destination) noexcept;

	static uint16_t readLength(const unsigned char *buffer) noexcept;
	static void writeLength(unsigned char *buffer, uint16_t length) noexcept;
	static uint16_t readSequenceNumber(const unsigned char *buffer) noexcept;
	static void writeSequenceNumber(unsigned char *buffer,
			uint16_t sequenceNumber) noexcept;
	static uint8_t readSession(const unsigned char *buffer) noexcept;
	static void writeSession(unsigned char *buffer, uint8_t session) noexcept;

	static uint8_t readCommand(const unsigned char *buffer) noexcept;
	static void writeCommand(unsigned char *buffer, uint8_t command) noexcept;
	static uint8_t readQualifier(const unsigned char *buffer) noexcept;
	static void writeQualifier(unsigned char *buffer,
			uint8_t qualifier) noexcept;
	static uint8_t readStatus(const unsigned char *buffer) noexcept;
	static void writeStatus(unsigned char *buffer, uint8_t status) noexcept;
public:
	/** Serialized header size in bytes */
	static constexpr unsigned int SIZE = MessageAddress::SIZE
			+ MessageControl::SIZE + MessageContext::SIZE;
private:
	//Offset to serialized address data
	static constexpr unsigned int OFF_ADDR = 0;
	//Offset to serialized flow control data
	static constexpr unsigned int OFF_CTRL = MessageAddress::SIZE;
	//Offset to serialized context data
	static constexpr unsigned int OFF_CTX = OFF_CTRL + MessageControl::SIZE;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEHEADER_H_ */
