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
 * Ref: http://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf
 * Thread safe at class level
 */
class Reactor {
public:
	//Doesn't initialize the object, call Reactor::initialize explicitly
	Reactor() noexcept;
	/*
	 * <maxEvents> is the maximum number of events to be reported in each cycle.
	 * If <signal> is true then the reactor handles asynchronous signal delivery
	 * atomically, i.e. it can be safely interrupted by signal.
	 */
	Reactor(unsigned int maxEvents, bool signal);
	virtual ~Reactor();
	/*
	 * Initialize the selector, <maxEvents> is the maximum number of IO events
	 * to be reported in each cycle. If <signal> is true then the reactor can
	 * be safely interrupted by asynchronously delivered signal.
	 */
	void initialize(unsigned int maxEvents, bool signal);
	//Start monitoring the Watcher <w> for <events>
	void add(Watcher *w, uint32_t events);
	/*
	 * Modify the <events> associated with a watcher. Can be used to re-arm the
	 * watcher if TRIGGER_ONCE flag was specified in the events previously.
	 */
	void modify(Watcher *w, uint32_t events);
	/*
	 * Invalidates a watcher and returns true if it is still in use internally,
	 * removes it immediately and returns false otherwise. Invalidated watcher
	 * will be removed by the reactor synchronously in the next iteration if
	 * it could not be removed immediately.
	 */
	bool disable(Watcher *w) noexcept;
	/*
	 * Waits for IO events, signal or timeout. If <block> is false then returns
	 * immediately even if no event got reported, otherwise waits until the timeout.
	 * To block indefinitely set the timeout value to -1 (default), a timeout value
	 * of zero (0) will result in non-blocking operation in all the cases.
	 */
	void monitor(bool block);
	//Processes the ready watchers
	void dispatch() noexcept;
	//Adds the watcher back into the ready list
	void retain(Watcher *w) noexcept;
	//Returns true if Reactor::monitor got interrupted by a signal
	bool interrupted() const noexcept;
	//Returns true if Reactor::monitor got timed out
	bool timedOut() const noexcept;
	//Returns monitor's timeout value
	int getTimeout() const noexcept;
	//Sets monitor's timeout value
	void setTimeout(int milliseconds) noexcept;
private:
	/*
	 * Customizes the Watcher <w> and the associated records before
	 * adding the Watcher to the reactor's event loop.
	 */
	virtual void adapt(Watcher *w) = 0;
	/*
	 * Processes the IO events reported on the Watcher <w>. Should return
	 * <false> if further processing isn't needed, <true> otherwise.
	 */
	virtual bool react(Watcher *w) noexcept = 0;
	/*
	 * Cleans up the Watcher <w> and the associated data structures after
	 * it's removal from the reactor's event loop.
	 */
	virtual void stop(Watcher *w) noexcept = 0;
private:
	//Release the next watcher from the ready list
	Watcher* release() noexcept;
	//Remove a watcher (watcher will no longer be monitored)
	void remove(Watcher *w) noexcept;
private:
	//Selector timeout in milliseconds (default value: -1)
	int timeout;
	//The selector (event demultiplexer)
	Selector selector;
	//List of watchers waiting for processing
	Array<Watcher*> readyList;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_REACTOR_H_ */
