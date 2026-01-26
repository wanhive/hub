/**
 * @file State.h
 *
 * State machine
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_STATE_H_
#define WH_BASE_DS_STATE_H_
#include <cstdint>

/*! @namespace wanhive */
namespace wanhive {
/**
 * Basic state machine
 */
class State {
public:
	/**
	 * Default constructor: clears all the fields.
	 */
	State() noexcept;
	/**
	 * Destructor
	 */
	~State();
	//-----------------------------------------------------------------
	/**
	 * Clears out all the fields except the mark (see State::setMarked()).
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the pointer value associated to this object.
	 * @return the pointer value
	 */
	void* getReference() const noexcept;
	/**
	 * Associates a generic pointer to this object.
	 * @param reference the pointer value
	 */
	void setReference(void *reference) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the type identifier.
	 * @return the type value
	 */
	int getType() const noexcept;
	/**
	 * Checks whether the type identifier equals the given value.
	 * @param type the expected value
	 * @return true on match, false on mismatch
	 */
	bool isType(int type) const noexcept;
	/**
	 * Assigns a new value to the type identifier.
	 * @param type the new value
	 */
	void setType(int type) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the flags value.
	 * @return 32-bit value describing the flags
	 */
	uint32_t getFlags() const noexcept;
	/**
	 * Tests whether the given flags bits are set.
	 * @param flags 32-bit bitmask for testing
	 * @return true if at least one of the bits from the bitmask is set,
	 * false otherwise.
	 */
	bool testFlags(uint32_t flags) const noexcept;
	/**
	 * Sets a new value for flags.
	 * @param flags the new 32-bit value
	 */
	void putFlags(uint32_t flags) noexcept;
	/**
	 * Sets the given flags bits.
	 * @param flags the bitmask to set
	 */
	void setFlags(uint32_t flags) noexcept;
	/**
	 * Clears the given flags bits.
	 * @param flags the bitmask to clear
	 */
	void clearFlags(uint32_t flags) noexcept;
	/**
	 * Sets or clears the given events bits.
	 * @param flags the bitmask to set or clear
	 * @param set true to set the bits, false to clear the bits
	 */
	void maskFlags(uint32_t flags, bool set) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the events.
	 * @return 32-bit value describing the events
	 */
	uint32_t getEvents() const noexcept;
	/**
	 * Tests whether the given events bits are set.
	 * @param events the 32-bit bitmask for testing
	 * @return true if at least one of the bits from the bitmask is set,
	 * false otherwise.
	 */
	bool testEvents(uint32_t events) const noexcept;
	/**
	 * Sets a new value for events.
	 * @param events the new 32-bit value
	 */
	void putEvents(uint32_t events) noexcept;
	/**
	 * Sets the given events bits.
	 * @param events the bitmask to set
	 */
	void setEvents(uint32_t events) noexcept;
	/**
	 * Clears the given events bits.
	 * @param events the bitmask to clear
	 */
	void clearEvents(uint32_t events) noexcept;
	/**
	 * Sets or clears the given events bits.
	 * @param events the bitmask to set or clear
	 * @param set true to set the bits, false to clear the bits
	 */
	void maskEvents(uint32_t events, bool set) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the trace.
	 * @return 32-bit value describing the trace
	 */
	uint32_t getTrace() const noexcept;
	/**
	 * Tests whether the given trace bits are set.
	 * @param trace the 32-bit bitmask for testing
	 * @return true if at least one of the bits from the bitmask is set,
	 * false otherwise.
	 */
	bool testTrace(uint32_t trace) const noexcept;
	/**
	 * Sets a new value for trace.
	 * @param trace the new 32-bit value
	 */
	void putTrace(uint32_t trace) noexcept;
	/**
	 * Sets the given trace bits.
	 * @param trace the bitmask to set
	 */
	void setTrace(uint32_t trace) noexcept;
	/**
	 * Clears the given trace bits.
	 * @param trace the bitmask to clear
	 */
	void clearTrace(uint32_t trace) noexcept;
	/**
	 * Sets or clears the given trace bits.
	 * @param trace the bitmask to set or clear
	 * @param set true to set the bits, false to clear the bits
	 */
	void maskTrace(uint32_t trace, bool set) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the group identifier.
	 * @return group identifier's value
	 */
	unsigned char getGroup() const noexcept;
	/**
	 * Checks whether the group identifier equals the given value.
	 * @param group the expected value
	 * @return true on match, false on mismatch
	 */
	bool testGroup(unsigned char group) const noexcept;
	/**
	 * Sets the group identifier.
	 * @param group the new group identifier
	 */
	void setGroup(unsigned char group) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks the indelible mark.
	 * @return true if the mark is set, false otherwise
	 */
	bool isMarked() const noexcept;
	/**
	 * Sets the indelible mark which cannot be cleared.
	 */
	void setMarked() noexcept;
private:
	void *reference { };
	int type { };
	uint32_t flags { };
	uint32_t events { };
	uint32_t trace { };
	unsigned char group { };
	bool marked { };
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_STATE_H_ */
