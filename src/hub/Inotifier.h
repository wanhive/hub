/*
 * Inotifier.h
 *
 * File system watcher
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_INOTIFIER_H_
#define WH_HUB_INOTIFIER_H_
#include "../reactor/Watcher.h"
#include <sys/inotify.h>

namespace wanhive {
/**
 * Treat this as an opaque object and use the methods provided by the Inotifier
 * class to fetch additional information.
 */
using InotifyEvent = inotify_event;

/**
 * Reports file system events
 * @note Abstraction of Linux's inotify(7) mechanism
 */
class Inotifier final: public Watcher {
public:
	/**
	 * Constructor: creates a file system watcher.
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 */
	Inotifier(bool blocking = false);
	/**
	 * Destructor
	 */
	~Inotifier();
	//-----------------------------------------------------------------
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Adds a file or directory to the monitor.
	 * @param pathname path to monitor
	 * @param mask events of interest
	 * @return watch descriptor
	 */
	int add(const char *pathname, uint32_t mask = IN_CLOSE_WRITE | IN_ATTRIB);
	/**
	 * Stops monitoring a file/directory.
	 * @param identifier watch descriptor
	 */
	void remove(int identifier);
	/**
	 * Reads file system events and stores them into an internal buffer. Each
	 * call overwrites the old events.
	 * @return number of bytes read on success, 0 if non-blocking mode is on and
	 * the call would block, -1 if the file descriptor was closed.
	 */
	ssize_t read();
	//-----------------------------------------------------------------
	/**
	 * Returns the next notification.
	 * @return next notification or nullptr
	 */
	const InotifyEvent* next() noexcept;
	/**
	 * Returns the watch descriptor associated with a notification.
	 * @param e notification
	 * @return watch descriptor
	 */
	static int getWatchDescriptor(const InotifyEvent *e) noexcept;
	/**
	 * Returns the events associated with a notification.
	 * @param e notification
	 * @return 32-bit events mask
	 */
	static uint32_t getMask(const InotifyEvent *e) noexcept;
	/**
	 * Returns the unique cookie associating related events (for rename(2)).
	 * @param e notification
	 * @return cookie for tracking a renamed file
	 */
	static uint32_t getCookie(const InotifyEvent *e) noexcept;
	/**
	 * Returns notification's source in a watched directory.
	 * @param e notification
	 * @return source file's name
	 */
	static const char* getFileName(const InotifyEvent *e) noexcept;
private:
	unsigned int offset { }; //
	unsigned int limit { }; //
	alignas(InotifyEvent) unsigned char buffer[4096];
};

} /* namespace wanhive */

#endif /* WH_HUB_INOTIFIER_H_ */
