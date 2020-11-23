/*
 * Finger.h
 *
 * Finger implementation for the Chord (distributed hash table)
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
 * Finger implementation for the Chord (distributed hash table)
 * Ref: https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf
 */
class Finger {
public:
	Finger() noexcept;
	~Finger();

	unsigned int getStart() const noexcept;
	void setStart(unsigned int start) noexcept;

	unsigned int getId() const noexcept;
	void setId(unsigned int id) noexcept;

	unsigned int getOldId() const noexcept;
	void setOldId(unsigned int oldId) noexcept;

	bool isConsistent() const noexcept;
	void makeConsistent() noexcept;

	bool isConnected() const noexcept;
	void setConnected(bool connected) noexcept;
private:
	unsigned int start;
	unsigned int id;
	unsigned int oldId;
	bool connected;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_FINGER_H_ */
