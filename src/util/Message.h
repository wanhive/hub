/*
 * Message.h
 *
 * Wanhive packet structure implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGE_H_
#define WH_UTIL_MESSAGE_H_
#include "MessageHeader.h"
#include "../base/ds/MemoryPool.h"
#include "../base/ds/State.h"
#include "../base/ds/StaticBuffer.h"
#include <cstdarg>

namespace wanhive {
//-----------------------------------------------------------------
enum MessageFlag {
	/*
	 * Wait for Header -> Wait for Data -> Wait for Processing -> Processed
	 * The first four flags must be set exclusively (clear other flags)
	 */
	MSG_WAIT_HEADER = 1, //Waiting for the header
	MSG_WAIT_DATA = 2, //Waiting for the payload
	MSG_WAIT_PROCESSING = 4, //Waiting for processing
	MSG_PROCESSED = 8, //Message has been processed
	MSG_PRIORITY = 16, //Priority message, cannot be dropped on congestion
	MSG_TRAP = 32, //Requires additional processing before dispatch
	MSG_INVALID = 64 //Invalid message
};
//-----------------------------------------------------------------
/**
 * Wanhive packet structure implementation
 * Packet structure: [{FIXED HEADER}{VARIABLE LENGTH PAYLOAD}]
 * Not thread safe
 */
class Message: public State {
private:
	Message(uint64_t origin) noexcept;
	~Message();
public:
	static void initPool(unsigned int size);
	static void destroyPool();
	static unsigned int poolSize() noexcept;
	static unsigned int allocated() noexcept;
	static unsigned int unallocated() noexcept;

	//Creates a new Message
	static Message* create(uint64_t origin = 0) noexcept;
	//Recycles a Message (nullptr results in noop)
	static void recycle(Message *p) noexcept;

	//Resets the message to it's initial state
	void clear() noexcept;
	//Returns true if the message is internally consistent
	bool validate() const noexcept;
	//Increases and returns the reference count
	unsigned int addReferenceCount() noexcept;
	//Increases and returns the ttl count
	unsigned int addTTL() noexcept;
	//Returns the local source identifier
	uint64_t getOrigin() const noexcept;
	//=================================================================
	/**
	 * For efficient IO operations
	 * Following five methods grant direct access to the internal structures.
	 * These methods violate encapsulation. Use with great care.
	 */
	//Returns direct reference to the routing header
	MessageHeader& getHeader() noexcept;
	//Returns the message buffer offset correctly for data processing
	unsigned char* getStorage() noexcept;
	//Returns the number of bytes waiting for transfer to a data sink
	unsigned int remaining() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For efficient IO operations
	 * Following two methods must be called in the same order while reading
	 * from a data source. Use with great care.
	 */
	//Builds the routing header and readies the object for payload transfer
	void prepareHeader() noexcept;
	//Finalizes the object after completion of data transfer
	void prepareData() noexcept;
	//=================================================================
	/**
	 * Message header handling functions
	 ** getX: returns a value from the routing header
	 ** testX: tests whether a value in the routing header is valid
	 ** setX: modifies the routing header
	 ** updateX: modifies the IO buffer (serialized data)
	 ** putX: combines setX and updateX in a single call.
	 *
	 * NOTE: Functions returning boolean perform sanity test
	 * and return true on success, false otherwise.
	 */
	uint64_t getLabel() const noexcept;
	void setLabel(uint64_t label) noexcept;
	void updateLabel(uint64_t label) noexcept;
	void putLabel(uint64_t label) noexcept;

	uint64_t getSource() const noexcept;
	void setSource(uint64_t source) noexcept;
	void updateSource(uint64_t source) noexcept;
	void putSource(uint64_t source) noexcept;

	uint64_t getDestination() const noexcept;
	void setDestination(uint64_t destination) noexcept;
	void updateDestination(uint64_t destination) noexcept;
	void putDestination(uint64_t destination) noexcept;

	uint16_t getLength() const noexcept;
	uint16_t getPayloadLength() const noexcept;
	bool testLength() const noexcept;
	bool setLength(uint16_t length) noexcept;
	bool updateLength(uint16_t length) noexcept;
	bool putLength(uint16_t length) noexcept;

	uint16_t getSequenceNumber() const noexcept;
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	void updateSequenceNumber(uint16_t sequenceNumber) noexcept;
	void putSequenceNumber(uint16_t sequenceNumber) noexcept;

	uint8_t getSession() const noexcept;
	void setSession(uint8_t session) noexcept;
	void updateSession(uint8_t session) noexcept;
	void putSession(uint8_t session) noexcept;

	uint8_t getCommand() const noexcept;
	void setCommand(uint8_t command) noexcept;
	void updateCommand(uint8_t command) noexcept;
	void putCommand(uint8_t command) noexcept;

	uint8_t getQualifier() const noexcept;
	void setQualifier(uint8_t qualifier) noexcept;
	void updateQualifier(uint8_t qualifier) noexcept;
	void putQualifier(uint8_t qualifier) noexcept;

	uint8_t getStatus() const noexcept;
	void setStatus(uint8_t status) noexcept;
	void updateStatus(uint8_t status) noexcept;
	void putStatus(uint8_t status) noexcept;

	void getHeader(MessageHeader &header) const noexcept;
	bool setHeader(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;
	bool setHeader(const MessageHeader &header) noexcept;
	bool updateHeader(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;
	bool updateHeader(const MessageHeader &header) noexcept;
	bool putHeader(uint64_t source, uint64_t destination, uint16_t length,
			uint16_t sequenceNumber, uint8_t session, uint8_t command,
			uint8_t qualifier, uint8_t status, uint64_t label = 0) noexcept;
	bool putHeader(const MessageHeader &header) noexcept;
	//=================================================================
	/**
	 * Payload handling functions
	 ** getDataxx/getBytes/getDouble: returns the payload data at the given
	 * index. May fail silently if the index violates the MTU.
	 *
	 ** setDataxx/setBytes/setDouble: inserts data into the payload at given
	 * index. Doesn't modify the message length. Returns true on success,
	 * false otherwise.
	 *
	 ** appendDataxx/appendBytes/appendDouble: appends the data at the end of
	 * the payload and modifies the message length accordingly. Returns true
	 * on success, false otherwise.
	 *
	 */
	uint64_t getData64(unsigned int index) const noexcept;
	bool setData64(unsigned int index, uint64_t data) noexcept;
	bool appendData64(uint64_t data) noexcept;

	uint32_t getData32(unsigned int index) const noexcept;
	bool setData32(unsigned int index, uint32_t data) noexcept;
	bool appendData32(uint32_t data) noexcept;

	uint16_t getData16(unsigned int index) const noexcept;
	bool setData16(unsigned int index, uint16_t data) noexcept;
	bool appendData16(uint16_t data) noexcept;

	uint8_t getData8(unsigned int index) const noexcept;
	bool setData8(unsigned int index, uint8_t data) noexcept;
	bool appendData8(uint8_t data) noexcept;

	bool getBytes(unsigned int index, unsigned char *block,
			unsigned int length) const noexcept;
	const unsigned char* getBytes(unsigned int index) const noexcept;
	bool setBytes(unsigned int index, const unsigned char *block,
			unsigned int length) noexcept;
	bool appendBytes(const unsigned char *block, unsigned int length) noexcept;

	double getDouble(unsigned int index) const noexcept;
	bool setDouble(unsigned int index, double data) noexcept;
	bool appendDouble(double data) noexcept;

	//Unpacks the header data from the IO buffer into <header>
	void unpackHeader(MessageHeader &header) const noexcept;
	//=================================================================
	/**
	 * Following two functions pack <header> and data into this Message using
	 * the <format> string. If the payload is empty then the <format> should
	 * be nullptr or empty string. Length field in <header> is ignored and the
	 * correct length is calculated. Return true on success, false on failure.
	 * NOTE: The format string follows the Serializer class.
	 */
	bool pack(const MessageHeader &header, const char *format, ...) noexcept;
	bool pack(const MessageHeader &header, const char *format,
			va_list ap) noexcept;
	//-----------------------------------------------------------------
	//Packs <header> and <payload> into this object, returns true on success
	bool pack(const MessageHeader &header,
			const unsigned char *payload) noexcept;
	//Packs <message> into this object, returns true on success
	bool pack(const unsigned char *message) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following two functions append data at the end of this message and
	 * update the message length. Return true on success, false on failure.
	 */
	bool append(const char *format, ...) noexcept;
	bool append(const char *format, va_list ap) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following two functions extract the payload using the <format>.
	 * Return true on success, false on failure.
	 */
	bool unpack(const char *format, ...) const noexcept;
	bool unpack(const char *format, va_list ap) const noexcept;
	//=================================================================
	/*
	 * For debugging purposes, header is printed to stderr.
	 * If <deep> is true then the serialized header from IO buffer is
	 * extracted and printed otherwise the message header is printed.
	 */
	void printHeader(bool deep = false) const noexcept;
	//=================================================================
	//Returns true if <length> is a valid message length
	static bool testLength(unsigned int length) noexcept;
	//Returns number of messages required to transmit <bytes> of data
	static unsigned int packets(unsigned int bytes) noexcept;
private:
	void* operator new(size_t size) noexcept;
	void operator delete(void *p) noexcept;
public:
	//Message header size in bytes
	static constexpr unsigned int HEADER_SIZE = MessageHeader::SIZE;
	//The maximum message size in bytes
	static constexpr unsigned int MTU = 1024;
	//The maximum payload size in bytes
	static constexpr unsigned int PAYLOAD_SIZE = (MTU - HEADER_SIZE);
private:
	unsigned int referenceCount; //Reference count
	unsigned int ttl; //TTL up-counter

	const uint64_t origin; //The local source
	MessageHeader header; //The routing header
	StaticBuffer<unsigned char, MTU> buffer; //The raw bytes

	static MemoryPool pool;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGE_H_ */
