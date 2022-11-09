/*
 * MessageControl.h
 *
 * Message's flow control
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
 * Message flow control implementation
 */
class MessageControl {
public:
	/**
	 * Default constructor: zero-initializes the data members.
	 */
	MessageControl() noexcept;
	/**
	 * Constructor: initializes data members with the given values.
	 * @param length length's value
	 * @param sequenceNumber sequence number's value
	 * @param session session identifier's value
	 */
	MessageControl(uint16_t length, uint16_t sequenceNumber,
			uint8_t session) noexcept;
	/**
	 * Constructor: extracts values from a serialized object.
	 * @param data serialized object
	 */
	MessageControl(const unsigned char *data) noexcept;
	/**
	 * Destructor
	 */
	~MessageControl();
	//-----------------------------------------------------------------
	/**
	 * Returns the length.
	 * @return length
	 */
	uint16_t getLength() const noexcept;
	/**
	 * Sets a new length.
	 * @param length new length
	 */
	void setLength(uint16_t length) noexcept;
	/**
	 * Returns the sequence number.
	 * @return sequence number
	 */
	uint16_t getSequenceNumber() const noexcept;
	/**
	 * Sets a new sequence number.
	 * @param sequenceNumber new sequence number
	 */
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	/**
	 * Returns the session identifier.
	 * @return session identifier
	 */
	uint8_t getSession() const noexcept;
	/**
	 * Sets a new session identifier.
	 * @param session new session identifier
	 */
	void setSession(uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns length, sequence-number, and session values.
	 * @param length stores length's value
	 * @param sequenceNumber stores sequence number's value
	 * @param session stores session identifier's value
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
	//-----------------------------------------------------------------
	/**
	 * Extracts values from a serialized object.
	 * @param data serialized object
	 */
	void readControl(const unsigned char *data) noexcept;
	/**
	 * Translates control data into a serialized object.
	 * @param data output buffer (MessageControl::SIZE is the minimum required
	 * buffer size in bytes).
	 */
	void writeControl(unsigned char *data) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized object's length field.
	 * @param data serialized object
	 * @return length
	 */
	static uint16_t readLength(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's length field.
	 * @param data serialized object
	 * @param length new length
	 */
	static void writeLength(unsigned char *data, uint16_t length) noexcept;
	/**
	 * Reads serialized object's sequence number.
	 * @param data serialized object
	 * @return sequence number
	 */
	static uint16_t readSequenceNumber(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's sequence number.
	 * @param data serialized object
	 * @param sequenceNumber new sequence number
	 */
	static void writeSequenceNumber(unsigned char *data,
			uint16_t sequenceNumber) noexcept;
	/**
	 * Reads serialized object's session identifier.
	 * @param data serialized object
	 * @return session identifier
	 */
	static uint8_t readSession(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's session identifier.
	 * @param data serialized object
	 * @param session new session identifier
	 */
	static void writeSession(unsigned char *data, uint8_t session) noexcept;
public:
	/** Serialized object's size in bytes */
	static constexpr unsigned int SIZE = 5;
private:
	uint16_t length; //Length in bytes
	uint16_t sequenceNumber; //Sequence number
	uint8_t session; //Communication channel
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGECONTROL_H_ */
