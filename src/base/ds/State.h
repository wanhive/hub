/*
 * State.h
 *
 * State machine base
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

namespace wanhive {
/**
 * Basic state machine
 */
class State {
public:
	State() noexcept;
	~State();

	//Clear out all the fields, except the mark
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * getXXX: returns the value
	 * isXXX: returns true if the value equals the argument
	 * testXXX: tests whether given bits are set in the bitmask
	 * putXXX: replaces the bitmask/value with the given argument
	 * setXXX: sets the given bits in a bitmask (replaces if type or group)
	 * clearXXX: clears the given bits in a bitmask
	 * maskXXX: if <set> then set the given bits otherwise clear the given bits
	 */

	void* getReference() const noexcept;
	void setReference(void *reference) noexcept;

	int getType() const noexcept;
	bool isType(int type) const noexcept;
	void setType(int type) noexcept;

	uint32_t getEvents() const noexcept;
	bool testEvents(uint32_t events) const noexcept;
	void putEvents(uint32_t events) noexcept;
	void setEvents(uint32_t events) noexcept;
	void clearEvents(uint32_t events) noexcept;
	void maskEvents(uint32_t events, bool set) noexcept;

	uint32_t getFlags() const noexcept;
	bool testFlags(uint32_t flags) const noexcept;
	void putFlags(uint32_t flags) noexcept;
	void setFlags(uint32_t flags) noexcept;
	void clearFlags(uint32_t flags) noexcept;
	void maskFlags(uint32_t flags, bool set) noexcept;

	unsigned char getGroup() const noexcept;
	bool testGroup(unsigned char group) const noexcept;
	void setGroup(unsigned char group) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Indelible mark: cannot be cleared once set
	 */
	bool isMarked() const noexcept;
	void setMarked() noexcept;
private:
	void *reference;
	int type;
	uint32_t flags;
	uint32_t events;
	unsigned char group;
	bool marked;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_STATE_H_ */
