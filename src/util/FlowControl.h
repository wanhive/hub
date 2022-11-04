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
	 * Constructor: initializes all the fields to zero (0).
	 */
	FlowControl() noexcept;
	/**
	 * Destructor
	 */
	~FlowControl();
	//-----------------------------------------------------------------
	/**
	 * Returns the current sequence number.
	 * @return the sequence number
	 */
	uint16_t getSequenceNumber() const noexcept;
	/**
	 * Generates and returns a new sequence number.
	 * @return the new sequence number
	 */
	uint16_t nextSequenceNumber() noexcept;
	/**
	 * Sets a new sequence number.
	 * @param sequenceNumber the desired sequence number
	 */
	void setSequenceNumber(uint16_t sequenceNumber) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the current session identifier.
	 * @return the session identifier
	 */
	uint8_t getSession() const noexcept;
	/**
	 * Sets the session identifier.
	 * @param session the desired session identifier
	 */
	void setSession(uint8_t session) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the current source identifier.
	 * @return the source identifier
	 */
	uint64_t getSource() const noexcept;
	/**
	 * Sets a new source identifier.
	 * @param source the new source identifier
	 */
	void setSource(uint64_t source) noexcept;
private:
	uint64_t source { 0 };
	uint16_t sequenceNumber { 0 };
	uint8_t session { 0 };
};

} /* namespace wanhive */

#endif /* WH_UTIL_FLOWCONTROL_H_ */
