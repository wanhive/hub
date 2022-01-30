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
#include "../base/ds/MemoryPool.h"
#include "../base/ds/State.h"
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
 * Wanhive message implementation
 * Not thread safe
 */
class Message: public State, public Packet {
private:
	Message(uint64_t origin) noexcept;
	~Message();

	void* operator new(size_t size) noexcept;
	void operator delete(void *p) noexcept;
public:
	//Creates a new Message
	static Message* create(uint64_t origin = 0) noexcept;
	//Recycles a Message (nullptr results in noop)
	static void recycle(Message *p) noexcept;

	//Resets the message (origin, mark, and reference/hop counts are preserved)
	void clear() noexcept;
	//Incrementally builds the message, returns true when done
	bool build(Source<unsigned char> &in);
	//=================================================================
	/**
	 * Message header handling functions
	 ** getX: returns a value from the routing header
	 ** testX: tests whether a value in the routing header is valid
	 ** setX: modifies the routing header
	 ** updateX: modifies the frame buffer (serialized data for IO)
	 ** putX: combines setX and updateX in a single call.
	 *
	 * NOTE: Functions returning boolean value perform sanity test
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
	bool setHeader(const MessageHeader &header) noexcept;
	bool updateHeader(const MessageHeader &header) noexcept;
	bool putHeader(const MessageHeader &header) noexcept;
	//=================================================================
	/**
	 * Payload data handlers
	 *
	 * All the methods perform bounds checking.
	 *
	 ** getDataxx/getDouble/getBytes: returns the payload data at the given
	 * index. Methods returning value of matching type fail silently. Methods
	 * returning boolean return true on success, false otherwise.
	 *
	 ** setDataxx/setDouble/setBytes: inserts data into the payload at the
	 ** given index. Doesn't modify the message length. Returns true on success,
	 * false otherwise.
	 *
	 ** appendDataxx/appendDouble/appendBytes: appends data at the end of
	 * the payload and updates the message length accordingly. Returns true
	 * on success, false otherwise.
	 *
	 */
	uint64_t getData64(unsigned int index) const noexcept;
	bool getData64(unsigned int index, uint64_t &data) const noexcept;
	bool setData64(unsigned int index, uint64_t data) noexcept;
	bool appendData64(uint64_t data) noexcept;

	uint32_t getData32(unsigned int index) const noexcept;
	bool getData32(unsigned int index, uint32_t &data) const noexcept;
	bool setData32(unsigned int index, uint32_t data) noexcept;
	bool appendData32(uint32_t data) noexcept;

	uint16_t getData16(unsigned int index) const noexcept;
	bool getData16(unsigned int index, uint16_t &data) const noexcept;
	bool setData16(unsigned int index, uint16_t data) noexcept;
	bool appendData16(uint16_t data) noexcept;

	uint8_t getData8(unsigned int index) const noexcept;
	bool getData8(unsigned int index, uint8_t &data) const noexcept;
	bool setData8(unsigned int index, uint8_t data) noexcept;
	bool appendData8(uint8_t data) noexcept;

	double getDouble(unsigned int index) const noexcept;
	bool getDouble(unsigned int index, double &data) const noexcept;
	bool setDouble(unsigned int index, double data) noexcept;
	bool appendDouble(double data) noexcept;

	bool getBytes(unsigned int index, unsigned char *data,
			unsigned int length) const noexcept;
	const unsigned char* getBytes(unsigned int index) const noexcept;
	bool setBytes(unsigned int index, const unsigned char *data,
			unsigned int length) noexcept;
	bool appendBytes(const unsigned char *data, unsigned int length) noexcept;
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
	//Returns true if <count> Messages can be allocated from the memory pool
	static bool available(unsigned int count) noexcept;
	//=================================================================
	/**
	 * For efficient IO and memory operations
	 * WARNING: Following methods have potentially dangerous side-effects.
	 * Application developers should not use these methods.
	 */
	//Increases and returns the reference count
	unsigned int addReferenceCount() noexcept;
	//Increases and returns the hop count
	unsigned int addTTL() noexcept;

	//Initializes the memory pool
	static void initPool(unsigned int size);
	//Destroys the memory pool
	static void destroyPool();
	//Returns the memory pool's capacity
	static unsigned int poolSize() noexcept;
	//Returns the number of allocated objects
	static unsigned int allocated() noexcept;
	//Returns the unallocated objects count
	static unsigned int unallocated() noexcept;
private:
	static MemoryPool pool;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGE_H_ */
