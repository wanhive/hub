/*
 * SignalWatcher.h
 *
 * Signal handler for Wanhive hubs
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
/**
 * Abstraction of Linux's signalfd(2) mechanism
 * Synchronous signal handler
 */
using SignalInfo=signalfd_siginfo;
class SignalWatcher: public Watcher {
public:
	SignalWatcher(bool blocking = false);
	virtual ~SignalWatcher();
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
	/*
	 * Returns the number of bytes read, possibly zero (buffer full or would block),
	 * or -1 if the descriptor got closed. Every new call overwrites the old data.
	 */
	ssize_t read();
	//Retrieves information about caught signals
	const SignalInfo* getSignalInfo() const noexcept;
private:
	SignalInfo info;
};

} /* namespace wanhive */

#endif /* WH_HUB_SIGNALWATCHER_H_ */
