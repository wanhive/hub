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
/**
 * The address structure: {source, destination}
 */
struct MessageAddress {
	uint64_t source; //Source identifier
	uint64_t destination; //Destination identifier
};

/**
 * The flow control structure: {length, sequence, session}
 */
struct MessageControl {
	uint16_t length; //Length in bytes
	uint16_t sequenceNumber; //Sequence number
	uint8_t session; //Communication channel
};

/**
 * The context structure: {command, qualifier, status}
 */
struct MessageContext {
	uint8_t command; //Command
	uint8_t qualifier; //Command's type
	uint8_t status; //Command's status
};
//-----------------------------------------------------------------
/**
 * The message header
 * Message structure: [{FIXED LENGTH HEADER}{VARIABLE LENGTH PAYLOAD}]
 */
class MessageHeader {
public:
	/**
	 * Constructor: zeroes out all the fields
	 */
	MessageHeader() noexcept;

	/**
	 * Constructor
	 *
	 * @param address the address data
	 * @param ctrl the flow control data
	 * @param ctx the context data
	 * @param label the label (optional)
	 */
	MessageHeader(const MessageAddress &address, const MessageControl &ctrl,
			const MessageContext &ctx, uint64_t label = 0) noexcept;
	/**
	 * Constructor
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
	MessageHeader(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;

	/**
	 * Destructor
	 */
	~MessageHeader();

	/**
	 * Zeroes out all all the fields
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the label
	 *
	 * @return the label
	 */
	uint64_t getLabel() const noexcept;

	/**
	 * Sets the label
	 *
	 * @param label the new label
	 */
	void setLabel(uint64_t label) noexcept;

	/**
	 * Returns the source identifier
	 *
	 * @return the source identifier
	 */
	uint64_t getSource() const noexcept;

	/**
	 * Sets the source identifier
	 *
	 * @param source the new source identifier
	 */
	void setSource(uint64_t source) noexcept;

	/**
	 * Returns the destination identifier
	 *
	 * @return the destination identifier
	 */
	uint64_t getDestination() const noexcept;

	/**
	 * Sets the destination identifier
	 *
	 * @param destination the new destination identifier
	 */
	void setDestination(uint64_t destination) noexcept;

	/**
	 * Returns the message length in bytes
	 *
	 * @return the message length
	 */
	uint16_t getLength() const noexcept;

	/**
	 * Sets the message length in bytes
	 *
	 * @param length the new message length
	 */
	void setLength(uint16_t length) noexcept;

	/**
	 * Returns the sequence number
	 *
	 * @return the sequence number
	 */
	uint16_t getSequenceNumber() const noexcept;

	/**
	 * Sets the sequence number
	 *
	 * @param sequenceNumber the new sequence number
	 */
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;

	/**
	 * Returns the session identifier
	 *
	 * @return the session identifier
	 */
	uint8_t getSession() const noexcept;

	/**
	 * Sets the session identifier
	 *
	 * @param session the new session identifier
	 */
	void setSession(uint8_t session) noexcept;

	/**
	 * Returns the command classifier
	 *
	 * @return the command classifier
	 */
	uint8_t getCommand() const noexcept;

	/**
	 * Sets the new command classifier
	 *
	 * @param command the new command classifier
	 */
	void setCommand(uint8_t command) noexcept;

	/**
	 * Returns the command qualifier
	 *
	 * @return the command qualifier
	 */
	uint8_t getQualifier() const noexcept;

	/**
	 * Sets the command qualifier
	 *
	 * @param qualifier the new command qualifier
	 */
	void setQualifier(uint8_t qualifier) noexcept;

	/**
	 * Returns the status code
	 *
	 * @return the status code
	 */
	uint8_t getStatus() const noexcept;

	/**
	 * Sets the status code
	 *
	 * @param status the new status code
	 */
	void setStatus(uint8_t status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the address data (source and destination).
	 *
	 * @param address object for storing the address data
	 */
	void getAddress(MessageAddress &address) const noexcept;

	/**
	 * Sets the address data (source and destination).
	 *
	 * @param address object containing the address data
	 */
	void setAddress(const MessageAddress &address) noexcept;

	/**
	 * Returns the flow control data (length, sequence-number, session).
	 *
	 * @param ctrl object for storing the flow control data
	 */
	void getControl(MessageControl &ctrl) const noexcept;

	/**
	 * Sets the flow control data (length, sequence-number, session).
	 *
	 * @param ctrl object containing the flow control data
	 */
	void setControl(const MessageControl &ctrl) noexcept;

	/**
	 * Returns the context data (command, qualifier, status).
	 *
	 * @param ctx object for storing the context data
	 */
	void getContext(MessageContext &ctx) const noexcept;

	/**
	 * Sets the context data (command, qualifier, status).
	 *
	 * @param ctx object containing the context data
	 */
	void setContext(const MessageContext ctx) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Initializes the header data, all at once.
	 *
	 * @param address the address data
	 * @param ctrl the flow control data
	 * @param ctx the context data
	 * @param label the label (optional)
	 */
	void load(const MessageAddress &address, const MessageControl &ctrl,
			const MessageContext &ctx, uint64_t label = 0) noexcept;

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

	/**
	 * Deserializes the header data from a buffer into this object.
	 *
	 * @param buffer pointer to memory containing the serialized header
	 * @return serialized header size in bytes
	 */
	unsigned int deserialize(const unsigned char *buffer) noexcept;

	/**
	 * Serializes and stores this object into a buffer.
	 *
	 * @param buffer the buffer for storing the serialized data. The Minimum
	 * buffer size (in bytes) should be MessageHeader::SIZE.
	 * @return serialized header size in bytes
	 */
	unsigned int serialize(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging, prints the object to stderr.
	 */
	void print() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the label from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the label
	 */
	static uint64_t getLabel(const unsigned char *buffer) noexcept;

	/**
	 * Sets the label of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param label the new label
	 */
	static void setLabel(unsigned char *buffer, uint64_t label) noexcept;

	/**
	 * Returns the source identifier from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the source identifier
	 */
	static uint64_t getSource(const unsigned char *buffer) noexcept;

	/**
	 * Sets the source identifier of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param source the new source identifier
	 */
	static void setSource(unsigned char *buffer, uint64_t source) noexcept;

	/**
	 * Returns the destination identifier from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the destination identifier
	 */
	static uint64_t getDestination(const unsigned char *buffer) noexcept;

	/**
	 * Sets the destination identifier of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param destination the new destination identifier
	 */
	static void setDestination(unsigned char *buffer,
			uint64_t destination) noexcept;

	/**
	 * Returns the message length from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the message length
	 */
	static uint16_t getLength(const unsigned char *buffer) noexcept;

	/**
	 * Sets the message length of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param length the new message length
	 */
	static void setLength(unsigned char *buffer, uint16_t length) noexcept;

	/**
	 * Returns the sequence number from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the sequence number
	 */
	static uint16_t getSequenceNumber(const unsigned char *buffer) noexcept;

	/**
	 * Sets the sequence number of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param sequenceNumber the new sequence number
	 */
	static void setSequenceNumber(unsigned char *buffer,
			uint16_t sequenceNumber) noexcept;

	/**
	 * Returns the session identifier from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the session identifier
	 */
	static uint8_t getSession(const unsigned char *buffer) noexcept;

	/**
	 * Sets the session identifier of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param session the new session identifier
	 */
	static void setSession(unsigned char *buffer, uint8_t session) noexcept;

	/**
	 * Returns the command classifier from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the command classifier
	 */
	static uint8_t getCommand(const unsigned char *buffer) noexcept;

	/**
	 * Sets the command classifier of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param command the new command classifier
	 */
	static void setCommand(unsigned char *buffer, uint8_t command) noexcept;

	/**
	 * Returns the command qualifier from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the command qualifier
	 */
	static uint8_t getQualifier(const unsigned char *buffer) noexcept;

	/**
	 * Sets the command qualifier of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param qualifier the new command qualifier
	 */
	static void setQualifier(unsigned char *buffer, uint8_t qualifier) noexcept;

	/**
	 * Returns the status code from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @return the status code
	 */
	static uint8_t getStatus(const unsigned char *buffer) noexcept;

	/**
	 * Sets the status code of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param status the new status code
	 */
	static void setStatus(unsigned char *buffer, uint8_t status) noexcept;

	/**
	 * Returns the address data from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param address object for storing the address data
	 */
	static void getAddress(const unsigned char *buffer,
			MessageAddress &address) noexcept;

	/**
	 * Sets the address data of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param address the new address data
	 */
	void setAddress(unsigned char *buffer,
			const MessageAddress &address) noexcept;

	/**
	 * Returns the flow control data from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param ctrl object for storing the flow control data
	 */
	static void getControl(const unsigned char *buffer,
			MessageControl &ctrl) noexcept;

	/**
	 * Sets the flow control data of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param ctrl the new flow control data
	 */
	static void setControl(unsigned char *buffer,
			const MessageControl &ctrl) noexcept;

	/**
	 * Returns the context data from serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param ctx the context data
	 */
	static void getContext(const unsigned char *buffer,
			MessageContext &ctx) noexcept;

	/**
	 * Sets the flow control data of serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param ctx the new context data
	 */
	static void setContext(unsigned char *buffer,
			const MessageContext ctx) noexcept;

	/**
	 * Updates the serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param address the address data
	 * @param ctrl the flow control data
	 * @param ctx the context data
	 * @param label the label (optional)
	 * @return serialized header size in bytes
	 */
	static unsigned int serialize(unsigned char *buffer,
			const MessageAddress &address, const MessageControl &ctrl,
			const MessageContext &ctx, uint64_t label = 0) noexcept;

	/**
	 * Updates the serialized header.
	 *
	 * @param buffer pointer to the serialized header buffer
	 * @param source the source identifier
	 * @param destination the destination identifier
	 * @param length the message length
	 * @param sequenceNumber the sequence number
	 * @param session the session identifier
	 * @param command the command classifier
	 * @param qualifier the command qualifier
	 * @param status the status code
	 * @param label the label (optional)
	 * @return serialized header size in bytes
	 */
	static unsigned int serialize(unsigned char *buffer, uint64_t source,
			uint64_t destination, uint16_t length, uint16_t sequenceNumber,
			uint8_t session, uint8_t command, uint8_t qualifier, uint8_t status,
			uint64_t label = 0) noexcept;
public:
	/** Serialized header size in bytes */
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
