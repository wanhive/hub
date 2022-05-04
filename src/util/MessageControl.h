/*
 * MessageControl.h
 *
 * Message flow control implementation
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGECONTROL_H_
#define WH_UTIL_MESSAGECONTROL_H_
#include <cstdint>

namespace wanhive {
/**
 * Message's flow control (length, sequence-number, session)
 */
class MessageControl {
public:
	/**
	 * Default constructor: zeroes (0) out all the fields.
	 */
	MessageControl() noexcept;
	/**
	 * Constructor: initializes length, sequence-number, and session to
	 * the given values.
	 * @param length the length
	 * @param sequenceNumber the sequence number
	 * @param session the session identifier
	 */
	MessageControl(uint16_t length, uint16_t sequenceNumber,
			uint8_t session) noexcept;
	/**
	 * Constructor: uses serialized data for initialization.
	 * @param buffer pointer to the serialized object
	 */
	MessageControl(const unsigned char *buffer) noexcept;
	/**
	 * Destructor
	 */
	~MessageControl();
	//-----------------------------------------------------------------
	/**
	 * Returns the length.
	 * @return the length
	 */
	uint16_t getLength() const noexcept;
	/**
	 * Sets the length.
	 * @param length the new length
	 */
	void setLength(uint16_t length) noexcept;
	/**
	 * Returns the sequence number.
	 * @return the sequence number
	 */
	uint16_t getSequenceNumber() const noexcept;
	/**
	 * Sets the sequence number.
	 * @param sequenceNumber the new sequence number
	 */
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	/**
	 * Returns the session identifier.
	 * @return the session identifier
	 */
	uint8_t getSession() const noexcept;
	/**
	 * Sets the session identifier.
	 * @param session the new session identifier
	 */
	void setSession(uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the length, sequence-number, and session values in a single call.
	 * @param length object for storing the length
	 * @param sequenceNumber object for storing the sequence number
	 * @param session object for storing the session identifier
	 */
	void getControl(uint16_t &length, uint16_t &sequenceNumber,
			uint8_t &session) const noexcept;
	/**
	 * Sets the length, sequence-number, and session values in a single call.
	 * @param length the new length
	 * @param sequenceNumber the new sequence number
	 * @param session the new session identifier
	 */
	void setControl(uint16_t length, uint16_t sequenceNumber,
			uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the serialized data and stores them into this object.
	 * @param buffer pointer to the serialized object
	 */
	void readControl(const unsigned char *buffer) noexcept;
	/**
	 * Serializes this object into the given buffer.
	 * @param buffer pointer to the buffer for storing the result. The
	 * minimum buffer size should be MessageControl::SIZE bytes.
	 */
	void writeControl(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the length from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the length
	 */
	static uint16_t readLength(const unsigned char *buffer) noexcept;
	/**
	 * Writes new length into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param length the new length
	 */
	static void writeLength(unsigned char *buffer, uint16_t length) noexcept;
	/**
	 * Reads the sequence number from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the sequence number
	 */
	static uint16_t readSequenceNumber(const unsigned char *buffer) noexcept;
	/**
	 * Writes new sequence number into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param sequenceNumber the new sequence number
	 */
	static void writeSequenceNumber(unsigned char *buffer,
			uint16_t sequenceNumber) noexcept;
	/**
	 * Reads the session identifier from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the session identifier
	 */
	static uint8_t readSession(const unsigned char *buffer) noexcept;
	/**
	 * Writes new session identifier into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param session the new session identifier
	 */
	static void writeSession(unsigned char *buffer, uint8_t session) noexcept;
public:
	/** Serialized object size in bytes */
	static constexpr unsigned int SIZE = 5;
private:
	uint16_t length; //Length in bytes
	uint16_t sequenceNumber; //Sequence number
	uint8_t session; //Communication channel
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGECONTROL_H_ */
