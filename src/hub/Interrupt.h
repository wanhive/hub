/*
 * Interrupt.h
 *
 * Signal watcher
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_INTERRUPT_H_
#define WH_HUB_INTERRUPT_H_
#include "../reactor/Watcher.h"

namespace wanhive {
/**
 * Signal watcher
 * @note Abstraction of Linux's signalfd(2) mechanism
 */
class Interrupt final: public Watcher {
public:
	/**
	 * Constructor: creates a signal watcher.
	 * @param blocking true for blocking IO, false for non-blocking IO (default)
	 */
	Interrupt(bool blocking = false);
	/**
	 * Destructor
	 */
	~Interrupt();
	//-----------------------------------------------------------------
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Reads a pending signal.
	 * @param signum signal's number
	 * @return number of bytes read on success, 0 if non-blocking mode is on and
	 * the call would block, -1 if the file descriptor was closed.
	 */
	ssize_t read(int &signum);
private:
	void create(bool blocking);
};

} /* namespace wanhive */

#endif /* WH_HUB_INTERRUPT_H_ */
