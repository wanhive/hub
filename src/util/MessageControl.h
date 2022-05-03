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

class MessageControl {
public:
	MessageControl() noexcept;
	MessageControl(uint16_t length, uint16_t sequenceNumber,
			uint8_t session) noexcept;
	MessageControl(const unsigned char *buffer) noexcept;
	~MessageControl();
	//-----------------------------------------------------------------
	uint16_t getLength() const noexcept;
	void setLength(uint16_t length) noexcept;
	uint16_t getSequenceNumber() const noexcept;
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	uint8_t getSession() const noexcept;
	void setSession(uint8_t session) noexcept;
	//-----------------------------------------------------------------
	void getControl(uint16_t &length, uint16_t &sequenceNumber,
			uint8_t &session) const noexcept;
	void setControl(uint16_t length, uint16_t sequenceNumber,
			uint8_t session) noexcept;
	//-----------------------------------------------------------------
	void readControl(const unsigned char *buffer) noexcept;
	void writeControl(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	static uint16_t readLength(const unsigned char *buffer) noexcept;
	static void writeLength(unsigned char *buffer, uint16_t length) noexcept;
	static uint16_t readSequenceNumber(const unsigned char *buffer) noexcept;
	static void writeSequenceNumber(unsigned char *buffer,
			uint16_t sequenceNumber) noexcept;
	static uint8_t readSession(const unsigned char *buffer) noexcept;
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
