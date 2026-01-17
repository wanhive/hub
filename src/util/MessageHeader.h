/*
 * MessageHeader.h
 *
 * Message header
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
 * Message header implementation
 */
class MessageHeader final: public MessageAddress,
		public MessageControl,
		public MessageContext {
public:
	/**
	 * Default constructor: clears (zeroes out) the header data.
	 */
	MessageHeader() noexcept;
	/**
	 * Constructor: reads the serialized header data.
	 * @param data serialized header
	 */
	MessageHeader(const unsigned char *data) noexcept;
	/**
	 * Destructor
	 */
	~MessageHeader();
	/**
	 * Clears (zeroes out) the header data.
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a reference to the message address.
	 * @return address's reference
	 */
	MessageAddress& address() noexcept;
	/**
	 * Returns a constant reference to the message address.
	 * @return address's reference
	 */
	const MessageAddress& address() const noexcept;
	/**
	 * Returns a reference to the message control.
	 * @return control's reference
	 */
	MessageControl& control() noexcept;
	/**
	 * Returns a constant reference to the message control.
	 * @return control's reference
	 */
	const MessageControl& control() const noexcept;
	/**
	 * Returns a reference to the message context.
	 * @return context's reference
	 */
	MessageContext& context() noexcept;
	/**
	 * Returns a constant reference to the message context.
	 * @return context's reference
	 */
	const MessageContext& context() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns source and destination identifiers.
	 * @param source stores source identifier
	 * @param destination stores destination identifier
	 */
	void getAddress(uint64_t &source, uint64_t &destination) const noexcept;
	/**
	 * Sets new source and destination identifiers.
	 * @param source new source
	 * @param destination new destination
	 */
	void setAddress(uint64_t source, uint64_t destination) noexcept;
	/**
	 * Returns length, sequence-number, and session values.
	 * @param length stores length
	 * @param sequenceNumber stores sequence number
	 * @param session stores session identifier
	 */
	void getControl(uint16_t &length, uint16_t &sequenceNumber,
			uint8_t &session) const noexcept;
	/**
	 * Sets new length, sequence-number, and session values.
	 * @param length new length
	 * @param sequenceNumber new sequence number
	 * @param session new session identifier
	 */
	void setControl(uint16_t length, uint16_t sequenceNumber,
			uint8_t session) noexcept;
	/**
	 * Returns command, qualifier, and status values.
	 * @param command stores command
	 * @param qualifier stores qualifier
	 * @param status stores status code
	 */
	void getContext(uint8_t &command, uint8_t &qualifier,
			uint8_t &status) const noexcept;
	/**
	 * Sets command, qualifier, and status values.
	 * @param command new command
	 * @param qualifier new qualifier
	 * @param status new status code
	 */
	void setContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the serialized header data.
	 * @param data serialized header
	 * @return serialized header's size in bytes
	 */
	unsigned int read(const unsigned char *data) noexcept;
	/**
	 * Serializes the header data.
	 * @param data output buffer
	 * @return serialized header's size in bytes
	 */
	unsigned int write(unsigned char *data) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized header's address.
	 * @param data serialized header
	 */
	void readAddress(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's address.
	 * @param data serialized header
	 */
	void writeAddress(unsigned char *data) const noexcept;
	/**
	 * Reads serialized header's flow control.
	 * @param data serialized header
	 */
	void readControl(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's flow control.
	 * @param data serialized header
	 */
	void writeControl(unsigned char *data) const noexcept;
	/**
	 * Reads serialized header's context.
	 * @param data serialized header
	 */
	void readContext(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's context.
	 * @param data serialized header
	 */
	void writeContext(unsigned char *data) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging: prints the header data to stderr.
	 */
	void print() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized header's label.
	 * @param data serialized header
	 * @return label
	 */
	static uint64_t readLabel(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's label.
	 * @param data serialized header
	 * @param label new label
	 */
	static void writeLabel(unsigned char *data, uint64_t label) noexcept;
	/**
	 * Reads serialized header's source identifier.
	 * @param data serialized header
	 * @return source identifier
	 */
	static uint64_t readSource(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's source identifier.
	 * @param data serialized header
	 * @param source new source
	 */
	static void writeSource(unsigned char *data, uint64_t source) noexcept;
	/**
	 * Reads serialized header's destination identifier.
	 * @param data serialized header
	 * @return destination identifier
	 */
	static uint64_t readDestination(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's destination identifier.
	 * @param data serialized header
	 * @param destination new destination
	 */
	static void writeDestination(unsigned char *data,
			uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized header's length field.
	 * @param data serialized header
	 * @return length
	 */
	static uint16_t readLength(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's length field.
	 * @param data serialized header
	 * @param length new length
	 */
	static void writeLength(unsigned char *data, uint16_t length) noexcept;
	/**
	 * Reads serialized header's sequence number.
	 * @param data serialized header
	 * @return sequence number
	 */
	static uint16_t readSequenceNumber(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's sequence number.
	 * @param data serialized header
	 * @param sequenceNumber new sequence number
	 */
	static void writeSequenceNumber(unsigned char *data,
			uint16_t sequenceNumber) noexcept;
	/**
	 * Reads serialized header's session identifier.
	 * @param data serialized header
	 * @return session
	 */
	static uint8_t readSession(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's session identifier.
	 * @param data serialized header
	 * @param session new session
	 */
	static void writeSession(unsigned char *data, uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized header's command.
	 * @param data serialized header
	 * @return command
	 */
	static uint8_t readCommand(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's command.
	 * @param data serialized header
	 * @param command new command
	 */
	static void writeCommand(unsigned char *data, uint8_t command) noexcept;
	/**
	 * Reads serialized header's qualifier.
	 * @param data serialized header
	 * @return qualifier
	 */
	static uint8_t readQualifier(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's qualifier.
	 * @param data serialized header
	 * @param qualifier new qualifier
	 */
	static void writeQualifier(unsigned char *data, uint8_t qualifier) noexcept;
	/**
	 * Reads serialized header's status code.
	 * @param data serialized header
	 * @return status
	 */
	static uint8_t readStatus(const unsigned char *data) noexcept;
	/**
	 * Updates serialized header's status code.
	 * @param data serialized header
	 * @param status new status
	 */
	static void writeStatus(unsigned char *data, uint8_t status) noexcept;
public:
	/*! Serialized header's size in bytes */
	static constexpr unsigned int SIZE = (MessageAddress::SIZE
			+ MessageControl::SIZE + MessageContext::SIZE);
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEHEADER_H_ */
