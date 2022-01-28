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
#include <cstdint>

namespace wanhive {
//-----------------------------------------------------------------
/*
 * Message's address fields: {source, destination}
 */
struct MessageAddress {
	uint64_t source; //Source identifier
	uint64_t destination; //Destination identifier
};

/*
 * Message's flow control fields: {length, sequence, session}
 */
struct MessageControl {
	uint16_t length; //Length in bytes
	uint16_t sequenceNumber; //Sequence number
	uint8_t session; //Communication channel
};
/*
 * Message's context fields: {command, qualifier, status}
 */
struct MessageContext {
	uint8_t command; //Command
	uint8_t qualifier; //Command's type
	uint8_t status; //Command's status
};
//-----------------------------------------------------------------
/**
 * Fixed length message header
 * Message structure: [{FIXED LENGTH HEADER}{VARIABLE LENGTH PAYLOAD}]
 */
class MessageHeader {
public:
	MessageHeader() noexcept;
	MessageHeader(const MessageAddress &address, const MessageControl &ctrl,
			const MessageContext &ctx, uint64_t label = 0) noexcept;
	MessageHeader(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;
	~MessageHeader();

	//Zero out the internal structure
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Getters and setters
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
	//-----------------------------------------------------------------
	/**
	 * Getters and setters which access/modify multiple fields at once
	 */
	void getAddress(MessageAddress &address) const noexcept;
	void setAddress(const MessageAddress &address) noexcept;

	void getControl(MessageControl &ctrl) const noexcept;
	void setControl(const MessageControl &ctrl) noexcept;

	void getContext(MessageContext &ctx) const noexcept;
	void setContext(const MessageContext ctx) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Getters and setters which access/modify all the fields at once
	 */
	//Load the values into the header
	void load(const MessageAddress &address, const MessageControl &ctrl,
			const MessageContext &ctx, uint64_t label = 0) noexcept;
	//Load the values into the header
	void load(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;

	//Load the values from a valid IO buffer into the header
	unsigned int deserialize(const unsigned char *buffer) noexcept;
	//Store the header data into a valid IO buffer
	unsigned int serialize(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	//For debugging purpose, prints the object to stderr
	void print() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * STATIC METHODS
	 * Get and set header fields from/to a valid IO buffer. Data is written to
	 * and read from correct offset inside the given buffer.
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

	static void getAddress(const unsigned char *buffer,
			MessageAddress &address) noexcept;
	void setAddress(unsigned char *buffer,
			const MessageAddress &address) noexcept;

	static void getControl(const unsigned char *buffer,
			MessageControl &ctrl) noexcept;
	static void setControl(unsigned char *buffer,
			const MessageControl &ctrl) noexcept;

	static void getContext(const unsigned char *buffer,
			MessageContext &ctx) noexcept;
	static void setContext(unsigned char *buffer,
			const MessageContext ctx) noexcept;

	//Store the values into the IO buffer
	static unsigned int serialize(unsigned char *buffer,
			const MessageAddress &address, const MessageControl &ctrl,
			const MessageContext &ctx, uint64_t label = 0) noexcept;
	//Store the values into the IO buffer
	static unsigned int serialize(unsigned char *buffer, uint64_t source,
			uint64_t destination, uint16_t length, uint16_t sequenceNumber,
			uint8_t session, uint8_t command, uint8_t qualifier, uint8_t status,
			uint64_t label = 0) noexcept;
public:
	//Serialized header size in bytes
	static constexpr unsigned int SIZE = 32;
private:
	struct {
		uint64_t label; //Application dependent
		MessageAddress addr; //Address fields
		MessageControl ctrl; //Control fields
		MessageContext ctx; //Context fields
	} header;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEHEADER_H_ */
