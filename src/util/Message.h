/*
 * Message.h
 *
 * Message implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGE_H_
#define WH_UTIL_MESSAGE_H_
#include "Packet.h"
#include "../base/common/Source.h"
#include "../base/ds/Pooled.h"
#include "../base/ds/State.h"
#include <cstdarg>

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Status flags: the first four flags are exclusively set in the following
 * order [Wait for Header -> Wait for Data -> Wait for Processing -> Processed].
 */
enum MessageFlag : uint32_t {
	MSG_WAIT_HEADER = 1, /**< Wait for header */
	MSG_WAIT_DATA = 2, /**< Wait for data */
	MSG_WAIT_PROCESSING = 4,/**< Wait for processing */
	MSG_PROCESSED = 8, /**< Processed */
	MSG_PRIORITY = 16, /**< High priority message */
	MSG_TRAP = 32, /**< Requires additional processing */
	MSG_INVALID = 64 /**< Invalid message */
};
//-----------------------------------------------------------------
/**
 * Message implementation
 * Not thread safe
 */
class Message final: public Pooled<Message>, public State, public Packet {
private:
	Message(uint64_t origin) noexcept;
	~Message();

	void* operator new(size_t size) noexcept;
	void operator delete(void *p) noexcept;
public:
	/**
	 * Creates a new message.
	 * @param origin the origin identifier
	 * @return pointer to the newly created message
	 */
	static Message* create(uint64_t origin = 0) noexcept;
	/**
	 * Recycles a message.
	 * @param p pointer a message (can be nullptr)
	 */
	static void recycle(Message *p) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Resets the message: origin, mark, reference-count, and hop-count
	 * are preserved.
	 */
	void clear() noexcept;
	/**
	 * Incrementally builds this message from a given source. The source may
	 * not contain sufficient data temporarily, in such case, call this method
	 * again when the additional data becomes available.
	 * @param in reference to the data source
	 * @return true if this message has been populated, false otherwise
	 */
	bool build(Source<unsigned char> &in);
	//-----------------------------------------------------------------
	/**
	 * Returns label from the routing header.
	 * @return the label
	 */
	uint64_t getLabel() const noexcept;
	/**
	 * Sets a new label into the routing header.
	 * @param label the new label
	 */
	void setLabel(uint64_t label) noexcept;
	/**
	 * Sets a new label into the frame buffer.
	 * @param label the new label
	 */
	void writeLabel(uint64_t label) noexcept;
	/**
	 * Combines setLabel() and writeLabel() in a single call.
	 * @param label the new label
	 */
	void putLabel(uint64_t label) noexcept;

	/**
	 * Returns source identifier from the routing header.
	 * @return the source identifier
	 */
	uint64_t getSource() const noexcept;
	/**
	 * Sets a new source identifier into the routing header.
	 * @param source the new source identifier
	 */
	void setSource(uint64_t source) noexcept;
	/**
	 * Sets a new source identifier into the frame buffer.
	 * @param source the new source identifier
	 */
	void writeSource(uint64_t source) noexcept;
	/**
	 * Combines setSource() and writeSource() in a single call.
	 * @param source the new source identifier
	 */
	void putSource(uint64_t source) noexcept;

	/**
	 * Returns destination identifier from the routing header.
	 * @return the destination identifier
	 */
	uint64_t getDestination() const noexcept;
	/**
	 * Sets a new destination identifier into the routing header.
	 * @param destination the new destination identifier
	 */
	void setDestination(uint64_t destination) noexcept;
	/**
	 * Sets a new destination identifier into the frame buffer.
	 * @param destination the new destination identifier
	 */
	void writeDestination(uint64_t destination) noexcept;
	/**
	 * Combines setDestination() and writeDestination() in a single call.
	 * @param destination the new destination identifier
	 */
	void putDestination(uint64_t destination) noexcept;

	/**
	 * Returns length from the routing header.
	 * @return the length
	 */
	uint16_t getLength() const noexcept;
	/**
	 * Sets a new length into the routing header.
	 * @param length the new length, it's value should be in the range of
	 * [Frame::HEADER_SIZE, Frame::MTU].
	 * @return true on success (valid length), false otherwise
	 */
	bool setLength(uint16_t length) noexcept;
	/**
	 * Sets frame buffer's length (equivalent to Packet::bind(length)).
	 * @param length the new length, this value should be in the range of
	 * [Frame::HEADER_SIZE, Frame::MTU] for this call to succeed.
	 * @return true on success (length is valid), false otherwise
	 */
	bool writeLength(uint16_t length) noexcept;
	/**
	 * Combines setLength() and writeLength() in a single call.
	 * @param length the new length
	 * @return true on success (valid length), false otherwise
	 */
	bool putLength(uint16_t length) noexcept;

	/**
	 * Returns sequence number from the routing header.
	 * @return the sequence number
	 */
	uint16_t getSequenceNumber() const noexcept;
	/**
	 * Sets a new sequence number into the routing header.
	 * @param sequenceNumber the new sequence number
	 */
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	/**
	 * Sets a new sequence number into the frame buffer.
	 * @param sequenceNumber the new sequence number
	 */
	void writeSequenceNumber(uint16_t sequenceNumber) noexcept;
	/**
	 * Combines setSequenceNumber() and writeSequenceNumber() in a single call.
	 * @param sequenceNumber the new sequence number
	 */
	void putSequenceNumber(uint16_t sequenceNumber) noexcept;

	/**
	 * Returns session identifier from the routing header.
	 * @return the session identifier
	 */
	uint8_t getSession() const noexcept;
	/**
	 * Sets a new session identifier into the routing header.
	 * @param session the new session identifier
	 */
	void setSession(uint8_t session) noexcept;
	/**
	 * Sets a new session identifier into the frame buffer.
	 * @param session the new session identifier
	 */
	void writeSession(uint8_t session) noexcept;
	/**
	 * Combines setSession() and writeSession() in a single call.
	 * @param session the new session identifier
	 */
	void putSession(uint8_t session) noexcept;

	/**
	 * Returns command from the routing header.
	 * @return the command
	 */
	uint8_t getCommand() const noexcept;
	/**
	 * Sets a new command into the routing header.
	 * @param command the new command
	 */
	void setCommand(uint8_t command) noexcept;
	/**
	 * Sets a new command into the frame buffer.
	 * @param command the new command
	 */
	void writeCommand(uint8_t command) noexcept;
	/**
	 * Combines setCommand() and writeCommand() in a single call.
	 * @param command the new command
	 */
	void putCommand(uint8_t command) noexcept;

	/**
	 * Returns qualifier from the routing header.
	 * @return the qualifier
	 */
	uint8_t getQualifier() const noexcept;
	/**
	 * Sets a new qualifier into the routing header.
	 * @param qualifier the new qualifier
	 */
	void setQualifier(uint8_t qualifier) noexcept;
	/**
	 * Sets a new qualifier into the frame buffer.
	 * @param qualifier the new qualifier
	 */
	void writeQualifier(uint8_t qualifier) noexcept;
	/**
	 * Combines setQualifier() and writeQualifier() in a single call.
	 * @param qualifier the new qualifier
	 */
	void putQualifier(uint8_t qualifier) noexcept;

	/**
	 * Returns status code from the routing header.
	 * @return the status code
	 */
	uint8_t getStatus() const noexcept;
	/**
	 * Sets a new status code into the routing header.
	 * @param status the new status code
	 */
	void setStatus(uint8_t status) noexcept;
	/**
	 * Sets a new status code into the frame buffer.
	 * @param status the new status code
	 */
	void writeStatus(uint8_t status) noexcept;
	/**
	 * Combines setStatus() and writeStatus() in a single call.
	 * @param status the new status code
	 */
	void putStatus(uint8_t status) noexcept;

	/**
	 * Makes a copy of the routing header.
	 * @param header object for storing the routing header data
	 */
	void getHeader(MessageHeader &header) const noexcept;
	/**
	 * Updates the routing header.
	 * @param header the new header data, the length field should contain value
	 * in the range [Frame::HEADER_SIZE, Frame::MTU] for this call to succeed.
	 * @return true on success (valid length), false otherwise
	 */
	bool setHeader(const MessageHeader &header) noexcept;
	/**
	 * Updates the frame buffer's serialized header.
	 * @param header the new header data, it's length field should have a value
	 * in the range [Frame::HEADER_SIZE, Frame::MTU].
	 * @return true on success (valid length), false otherwise
	 */
	bool writeHeader(const MessageHeader &header) noexcept;
	/**
	 * Combines setHeader() and writeHeader() in a single call.
	 * @param header the new header data
	 * @return true on success (valid length), false otherwise
	 */
	bool putHeader(const MessageHeader &header) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads 64-bit (8 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint64_t)).
	 * @return 8-byte unsigned integer on success, 0 on failure (overflow)
	 */
	uint64_t getData64(unsigned int index) const noexcept;
	/**
	 * Reads 64-bit (8 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint64_t)).
	 * @param data object for storing the 8-bit unsigned integer
	 * @return true on success, 0 on failure (overflow)
	 */
	bool getData64(unsigned int index, uint64_t &data) const noexcept;
	/**
	 * Writes 64-bit (8 byte) unsigned integer into the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint64_t)).
	 * @param data the 8-byte unsigned integer
	 * @return true on success, false on failure (overflow)
	 */
	bool setData64(unsigned int index, uint64_t data) noexcept;
	/**
	 * Appends 64-bit (8 byte) unsigned integer to the payload. On success,
	 * the frame buffer's length and the routing header's length field are
	 * updated. This call will fail if the frame buffer is not valid (see
	 * Packet::validate()), or if the operation will result in an overflow.
	 * @param data the 8-byte unsigned integer
	 * @return true on success, false otherwise
	 */
	bool appendData64(uint64_t data) noexcept;

	/**
	 * Reads 32-bit (4 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint32_t)).
	 * @return 4-byte unsigned integer on success, 0 on failure (overflow)
	 */
	uint32_t getData32(unsigned int index) const noexcept;
	/**
	 * Reads 32-bit (4 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint32_t)).
	 * @param data object for storing the 4-byte unsigned integer
	 * @return true on success, false on failure (overflow)
	 */
	bool getData32(unsigned int index, uint32_t &data) const noexcept;
	/**
	 * Writes 32-bit (4 byte) unsigned integer into the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint32_t)).
	 * @param data the 4-byte unsigned integer
	 * @return true on success, false on failure (overflow)
	 */
	bool setData32(unsigned int index, uint32_t data) noexcept;
	/**
	 * Appends 32-bit (4 byte) unsigned integer to the payload. On success,
	 * the frame buffer's length and the routing header's length field are
	 * updated. This call will fail if the frame buffer is not valid (see
	 * Packet::validate()), or if the operation will result in an overflow.
	 * @param data the 4-byte unsigned integer
	 * @return true on success, false otherwise
	 */
	bool appendData32(uint32_t data) noexcept;

	/**
	 * Reads 16-bit (2 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint16_t)).
	 * @return 2-byte unsigned integer on success, 0 on failure (overflow)
	 */
	uint16_t getData16(unsigned int index) const noexcept;
	/**
	 * Reads 16-bit (2 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint16_t)).
	 * @param data object for storing the 2-byte unsigned integer
	 * @return true on success, false on failure (overflow)
	 */
	bool getData16(unsigned int index, uint16_t &data) const noexcept;
	/**
	 * Writes 16-bit (2 byte) unsigned integer into the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint16_t)).
	 * @param data the 2-byte unsigned integer
	 * @return true on success, false on failure (overflow)
	 */
	bool setData16(unsigned int index, uint16_t data) noexcept;
	/**
	 * Appends 16-bit (2 byte) unsigned integer to the payload. On success,
	 * the frame buffer's length and the routing header's length field are
	 * updated. This call will fail if the frame buffer is not valid (see
	 * Packet::validate()), or if the operation will result in an overflow.
	 * @param data the 2-byte unsigned integer
	 * @return true on success, false otherwise
	 */
	bool appendData16(uint16_t data) noexcept;

	/**
	 * Reads 8-bit (1 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint8_t)).
	 * @return 1-byte unsigned integer on success, 0 on failure (overflow)
	 */
	uint8_t getData8(unsigned int index) const noexcept;
	/**
	 * Reads 8-bit (1 byte) unsigned integer from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint8_t)).
	 * @param data object for storing the 1-byte unsigned integer
	 * @return true on success, false on failure (overflow)
	 */
	bool getData8(unsigned int index, uint8_t &data) const noexcept;
	/**
	 * Writes 8-bit (1 byte) unsigned integer into the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - sizeof(uint8_t)).
	 * @param data the 1-byte unsigned integer
	 * @return true on success, false on failure (overflow)
	 */
	bool setData8(unsigned int index, uint8_t data) noexcept;
	/**
	 * Appends 8-bit (1 byte) unsigned integer to the payload. On success,
	 * the frame buffer's length and the routing header's length field are
	 * updated. This call will fail if the frame buffer is not valid (see
	 * Packet::validate()), or if the operation will result in an overflow.
	 * @param data the 1-byte unsigned integer
	 * @return true on success, false otherwise
	 */
	bool appendData8(uint8_t data) noexcept;

	/**
	 * Reads a double value from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - 8).
	 * @return the double value on success, 0 on failure (overflow)
	 */
	double getDouble(unsigned int index) const noexcept;
	/**
	 * Reads a double value from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - 8).
	 * @param data object for storing the double value
	 * @return true on success, false on failure (overflow)
	 */
	bool getDouble(unsigned int index, double &data) const noexcept;
	/**
	 * Writes a double value into the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - 8).
	 * @param data the double value
	 * @return true on success, false on failure (overflow)
	 */
	bool setDouble(unsigned int index, double data) noexcept;
	/**
	 * Appends a double value to the payload. On success, the frame buffer's
	 * length and the routing header's length field are updated. This call will
	 * fail if the frame buffer is not valid (see Packet::validate()), or if the
	 * operation will result in an overflow.
	 * @param data the double value
	 * @return true on success, false otherwise
	 */
	bool appendDouble(double data) noexcept;

	/**
	 * Reads a sequence of bytes from the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - length).
	 * @param data pointer to the buffer for storing the bytes
	 * @param length number of bytes to read
	 * @return true on success, false on failure (overflow)
	 */
	bool getBytes(unsigned int index, unsigned char *data,
			unsigned int length) const noexcept;
	/**
	 * Returns a constant pointer to the sequence of bytes at the given offset
	 * in the payload.
	 * @param index data's offset inside the payload, this value should be less
	 * than Frame::PAYLOAD_SIZE.
	 * @return constant pointer to the data, nullptr on overflow
	 */
	const unsigned char* getBytes(unsigned int index) const noexcept;
	/**
	 * Writes a sequence of bytes into the payload.
	 * @param index data's offset inside the payload, this value should not
	 * exceed (Frame::PAYLOAD_SIZE - length).
	 * @param data the bytes to write
	 * @param length number of bytes to write
	 * @return true on success, false on failure (overflow)
	 */
	bool setBytes(unsigned int index, const unsigned char *data,
			unsigned int length) noexcept;
	/**
	 * Appends a sequence of bytes to the payload. On success, the frame buffer's
	 * length and the routing header's length field are updated. This call will
	 * fail if the frame buffer is not valid (see Packet::validate()), or if the
	 * operation will result in an overflow.
	 * @param data the bytes to write
	 * @param length number of bytes to write
	 * @return true on success, false on failure (overflow)
	 */
	bool appendBytes(const unsigned char *data, unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes header and payload data into this message.
	 * @param header the header data. It's length field is ignored and the correct
	 * message length is calculated.
	 * @param format the payload format string. If payload is empty then the
	 * format string should either be nullptr or an empty string. Payload data
	 * is passed through the additional arguments following it.
	 * @return true on success, false on failure (payload overflow)
	 */
	bool pack(const MessageHeader &header, const char *format, ...) noexcept;
	/**
	 * Writes header and payload data into this message.
	 * @param header the header data. It's length field is ignored and the correct
	 * message length is calculated.
	 * @param format the payload format string. If payload is empty then the
	 * format string should either be nullptr or an empty string.
	 * @param ap list of additional arguments providing the payload data
	 * @return true on success, false on failure (payload overflow)
	 */
	bool pack(const MessageHeader &header, const char *format,
			va_list ap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes header and payload data into this message.
	 * @param header the header data. It's length field determines the message
	 * length and should be in the range [Frame::HEADER_SIZE, FRAME::MTU].
	 * @param payload the serialized payload data (can be nullptr)
	 * @return true on success, false on error (invalid length)
	 */
	bool pack(const MessageHeader &header,
			const unsigned char *payload) noexcept;
	/**
	 * Stores the serialized message data into this object.
	 * @param message pointer to the serialized data
	 * @return true on success, false on error (invalid data)
	 */
	bool pack(const unsigned char *message) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Appends additional data to this message's payload and updates this
	 * message's length.
	 * @param format the payload data format string. Payload data is passed
	 * through the additional arguments following it.
	 * @return true on success, false otherwise (payload overflow)
	 */
	bool append(const char *format, ...) noexcept;
	/**
	 * Appends additional data to this message's payload and updates this
	 * message's length.
	 * @param format the payload data format
	 * @param ap list of additional arguments providing the payload data
	 * @return true on success, false otherwise (payload overflow)
	 */
	bool append(const char *format, va_list ap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Extracts this message's payload data.
	 * @param format the payload data format string. Payload data is stored
	 * in the locations pointed to by the pointer arguments following it.
	 * @return true on success, false on error (invalid message or format)
	 */
	bool unpack(const char *format, ...) const noexcept;
	/**
	 * Extracts this message's payload data.
	 * @param format the payload data format string
	 * @param ap list of pointer arguments pointing to the output locations
	 * @return true on success, false on error (invalid message or format)
	 */
	bool unpack(const char *format, va_list ap) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks if the given number of messages can be created.
	 * @param count number of messages to create
	 * @return true if the given number of messages can be allocated from the
	 * memory pool, false otherwise.
	 */
	static bool available(unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Increments the reference count by one (1) and returns the new value.
	 * @return the new reference count
	 */
	unsigned int addReferenceCount() noexcept;
	/**
	 * Increments the hop count by one (1) and returns the new value.
	 * @return the new hop count
	 */
	unsigned int addHopCount() noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGE_H_ */
