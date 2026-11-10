/**
 * @file DynamicHub.h
 *
 * Dynamic hub
 *
 *
 * Copyright (C) 2026 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_DYNAMICHUB_H_
#define WH_HUB_DYNAMICHUB_H_
#include "Hub.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * @brief Dynamic Hub
 * @details Monitors and retrieves configuration updates at runtime.
 */
class DynamicHub: public Hub {
public:
	/**
	 * Constructor: creates a new dynamic hub.
	 * @param uid unique identifier
	 * @param path configuration file's path
	 */
	DynamicHub(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~DynamicHub();
protected:
	//-----------------------------------------------------------------
	/**
	 * Creates a connected pair of Unix domain sockets.
	 * @param sfd stores the file descriptor for one end of the socket pair
	 * @param blocking true to enable blocking mode on the paired file
	 * descriptor; set to false for non-blocking mode.
	 * @param timeout IO timeout in milliseconds
	 * @return a registered watcher for the other end of the socket pair, set
	 * for non-blocking I/O.
	 */
	Watcher* pair(int &sfd, bool blocking = false, int timeout = 0);
	/**
	 * Handles file system events and updates the settings accordingly.
	 * @param wd watch descriptor
	 * @param mask watch mask for the events
	 * @return true if an event of interest occurs; false otherwise
	 */
	bool reload(int wd, uint32_t mask) noexcept;
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	//-----------------------------------------------------------------
private:
	//-----------------------------------------------------------------
	void onInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept override;
	//-----------------------------------------------------------------
	void reload(unsigned int index) noexcept;
	void setup();
	void sanitize() noexcept;
	void clear() noexcept;
	//-----------------------------------------------------------------
private:
	static constexpr unsigned int TRACKERS = 8;
	struct {
		int context;
		int id;
		uint32_t events;
	} tracker[TRACKERS];
};

} /* namespace wanhive */

#endif /* WH_HUB_DYNAMICHUB_H_ */
