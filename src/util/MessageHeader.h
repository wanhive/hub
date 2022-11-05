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
/**
 * The message header
 */
class MessageHeader final: public MessageAddress,
		public MessageControl,
		public MessageContext {
public:
	/**
	 * Default constructor: zeroes out the data.
	 */
	MessageHeader() noexcept;
	/**
	 * Constructor: uses serialized data for initialization.
	 * @param buffer pointer to the serialized header data
	 */
	MessageHeader(const unsigned char *buffer) noexcept;
	/**
	 * Destructor
	 */
	~MessageHeader();
	/**
	 * Zeroes out the header data
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Deserializes the header data and stores them into this object.
	 * @param buffer pointer to the serialized header data
	 * @return serialized header size in bytes
	 */
	unsigned int read(const unsigned char *buffer) noexcept;
	/**
	 * Serializes this object.
	 * @param buffer pointer to a buffer for storing the serialized header. The
	 * minimum buffer size (in bytes) should be MessageHeader::SIZE.
	 * @return serialized header size in bytes
	 */
	unsigned int write(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the serialized address (label, source, destination) data and stores
	 * them into this object.
	 * @param buffer pointer to the serialized header data
	 */
	void readAddress(const unsigned char *buffer) noexcept;
	/**
	 * Serializes this object's address (label, source, destination) data.
	 * @param buffer pointer to the serialized header data
	 */
	void writeAddress(unsigned char *buffer) const noexcept;
	/**
	 * Reads the serialized flow control (length, sequence-number, session) data
	 * and stores them into this object.
	 * @param buffer pointer to the serialized header data
	 */
	void readControl(const unsigned char *buffer) noexcept;
	/**
	 * Serializes this object's flow control (length, sequence-number, session)
	 * data.
	 * @param buffer pointer to the serialized header data
	 */
	void writeControl(unsigned char *buffer) const noexcept;
	/**
	 * Reads the serialized context (command, qualifier, status) data and stores
	 * them into this object.
	 * @param buffer pointer to the serialized header data
	 */
	void readContext(const unsigned char *buffer) noexcept;
	/**
	 * Serializes this object's context (command, qualifier, status) data.
	 * @param buffer pointer to the serialized header data
	 */
	void writeContext(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging, prints the object to stderr.
	 */
	void print() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the label from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the label
	 */
	static uint64_t readLabel(const unsigned char *buffer) noexcept;
	/**
	 * Writes new label into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param label the new label
	 */
	static void writeLabel(unsigned char *buffer, uint64_t label) noexcept;
	/**
	 * Reads the source identifier from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the source identifier
	 */
	static uint64_t readSource(const unsigned char *buffer) noexcept;
	/**
	 * Writes new source identifier into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param source the new source identifier
	 */
	static void writeSource(unsigned char *buffer, uint64_t source) noexcept;
	/**
	 * Reads the destination identifier from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the destination identifier
	 */
	static uint64_t readDestination(const unsigned char *buffer) noexcept;
	/**
	 * Writes new destination identifier into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param destination the new destination identifier
	 */
	static void writeDestination(unsigned char *buffer,
			uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the length from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the length
	 */
	static uint16_t readLength(const unsigned char *buffer) noexcept;
	/**
	 * Writes new length into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param length the new length
	 */
	static void writeLength(unsigned char *buffer, uint16_t length) noexcept;
	/**
	 * Reads the sequence number from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the sequence number
	 */
	static uint16_t readSequenceNumber(const unsigned char *buffer) noexcept;
	/**
	 * Writes new sequence number into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param sequenceNumber the new sequence number
	 */
	static void writeSequenceNumber(unsigned char *buffer,
			uint16_t sequenceNumber) noexcept;
	/**
	 * Reads the session identifier from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the session identifier
	 */
	static uint8_t readSession(const unsigned char *buffer) noexcept;
	/**
	 * Writes new session identifier into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param session the new session identifier
	 */
	static void writeSession(unsigned char *buffer, uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the command from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the command
	 */
	static uint8_t readCommand(const unsigned char *buffer) noexcept;
	/**
	 * Writes new command into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param command the new command
	 */
	static void writeCommand(unsigned char *buffer, uint8_t command) noexcept;
	/**
	 * Reads the qualifier from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the qualifier
	 */
	static uint8_t readQualifier(const unsigned char *buffer) noexcept;
	/**
	 * Writes new qualifier into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param qualifier the new qualifier
	 */
	static void writeQualifier(unsigned char *buffer,
			uint8_t qualifier) noexcept;
	/**
	 * Reads the status code from serialized header.
	 * @param buffer pointer to the serialized header data
	 * @return the status code
	 */
	static uint8_t readStatus(const unsigned char *buffer) noexcept;
	/**
	 * Writes new status code into serialized header.
	 * @param buffer pointer to the serialized header data
	 * @param status the new status code
	 */
	static void writeStatus(unsigned char *buffer, uint8_t status) noexcept;
public:
	/** Serialized header data size in bytes */
	static constexpr unsigned int SIZE = MessageAddress::SIZE
			+ MessageControl::SIZE + MessageContext::SIZE;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEHEADER_H_ */
