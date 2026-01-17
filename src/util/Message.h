/*
 * Message.h
 *
 * Wanhive Message
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
	MSG_PROBE = 32, /**< Requires additional processing */
	MSG_INVALID = 64 /**< Invalid message */
};
//-----------------------------------------------------------------
/**
 * Message implementation
 * @note Not thread safe
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
	 * @param origin origin's identifier
	 * @return newly created message's pointer
	 */
	static Message* create(uint64_t origin = 0) noexcept;
	/**
	 * Recycles a message.
	 * @param message message's pointer (can be nullptr)
	 * @return true on success, false on failure (reference counted only)
	 */
	static bool recycle(Message *message) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Clears the message: preserves message's origin, mark, reference-count,
	 * and hop-count.
	 */
	void clear() noexcept;
	/**
	 * Incrementally builds this message from a given source. If the source does
	 * not contain sufficient data then call this method again when additional
	 * data becomes available.
	 * @param in source's reference
	 * @return true on completion (message populated), false otherwise
	 */
	bool build(Source<unsigned char> &in);
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's label.
	 * @return label's value
	 */
	uint64_t getLabel() const noexcept;
	/**
	 * Sets routing header's label.
	 * @param label new label
	 */
	void setLabel(uint64_t label) noexcept;
	/**
	 * Sets frame buffer's label.
	 * @param label new label
	 */
	void writeLabel(uint64_t label) noexcept;
	/**
	 * Combines Message::setLabel() and Message::writeLabel().
	 * @param label new label
	 */
	void putLabel(uint64_t label) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's source identifier.
	 * @return source
	 */
	uint64_t getSource() const noexcept;
	/**
	 * Sets routing header's source identifier.
	 * @param source new source
	 */
	void setSource(uint64_t source) noexcept;
	/**
	 * Sets frame buffer's source identifier.
	 * @param source new source
	 */
	void writeSource(uint64_t source) noexcept;
	/**
	 * Combines Message::setSource() and Message::writeSource().
	 * @param source new source
	 */
	void putSource(uint64_t source) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's destination identifier.
	 * @return destination
	 */
	uint64_t getDestination() const noexcept;
	/**
	 * Sets routing header's destination identifier.
	 * @param destination new destination
	 */
	void setDestination(uint64_t destination) noexcept;
	/**
	 * Sets frame buffer's destination identifier.
	 * @param destination new destination
	 */
	void writeDestination(uint64_t destination) noexcept;
	/**
	 * Combines Message::setDestination() and Message::writeDestination().
	 * @param destination new destination
	 */
	void putDestination(uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's length field.
	 * @return length
	 */
	uint16_t getLength() const noexcept;
	/**
	 * Sets routing header's length field.
	 * @param length new length
	 * @return true on success, false on error (invalid length)
	 */
	bool setLength(uint16_t length) noexcept;
	/**
	 * Sets frame buffer's length field (equivalent to Packet::bind(length)).
	 * @param length new length
	 * @return true on success, false on error (invalid length)
	 */
	bool writeLength(uint16_t length) noexcept;
	/**
	 * Combines Message::setLength() and Message::writeLength().
	 * @param length new length
	 * @return true on success, false on error (invalid length)
	 */
	bool putLength(uint16_t length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's sequence number.
	 * @return sequence number
	 */
	uint16_t getSequenceNumber() const noexcept;
	/**
	 * Sets routing header's sequence number.
	 * @param sequenceNumber new sequence number
	 */
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	/**
	 * Sets frame buffer's sequence number.
	 * @param sequenceNumber new sequence number
	 */
	void writeSequenceNumber(uint16_t sequenceNumber) noexcept;
	/**
	 * Combines Message::setSequenceNumber() and Message::writeSequenceNumber().
	 * @param sequenceNumber new sequence number
	 */
	void putSequenceNumber(uint16_t sequenceNumber) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's session identifier.
	 * @return session
	 */
	uint8_t getSession() const noexcept;
	/**
	 * Sets routing header's session identifier.
	 * @param session new session identifier
	 */
	void setSession(uint8_t session) noexcept;
	/**
	 * Sets frame buffer's session identifier.
	 * @param session new session
	 */
	void writeSession(uint8_t session) noexcept;
	/**
	 * Combines Message::setSession() and Message::writeSession().
	 * @param session new session
	 */
	void putSession(uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's command.
	 * @return command
	 */
	uint8_t getCommand() const noexcept;
	/**
	 * Sets routing header's command.
	 * @param command new command
	 */
	void setCommand(uint8_t command) noexcept;
	/**
	 * Sets frame buffer's command.
	 * @param command new command
	 */
	void writeCommand(uint8_t command) noexcept;
	/**
	 * Combines Message::setCommand() and Message::writeCommand().
	 * @param command new command
	 */
	void putCommand(uint8_t command) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's qualifier.
	 * @return qualifier
	 */
	uint8_t getQualifier() const noexcept;
	/**
	 * Sets routing header's qualifier.
	 * @param qualifier new qualifier
	 */
	void setQualifier(uint8_t qualifier) noexcept;
	/**
	 * Sets frame buffer's qualifier.
	 * @param qualifier new qualifier
	 */
	void writeQualifier(uint8_t qualifier) noexcept;
	/**
	 * Combines Message::setQualifier() and Message::writeQualifier().
	 * @param qualifier new qualifier
	 */
	void putQualifier(uint8_t qualifier) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns routing header's status code.
	 * @return status code
	 */
	uint8_t getStatus() const noexcept;
	/**
	 * Sets routing header's status code.
	 * @param status new status code
	 */
	void setStatus(uint8_t status) noexcept;
	/**
	 * Sets frame buffer's status code.
	 * @param status new status code
	 */
	void writeStatus(uint8_t status) noexcept;
	/**
	 * Combines Message::setStatus() and Message::writeStatus().
	 * @param status new status code
	 */
	void putStatus(uint8_t status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Copies the routing header.
	 * @param header stores routing header's data
	 */
	void getHeader(MessageHeader &header) const noexcept;
	/**
	 * Updates the routing header. This call will fail if the length field in
	 * the new header contains invalid value.
	 * @param header new header data
	 * @return true on success, false on error (invalid length)
	 */
	bool setHeader(const MessageHeader &header) noexcept;
	/**
	 * Updates frame buffer's header data. This call will fail if the length
	 * field in the new header contains invalid value.
	 * @param header new header data
	 * @return true on success, false on error (invalid length)
	 */
	bool writeHeader(const MessageHeader &header) noexcept;
	/**
	 * Combines Message::setHeader() and Message::writeHeader().
	 * @param header new header data
	 * @return true on success, false on error (invalid data)
	 */
	bool putHeader(const MessageHeader &header) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads 64-bit (8 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @return 8-byte unsigned integer on success, 0 on error (overflow)
	 */
	uint64_t getData64(unsigned int index) const noexcept;
	/**
	 * Reads 64-bit (8 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @param data stores the 8-bit unsigned integer
	 * @return true on success, 0 on error (overflow)
	 */
	bool getData64(unsigned int index, uint64_t &data) const noexcept;
	/**
	 * Writes 64-bit (8 byte) unsigned integer into the payload.
	 * @param index offset in bytes
	 * @param data 8-byte unsigned integer
	 * @return true on success, false on error (overflow)
	 */
	bool setData64(unsigned int index, uint64_t data) noexcept;
	/**
	 * Appends 64-bit (8 byte) unsigned integer to the payload and updates
	 * message's length.
	 * @param data 8-byte unsigned integer
	 * @return true on success, false on error
	 */
	bool appendData64(uint64_t data) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads 32-bit (4 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @return 4-byte unsigned integer on success, 0 on error (overflow)
	 */
	uint32_t getData32(unsigned int index) const noexcept;
	/**
	 * Reads 32-bit (4 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @param data stores the 4-byte unsigned integer
	 * @return true on success, false on error (overflow)
	 */
	bool getData32(unsigned int index, uint32_t &data) const noexcept;
	/**
	 * Writes 32-bit (4 byte) unsigned integer into the payload.
	 * @param index offset in bytes
	 * @param data 4-byte unsigned integer
	 * @return true on success, false on error (overflow)
	 */
	bool setData32(unsigned int index, uint32_t data) noexcept;
	/**
	 * Appends 32-bit (4 byte) unsigned integer to the payload and updates
	 * message's length.
	 * @param data 4-byte unsigned integer
	 * @return true on success, false on error
	 */
	bool appendData32(uint32_t data) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads 16-bit (2 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @return 2-byte unsigned integer on success, 0 on error (overflow)
	 */
	uint16_t getData16(unsigned int index) const noexcept;
	/**
	 * Reads 16-bit (2 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @param data stores the 2-byte unsigned integer
	 * @return true on success, false on error (overflow)
	 */
	bool getData16(unsigned int index, uint16_t &data) const noexcept;
	/**
	 * Writes 16-bit (2 byte) unsigned integer into the payload.
	 * @param index offset in bytes
	 * @param data 2-byte unsigned integer
	 * @return true on success, false on error (overflow)
	 */
	bool setData16(unsigned int index, uint16_t data) noexcept;
	/**
	 * Appends 16-bit (2 byte) unsigned integer to the payload and updates
	 * message's length.
	 * @param data 2-byte unsigned integer
	 * @return true on success, false on error
	 */
	bool appendData16(uint16_t data) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads 8-bit (1 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @return 1-byte unsigned integer on success, 0 on error (overflow)
	 */
	uint8_t getData8(unsigned int index) const noexcept;
	/**
	 * Reads 8-bit (1 byte) unsigned integer from the payload.
	 * @param index offset in bytes
	 * @param data stores the 1-byte unsigned integer
	 * @return true on success, false on error (overflow)
	 */
	bool getData8(unsigned int index, uint8_t &data) const noexcept;
	/**
	 * Writes 8-bit (1 byte) unsigned integer into the payload.
	 * @param index offset in bytes
	 * @param data 1-byte unsigned integer
	 * @return true on success, false on error (overflow)
	 */
	bool setData8(unsigned int index, uint8_t data) noexcept;
	/**
	 * Appends 8-bit (1 byte) unsigned integer to the payload and updates
	 * message's length.
	 * @param data 1-byte unsigned integer
	 * @return true on success, false on error
	 */
	bool appendData8(uint8_t data) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads a float value from the payload.
	 * @param index offset in bytes
	 * @return float value on success, 0 on error (overflow)
	 */
	float getFloat(unsigned int index) const noexcept;
	/**
	 * Reads a float value from the payload.
	 * @param index offset in bytes
	 * @param data stores the float value
	 * @return true on success, false on error (overflow)
	 */
	bool getFloat(unsigned int index, float &data) const noexcept;
	/**
	 * Writes a float value into the payload.
	 * @param index offset in bytes
	 * @param data float value
	 * @return true on success, false on error (overflow)
	 */
	bool setFloat(unsigned int index, float data) noexcept;
	/**
	 * Appends a float value to the payload and updates message's length.
	 * @param data float value
	 * @return true on success, false on error
	 */
	bool appendFloat(float data) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads a double value from the payload.
	 * @param index offset in bytes
	 * @return double value on success, 0 on error (overflow)
	 */
	double getDouble(unsigned int index) const noexcept;
	/**
	 * Reads a double value from the payload.
	 * @param index offset in bytes
	 * @param data stores the double value
	 * @return true on success, false on error (overflow)
	 */
	bool getDouble(unsigned int index, double &data) const noexcept;
	/**
	 * Writes a double value into the payload.
	 * @param index offset in bytes
	 * @param data double value
	 * @return true on success, false on failure (overflow)
	 */
	bool setDouble(unsigned int index, double data) noexcept;
	/**
	 * Appends a double value to the payload and updates message's length.
	 * @param data double value
	 * @return true on success, false on error
	 */
	bool appendDouble(double data) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads a sequence of bytes from the payload.
	 * @param index offset in bytes
	 * @param data stores the bytes
	 * @param length number of bytes to read
	 * @return true on success, false on error (overflow)
	 */
	bool getBytes(unsigned int index, unsigned char *data,
			unsigned int length) const noexcept;
	/**
	 * Returns a constant pointer to the sequence of bytes at the given offset
	 * in the payload.
	 * @param index offset in bytes
	 * @return data's pointer, nullptr on overflow
	 */
	const unsigned char* getBytes(unsigned int index) const noexcept;
	/**
	 * Writes a sequence of bytes into the payload.
	 * @param index offset in bytes
	 * @param data bytes to write
	 * @param length number of bytes to write
	 * @return true on success, false on error (overflow)
	 */
	bool setBytes(unsigned int index, const unsigned char *data,
			unsigned int length) noexcept;
	/**
	 * Appends a sequence of bytes to the payload and updates message's length.
	 * @param data bytes to write
	 * @param length number of bytes to write
	 * @return true on success, false on error
	 */
	bool appendBytes(const unsigned char *data, unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes header and payload data into this message and updates its length.
	 * @param header message header (the length field is ignored)
	 * @param format payload's format string. In case of empty payload, the
	 * format string should either be nullptr or an empty string. Additional
	 * arguments following it provide the payload data.
	 * @return true on success, false on error (overflow)
	 */
	bool pack(const MessageHeader &header, const char *format, ...) noexcept;
	/**
	 * Writes header and payload data into this message and updates its length.
	 * @param header message header (the length field is ignored)
	 * @param format payload's format string (should either be nullptr or an
	 * empty string if the payload is empty).
	 * @param ap payload data
	 * @return true on success, false on error (overflow)
	 */
	bool pack(const MessageHeader &header, const char *format,
			va_list ap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes header and payload data into this message and updates its length.
	 * @param header message header
	 * @param payload serialized payload data (can be nullptr)
	 * @return true on success, false on error (invalid length)
	 */
	bool pack(const MessageHeader &header,
			const unsigned char *payload) noexcept;
	/**
	 * Writes serialized message data into this message.
	 * @param message serialized data
	 * @return true on success, false on error (invalid data)
	 */
	bool pack(const unsigned char *message) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Appends additional data to the payload and updates message's length.
	 * @param format payload data's format string. Payload data is passed
	 * through the arguments following it.
	 * @return true on success, false on error (invalid data or format)
	 */
	bool append(const char *format, ...) noexcept;
	/**
	 * Appends additional data to the payload and updates message's length.
	 * @param format payload data's format string
	 * @param ap payload data
	 * @return true on success, false on error (invalid data or format)
	 */
	bool append(const char *format, va_list ap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads message's payload data.
	 * @param format payload data's format string. Payload data is stored
	 * at locations provided by the pointer arguments following it.
	 * @return true on success, false on error (invalid message or format)
	 */
	bool unpack(const char *format, ...) const noexcept;
	/**
	 * Reads message's payload data.
	 * @param format payload data's format string
	 * @param ap pointers to the output buffers
	 * @return true on success, false on error (invalid message or format)
	 */
	bool unpack(const char *format, va_list ap) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks if a given number of messages can be allocated.
	 * @param count number of messages to create
	 * @return true if messages can be created, false otherwise
	 */
	static bool available(unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Increments the reference count by one (1) and returns the new value.
	 * @return updated reference count
	 */
	unsigned int link() noexcept;
	/**
	 * Increments the hop count by one (1) and returns the new value.
	 * @return updated hop count
	 */
	unsigned int hop() noexcept;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGE_H_ */
