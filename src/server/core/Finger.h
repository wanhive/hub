/*
 * Finger.h
 *
 * Finger implementation for the chord (distributed hash table)
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_CORE_FINGER_H_
#define WH_SERVER_CORE_FINGER_H_

namespace wanhive {
/**
 * Finger implementation for the chord DHT (distributed hash table)
 * @ref https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf
 */
class Finger {
public:
	/**
	 * Constructor: creates a finger.
	 */
	Finger() noexcept;
	/**
	 * Destructor
	 */
	~Finger();
	//-----------------------------------------------------------------
	/**
	 * Returns start index on the identifier ring.
	 * @return start index
	 */
	unsigned int getStart() const noexcept;
	/**
	 * Sets start index on the identifier ring.
	 * @param start index
	 */
	void setStart(unsigned int start) noexcept;

	/**
	 * Returns current index on the identifier ring.
	 * @return current index
	 */
	unsigned int getId() const noexcept;
	/**
	 * Sets current index on the identifier ring.
	 * @param id current index
	 */
	void setId(unsigned int id) noexcept;
	/**
	 * Returns prior index on the identifier ring.
	 * @return prior index
	 */
	unsigned int getPrior() const noexcept;
	/**
	 * Sets prior index on the identifier ring.
	 * @param prior prior index
	 */
	void setPrior(unsigned int prior) noexcept;
	/**
	 * Returns the connected status.
	 * @return true if connected, false otherwise
	 */
	bool isConnected() const noexcept;
	/**
	 * Sets the connected status.
	 * @param connected true for connected, false for not connected
	 */
	void setConnected(bool connected) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether the current index matches the prior index.
	 * @return true on positive match, false otherwise
	 */
	bool isConsistent() const noexcept;
	/**
	 * Updates the prior index to match the current index.
	 * @return prior index's value before the update
	 */
	unsigned int commit() noexcept;
private:
	unsigned int start { };
	unsigned int id { };
	unsigned int prior { };
	bool connected { };
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_FINGER_H_ */
