/*
 * Reactor.h
 *
 * Synchronous event selector and dispatcher
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_REACTOR_REACTOR_H_
#define WH_REACTOR_REACTOR_H_
#include "Watcher.h"
#include "../base/ds/ReadyList.h"
#include "../base/Selector.h"

namespace wanhive {
/**
 * Reactor pattern implementation (synchronous event selector and dispatcher)
 * @ref http://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf
 */
class Reactor {
public:
	/**
	 * Constructor: creates a reactor.
	 */
	Reactor() noexcept;
	/**
	 * Constructor: creates and initializes a reactor.
	 * @param events maximum number of IO events to report in each poll
	 * @param signal set to true for handling the asynchronous signal delivery
	 * atomically, i.e. a poll can be reliably interrupted by signals.
	 */
	Reactor(unsigned int events, bool signal);
	/**
	 * Destructor: releases the managed resources.
	 */
	virtual ~Reactor();
	/**
	 * Initializes the reactor (performs necessary clean-up if the object was
	 * previously initialized).
	 * @param events maximum number of IO events to report in each poll
	 * @param signal set to true for handling the asynchronous signal delivery
	 * atomically, i.e. a poll can be reliably interrupted by signals.
	 */
	void initialize(unsigned int events, bool signal);
	//-----------------------------------------------------------------
	/**
	 * Adds a watcher: a watcher must be added only once to only one reactor.
	 * @param w a watcher to monitor
	 * @param events IO events of interest
	 */
	void add(Watcher *w, uint32_t events);
	/**
	 * Updates the I/O events of interest for a specified watcher. This can be
	 * used to re-arm the watcher if the TRIGGER_ONCE flag was previously set
	 * for the events.
	 * @param w a watcher being monitored
	 * @param events IO events of interest
	 */
	void modify(Watcher *w, uint32_t events);
	/**
	 * Invalidates and removes a watcher from the event loop.
	 * @param w a watcher being monitored
	 * @return true if the watcher was invalidated but not removed, false if it
	 * was successfully removed. An invalidated watcher will be removed in the
	 * next event loop iteration.
	 */
	bool disable(Watcher *w) noexcept;
	//-----------------------------------------------------------------
	/**
	 * EVENT LOOP: waits for IO events on watchers, signal-delivery, or timeout.
	 * @param block set true to block until timeout or signal delivery. Set
	 * false to return immediately even if no IO event got reported.
	 */
	void poll(bool block);
	/**
	 * EVENT LOOP: processes the ready list of watchers with available events
	 * and removes invalid ones.
	 */
	void dispatch() noexcept;
	/**
	 * Adds a watcher to the ready list of watchers.
	 * @param w a watcher being monitored
	 */
	void retain(Watcher *w) noexcept;
	/**
	 * Checks whether the last poll got timed out.
	 * @return true if timeout expired, false otherwise
	 */
	bool expired() const noexcept;
	/**
	 * Checks whether the last poll got interrupted by a signal.
	 * @return true if interrupted, false otherwise
	 */
	bool interrupted() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the current timeout value for poll.
	 * @return timeout value in milliseconds
	 */
	int getTimeout() const noexcept;
	/**
	 * Sets the timeout value for poll.
	 * @param timeout timeout value in milliseconds. Set to -1 (default)
	 * to block indefinitely, set to zero (0) for non-blocking operation.
	 */
	void setTimeout(int timeout) noexcept;
private:
	/**
	 * Processes a watcher before adding it to the event loop.
	 * @param w a watcher to monitor
	 */
	virtual void admit(Watcher *w) = 0;
	/**
	 * Reacts to the IO events available on a watcher.
	 * @param w a watcher ready for IO operation
	 * @return true if further processing is required, false otherwise.
	 */
	virtual bool react(Watcher *w) noexcept = 0;
	/**
	 * Processes a watcher after removing it from the event loop.
	 * @param w a watcher being removed
	 */
	virtual void expel(Watcher *w) noexcept = 0;
private:
	Watcher* ready() noexcept;
	void remove(Watcher *w) noexcept;
private:
	int timeout { -1 };
	Selector selector;
	ReadyList<Watcher*> readyList;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_REACTOR_H_ */
