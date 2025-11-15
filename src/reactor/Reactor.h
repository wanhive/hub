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
	 * @param maxEvents maximum number of IO events to report in each poll
	 * @param signal set to true for handling the asynchronous signal delivery
	 * atomically, i.e. a poll can be reliably interrupted by signals.
	 */
	Reactor(unsigned int maxEvents, bool signal);
	/**
	 * Destructor: releases the managed resources.
	 */
	virtual ~Reactor();
	/**
	 * Initializes the reactor (performs necessary clean-up if the object was
	 * previously initialized).
	 * @param maxEvents maximum number of IO events to report in each poll
	 * @param signal set to true for handling the asynchronous signal delivery
	 * atomically, i.e. a poll can be reliably interrupted by signals.
	 */
	void initialize(unsigned int maxEvents, bool signal);
	//-----------------------------------------------------------------
	/**
	 * Adds a watcher: a watcher must be added only once to only one reactor.
	 * @param w a watcher to monitor
	 * @param events IO events of interest
	 */
	void add(Watcher *w, uint32_t events);
	/**
	 * Modifies the IO events of interest for a given watcher. Can be used to
	 * re-arm the watcher if the TRIGGER_ONCE flag was specified in the events
	 * previously.
	 * @param w a watcher being monitored
	 * @param events IO events of interest
	 */
	void modify(Watcher *w, uint32_t events);
	/**
	 * Invalidates and removes a watcher from the event loop.
	 * @param w a watcher being monitored
	 * @return true if the watcher got invalidated but could not be removed from
	 * the event loop immediately, false otherwise. An invalidated watcher will
	 * be removed synchronously in the next iteration (event loop).
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
	 * EVENT LOOP: processes the ready list of watchers that have some events
	 * available and removes the invalid watchers.
	 */
	void dispatch() noexcept;
	/**
	 * Adds a watcher back to the list of watchers ready for IO (ready list).
	 * @param w a watcher being monitored
	 */
	void retain(Watcher *w) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether the last poll got interrupted by a signal.
	 * @return true if interrupted, false otherwise
	 */
	bool interrupted() const noexcept;
	/**
	 * Checks whether the last poll got timed out.
	 * @return true if timeout expired, false otherwise
	 */
	bool timedOut() const noexcept;
	/**
	 * Returns the current timeout value for poll.
	 * @return timeout value in milliseconds
	 */
	int getTimeout() const noexcept;
	/**
	 * Sets the number of milliseconds that a poll will block.
	 * @param milliseconds timeout value in milliseconds. Set to -1 (default)
	 * to block indefinitely, set to zero (0) for non-blocking operation.
	 */
	void setTimeout(int milliseconds) noexcept;
private:
	/**
	 * Customizes a watcher before adding it to the event loop.
	 * @param w watcher to monitor
	 */
	virtual void adapt(Watcher *w) = 0;
	/**
	 * Reacts to the IO events available on a watcher.
	 * @param w watcher ready for IO operation
	 * @return true if further processing is required, false otherwise.
	 */
	virtual bool react(Watcher *w) noexcept = 0;
	/**
	 * Cleans up a watcher after removing it from the event loop.
	 * @param w watcher being removed
	 */
	virtual void stop(Watcher *w) noexcept = 0;
private:
	/**
	 * Returns the next watcher from the ready list.
	 * @return next watcher
	 */
	Watcher* release() noexcept;
	/**
	 * Removes a watcher permanently from the event loop.
	 * @param w watcher being removed
	 */
	void remove(Watcher *w) noexcept;
private:
	int timeout { -1 };
	Selector selector;
	ReadyList<Watcher*> readyList;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_REACTOR_H_ */
