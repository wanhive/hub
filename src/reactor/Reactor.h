/*
 * Reactor.h
 *
 * Synchronous event demultiplexer and dispatcher
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
#include "../base/ds/Array.h"
#include "../base/Selector.h"

namespace wanhive {
/**
 * Reactor pattern implementation (synchronous event demultiplexer and dispatcher)
 * @ref http://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf
 */
class Reactor {
public:
	/**
	 * Default constructor: creates a new reactor (call  Reactor::initialize() to
	 * completely initialize the reactor).
	 */
	Reactor() noexcept;
	/**
	 * Constructor: creates an initialized reactor.
	 * @param maxEvents the maximum number of IO events to report in each call
	 * to Reactor::poll().
	 * @param signal set to true for handling the asynchronous signal delivery
	 * atomically, i.e. Reactor::poll() can be reliably interrupted by a signal.
	 */
	Reactor(unsigned int maxEvents, bool signal);
	/**
	 * Destructor: cleans up the internal resources.
	 */
	virtual ~Reactor();
	/**
	 * Initializes the reactor after performing any necessary clean-up (if the
	 * object was previously initialized).
	 * @param maxEvents the maximum number of IO events to report in each call
	 * to Reactor::poll().
	 * @param signal set to true for handling the asynchronous signal delivery
	 * atomically, i.e. Reactor::poll() can be reliably interrupted by a signal.
	 */
	void initialize(unsigned int maxEvents, bool signal);
	//-----------------------------------------------------------------
	/**
	 * Adds a watcher to this reactor. A watcher must be added only once, and to
	 * only one reactor.
	 * @param w the watcher to monitor
	 * @param events the IO events of interest
	 */
	void add(Watcher *w, uint32_t events);
	/**
	 * Modifies the IO events of interest for the given watcher. Can be used to
	 * re-arm the watcher if the TRIGGER_ONCE flag was specified in the events
	 * previously.
	 * @param w a watcher already being monitored by this reactor
	 * @param events the new IO events
	 */
	void modify(Watcher *w, uint32_t events);
	/**
	 * Invalidates and removes a watcher from the event loop, the removed watcher
	 * will no longer be monitored by this reactor.
	 * @param w a watcher currently being monitored by this reactor
	 * @return true if the watcher got invalidated but could not be removed from
	 * the event loop immediately, false otherwise. An invalidated watcher will
	 * be removed synchronously in the next iteration (event loop) if it could
	 * not be removed immediately.
	 */
	bool disable(Watcher *w) noexcept;
	//-----------------------------------------------------------------
	/**
	 * The first step of an event loop: waits for IO events on the registered
	 * watchers, signal-delivery, or timeout (see Reactor::setTimeout()).
	 * @param block if true then the call will block until the timeout or signal
	 * delivery. if false then the call will return immediately even if no IO
	 * event got reported.
	 */
	void poll(bool block);
	/**
	 * The last step of an event loop: processes the ready list of watchers that
	 * have some events available and removes the invalid watchers.
	 */
	void dispatch() noexcept;
	/**
	 * Adds a watcher back to the ready-list of watchers which are ready for
	 * dispatch (see Reactor::dispatch()).
	 * @param w a watcher being monitored by this reactor
	 */
	void retain(Watcher *w) noexcept;
	/**
	 * Checks whether the last call to Reactor::poll() got interrupted by a
	 * signal.
	 * @return true on signal delivery, false otherwise
	 */
	bool interrupted() const noexcept;
	/**
	 * Checks whether the last call to Reactor::poll() got timed out.
	 * @return true on timeout, false otherwise
	 */
	bool timedOut() const noexcept;
	/**
	 * Returns the timeout value (see Reactor::poll()).
	 * @return the timeout value
	 */
	int getTimeout() const noexcept;
	/**
	 * Sets a new timeout value in milliseconds (see Reactor::poll()).
	 * @param milliseconds the timeout value in milliseconds. Set this value to
	 * -1 (default) to block indefinitely, setting it to zero (0) will result in
	 * non-blocking operation.
	 */
	void setTimeout(int milliseconds) noexcept;
private:
	/**
	 * Customizes a watcher before adding it to this reactor's event loop. This
	 * method is called internally by Reactor::add().
	 * @param w a watcher being added to this reactor
	 */
	virtual void adapt(Watcher *w) = 0;
	/**
	 * Reacts to the IO events reported on a watcher being monitored by this
	 * reactor. This method is called internally by Reactor::dispatch().
	 * @param w a watcher which has reported IO events
	 * @return true if further processing is required (e.g. IO events are still
	 * pending), false otherwise.
	 */
	virtual bool react(Watcher *w) noexcept = 0;
	/**
	 * Cleans up a watcher and the associated records after removing it from the
	 * event loop (see Reactor::disable() and Reactor::dispatch()).
	 * @param w the watcher being removed
	 */
	virtual void stop(Watcher *w) noexcept = 0;
private:
	/**
	 * Returns the next watcher from the ready-list.
	 * @return a watcher which requires processing
	 */
	Watcher* release() noexcept;
	/**
	 * Removes a watcher permanently from the event loop.
	 * @param w a watcher currently being monitored by this reactor
	 */
	void remove(Watcher *w) noexcept;
private:
	//Selector timeout in milliseconds (default value: -1)
	int timeout;
	//The selector (event demultiplexer)
	Selector selector;
	//The ready list of watchers
	Array<Watcher*> readyList;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_REACTOR_H_ */
