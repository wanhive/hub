/*
 * Inotifier.h
 *
 * File system monitor
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
 * Treat this as an opaque object and use methods provided by the Inotifier to
 * fetch additional information.
 */
using InotifyEvent = inotify_event;

/**
 * Reports file system events
 * @note Abstraction of Linux's inotify(7) mechanism
 */
class Inotifier final: public Watcher {
public:
	/**
	 * Creates a new file-system monitor.
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
	 * Adds a pathname (file/directory) to the monitor.
	 * @param pathname the file/directory for monitoring
	 * @param mask the events of interest
	 * @return a unique identifier (watch descriptor)
	 */
	int add(const char *pathname, uint32_t mask = IN_CLOSE_WRITE | IN_ATTRIB);
	/**
	 * Stops monitoring a file/directory.
	 * @param identifier the watch descriptor (see Inotifier::add())
	 */
	void remove(int identifier);
	/**
	 * Reads file system events and stores them into the internal buffer. Call
	 * Inotifier::next() to retrieve the notifications. Each new call overwrites
	 * the previous notifications.
	 * @return number of bytes read on success, 0 if non-blocking mode is on and
	 * the call would block, -1 if the underlying file descriptor was closed.
	 */
	ssize_t read();
	/**
	 * Returns the next notification (see Inotifier::read()).
	 * @return the next notification, nullptr if no notification is pending
	 */
	const InotifyEvent* next() noexcept;
	/**
	 * Returns the watch descriptor associated with the given notification.
	 * @param e the notification
	 * @return watch descriptor's value
	 */
	static int getWatchDescriptor(const InotifyEvent *e) noexcept;
	/**
	 * Returns the events associated with the given notification.
	 * @param e the notification
	 * @return 32-bit event mask
	 */
	static uint32_t getMask(const InotifyEvent *e) noexcept;
	/**
	 * Returns the unique cookie associating related events (for rename(2)).
	 * @param e the notification
	 * @return the cookie for tracking a renamed file
	 */
	static uint32_t getCookie(const InotifyEvent *e) noexcept;
	/**
	 * Returns notification's source within the watched directory.
	 * @param e the notification
	 * @return file's name which generated the notification
	 */
	static const char* getFileName(const InotifyEvent *e) noexcept;
private:
	//Offset within the buffer
	unsigned int offset { 0 };
	//Notifications count
	unsigned int limit { 0 };
	//MIN: sizeof(inotify_event) + NAME_MAX + 1
	alignas(InotifyEvent) unsigned char buffer[4096];
};

} /* namespace wanhive */

#endif /* WH_HUB_INOTIFIER_H_ */
