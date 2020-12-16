/*
 * MessageHeader.h
 *
 * Wanhive's fixed size message header implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGEHEADER_H_
#define WH_UTIL_MESSAGEHEADER_H_
#include <cstdint>
/**
 * Wanhive's fixed size message header
 * Message structure: [{FIXED HEADER}{VARIABLE LENGTH PAYLOAD}]
 */
namespace wanhive {

class MessageHeader {
public:
	MessageHeader() noexcept;
	MessageHeader(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;
	~MessageHeader();

	//Zero out the internal structure
	void clear() noexcept;
	//=================================================================
	/**
	 * Getters and Setters
	 */
	uint64_t getLabel() const noexcept;
	void setLabel(uint64_t label) noexcept;

	uint64_t getSource() const noexcept;
	void setSource(uint64_t source) noexcept;

	uint64_t getDestination() const noexcept;
	void setDestination(uint64_t destination) noexcept;

	uint16_t getLength() const noexcept;
	void setLength(uint16_t length) noexcept;

	uint16_t getSequenceNumber() const noexcept;
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;

	uint8_t getSession() const noexcept;
	void setSession(uint8_t session) noexcept;

	uint8_t getCommand() const noexcept;
	void setCommand(uint8_t command) noexcept;

	uint8_t getQualifier() const noexcept;
	void setQualifier(uint8_t qualifier) noexcept;

	uint8_t getStatus() const noexcept;
	void setStatus(uint8_t status) noexcept;
	//=================================================================
	/**
	 * Get and set header fields, all at once
	 */
	//Load the values into the header
	void load(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;
	//Load the values into the header from the IO buffer
	unsigned int deserialize(const unsigned char *buffer) noexcept;
	//Store the header data into the IO buffer
	unsigned int serialize(unsigned char *buffer) const noexcept;
	//For debugging purpose, prints the object to stderr
	void print() const noexcept;
	//=================================================================
	/**
	 * Get and set header fields from/to a binary IO buffer.
	 * Data is written to and read from correct offset inside the buffer.
	 */
	static uint64_t getLabel(const unsigned char *buffer) noexcept;
	static void setLabel(unsigned char *buffer, uint64_t label) noexcept;

	static uint64_t getSource(const unsigned char *buffer) noexcept;
	static void setSource(unsigned char *buffer, uint64_t source) noexcept;

	static uint64_t getDestination(const unsigned char *buffer) noexcept;
	static void setDestination(unsigned char *buffer,
			uint64_t destination) noexcept;

	static uint16_t getLength(const unsigned char *buffer) noexcept;
	static void setLength(unsigned char *buffer, uint16_t length) noexcept;

	static uint16_t getSequenceNumber(const unsigned char *buffer) noexcept;
	static void setSequenceNumber(unsigned char *buffer,
			uint16_t sequenceNumber) noexcept;

	static uint8_t getSession(const unsigned char *buffer) noexcept;
	static void setSession(unsigned char *buffer, uint8_t session) noexcept;

	static uint8_t getCommand(const unsigned char *buffer) noexcept;
	static void setCommand(unsigned char *buffer, uint8_t command) noexcept;

	static uint8_t getQualifier(const unsigned char *buffer) noexcept;
	static void setQualifier(unsigned char *buffer, uint8_t qualifier) noexcept;

	static uint8_t getStatus(const unsigned char *buffer) noexcept;
	static void setStatus(unsigned char *buffer, uint8_t status) noexcept;

	//Store the values into the buffer
	static unsigned int serialize(unsigned char *buffer, uint64_t source,
			uint64_t destination, uint16_t length, uint16_t sequenceNumber,
			uint8_t session, uint8_t command, uint8_t qualifier, uint8_t status,
			uint64_t label = 0) noexcept;
public:
	//Header size in bytes
	static constexpr unsigned int SIZE = 32;
private:
	struct {
		uint64_t label; //Application dependent
		uint64_t source; //Source of this message
		uint64_t destination; //Destination of this message
		uint16_t length; //Length of this message
		uint16_t sequenceNumber; //Sequence number
		uint8_t session; //Communication channel
		uint8_t command; //Command
		uint8_t qualifier; //Command type
		uint8_t status; //Command status
	} header;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEHEADER_H_ */
