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
	/**
	 * Constructor: zero-initializes all the fields.
	 */
	FlowControl() noexcept;
	/**
	 * Destructor
	 */
	~FlowControl();
	//-----------------------------------------------------------------
	/**
	 * Returns the current sequence number.
	 * @return sequence number
	 */
	uint16_t getSequenceNumber() const noexcept;
	/**
	 * Generates and returns a new sequence number.
	 * @return next sequence number
	 */
	uint16_t nextSequenceNumber() noexcept;
	/**
	 * Sets a new sequence number.
	 * @param sequenceNumber sequence number's value
	 */
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the current session identifier.
	 * @return session identifier
	 */
	uint8_t getSession() const noexcept;
	/**
	 * Sets a new session identifier.
	 * @param session session identifier's value
	 */
	void setSession(uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the current source identifier.
	 * @return source identifier
	 */
	uint64_t getSource() const noexcept;
	/**
	 * Sets a new source identifier.
	 * @param source new source identifier's value
	 */
	void setSource(uint64_t source) noexcept;
private:
	uint64_t source { 0 };
	uint16_t sequenceNumber { 0 };
	uint8_t session { 0 };
};

} /* namespace wanhive */

#endif /* WH_UTIL_FLOWCONTROL_H_ */
