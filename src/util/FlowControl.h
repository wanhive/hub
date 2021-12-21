/*
 * FlowControl.h
 *
 * Message stream flow control
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_FLOWCONTROL_H_
#define WH_UTIL_FLOWCONTROL_H_
#include <cstdint>

namespace wanhive {
/**
 * Message stream flow control
 */
class FlowControl {
public:
	FlowControl() noexcept;
	~FlowControl();

	//Returns the current sequence number
	uint16_t getSequenceNumber() const noexcept;
	//Generates and returns the next sequence number
	uint16_t nextSequenceNumber() noexcept;
	//Sets the sequence number
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;

	//Returns the session identifier
	uint8_t getSession() const noexcept;
	//Sets the session identifier
	void setSession(uint8_t session) noexcept;

	//Returns the source identifier
	uint64_t getSource() const noexcept;
	//Sets the source identifier
	void setSource(uint64_t source) noexcept;
private:
	uint64_t source;
	uint16_t sequenceNumber;
	uint8_t session;
};

} /* namespace wanhive */

#endif /* WH_UTIL_FLOWCONTROL_H_ */