/*
 * SignalWatcher.h
 *
 * Signal watcher
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_SIGNALWATCHER_H_
#define WH_HUB_SIGNALWATCHER_H_
#include "../reactor/Watcher.h"
#include <sys/signalfd.h>

namespace wanhive {
using SignalInfo=signalfd_siginfo;
/**
 * Signal watcher
 * Abstraction of Linux's signalfd(2) mechanism
 */
class SignalWatcher: public Watcher {
public:
	/**
	 * Constructor: creates a watcher for accepting signals.
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 */
	SignalWatcher(bool blocking = false);
	/**
	 * Destructor
	 */
	virtual ~SignalWatcher();
	//-----------------------------------------------------------------
	void start() override final;
	void stop() noexcept override final;
	bool callback(void *arg) noexcept override final;
	bool publish(void *arg) noexcept override final;
	//-----------------------------------------------------------------
	/**
	 * Accepts a pending signal. Call SignalWatcher::getInfo to get the result.
	 * @return number of bytes read on success, 0 if non-blocking mode is on and
	 * the call would block, -1 if the underlying file descriptor was closed.
	 */
	ssize_t read();
	/**
	 * Returns the information about the most recently caught signal. (Each new
	 * SignalWatcher::read() call overwrites the old value).
	 * @return pointer to object containing the signal information
	 */
	const SignalInfo* getInfo() const noexcept;
private:
	void create(bool blocking);
private:
	SignalInfo info;
};

} /* namespace wanhive */

#endif /* WH_HUB_SIGNALWATCHER_H_ */
