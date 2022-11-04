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

#ifndef WH_SERVER_OVERLAY_FINGER_H_
#define WH_SERVER_OVERLAY_FINGER_H_

namespace wanhive {
/**
 * Finger implementation for the chord (distributed hash table)
 * @ref https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf
 */
class Finger {
public:
	/**
	 * Default constructor
	 */
	Finger() noexcept;
	/**
	 * Destructor
	 */
	~Finger();
	//-----------------------------------------------------------------
	/**
	 * Returns the start index on the identifier ring.
	 * @return start index's value
	 */
	unsigned int getStart() const noexcept;
	/**
	 * Sets the start index on the identifier ring.
	 * @param start index's value
	 */
	void setStart(unsigned int start) noexcept;

	/**
	 * Returns the current index on the on the identifier ring.
	 * @return current index's value
	 */
	unsigned int getId() const noexcept;
	/**
	 * Sets the current index on the identifier ring.
	 * @param id current index's value
	 */
	void setId(unsigned int id) noexcept;
	/**
	 * Returns the old index on the on the identifier ring.
	 * @return old index's value
	 */
	unsigned int getOldId() const noexcept;
	/**
	 * Sets the current index on the identifier ring.
	 * @param oldId old index's value
	 */
	void setOldId(unsigned int oldId) noexcept;
	/**
	 * Returns the "connected" status.
	 * @return true if connected, false otherwise
	 */
	bool isConnected() const noexcept;
	/**
	 * Sets the "connected" status.
	 * @param connected true if connected, false if not connected
	 */
	void setConnected(bool connected) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether the current index matches the old index.
	 * @return true if current and old indexes are equal, false otherwise.
	 */
	bool isConsistent() const noexcept;
	/**
	 * Updates the old index to match the current index.
	 * @return previous value of the old index
	 */
	unsigned int commit() noexcept;
private:
	unsigned int start { 0 };
	unsigned int id { 0 };
	unsigned int oldId { 0 };
	bool connected { false };
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_FINGER_H_ */
