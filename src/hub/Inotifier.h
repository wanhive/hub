/*
 * Inotifier.h
 *
 * File system monitor for Wanhive hubs
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
 * Abstraction of Linux's inotify(7) mechanism
 * Reports file system events
 */
using InotifyEvent=inotify_event;
class Inotifier: public Watcher {
public:
	Inotifier(bool blocking = false);
	virtual ~Inotifier();
	//-----------------------------------------------------------------
	//Start the notifier (no-op)
	void start() override final;
	//Disarm the notifier (no-op)
	void stop() noexcept override final;
	//Handle the file system notification
	bool callback(void *arg) noexcept override final;
	//Always returns false
	bool publish(void *arg) noexcept override final;
	//-----------------------------------------------------------------
	//Returns a unique identifier (watch descriptor) on success
	int add(const char *pathname, uint32_t mask = IN_CLOSE_WRITE | IN_ATTRIB);
	//Removes an identifier (watch descriptor)
	void remove(int identifier);
	/*
	 * Returns the number of bytes read, possibly zero (buffer full or would
	 * block), or -1 if the descriptor was closed. Each new call overwrites the
	 * previous notifications.
	 */
	ssize_t read();
	//Returns the next notification
	const InotifyEvent* next();
private:
	unsigned char buffer[4096];	//MIN: sizeof(inotify_event) + NAME_MAX + 1
	unsigned int offset;
	unsigned int limit;
};

} /* namespace wanhive */

#endif /* WH_HUB_INOTIFIER_H_ */
