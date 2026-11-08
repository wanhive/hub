/*
 * DynamicHub.h
 *
 * Dynamic hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_DYNAMICHUB_H_
#define WH_SERVER_DYNAMICHUB_H_
#include "../hub/Hub.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * @brief Dynamic hub implementation
 * @details Actively monitors and pulls configuration updates at runtime.
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
	 * @param sfd stores the paired file descriptor
	 * @param blocking true to enable blocking mode on the paired file
	 * descriptor; set to false for non-blocking mode.
	 * @param timeout IO timeout in milliseconds
	 * @return A watcher for the other end of the socket pair, set for
	 * non-blocking IO.
	 */
	Watcher* bridge(int &sfd, bool blocking = false, int timeout = 0);
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	//-----------------------------------------------------------------
private:
	//-----------------------------------------------------------------
	void onInotification(unsigned long long uid,
			const InotifyEvent *event) noexcept override;
	//-----------------------------------------------------------------
	void refresh(unsigned int index) noexcept;
	void setup();
	void sanitize() noexcept;
	void clear() noexcept;
	//-----------------------------------------------------------------
private:
	static constexpr unsigned int TRACKER_CNT = 8;
	struct {
		int context;
		int id;
		uint32_t events;
	} tracker[TRACKER_CNT];
};

} /* namespace wanhive */

#endif /* WH_SERVER_DYNAMICHUB_H_ */
