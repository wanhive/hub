/*
 * Selector.h
 *
 * IO multiplexer
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SELECTOR_H_
#define WH_BASE_SELECTOR_H_
#include "ds/Buffer.h"
#include <signal.h>
#include <sys/epoll.h>

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Event description: treat this as an opaque object. Use methods provided by
 * the Selector to fetch additional information.
 */
using SelectionEvent = epoll_event;

/**
 * Supported IO events
 */
enum SelectorEvent : uint32_t {
	IO_READ = EPOLLIN, /**< Read event */
	IO_WRITE = EPOLLOUT, /**< Write event */
	IO_WR = (EPOLLIN | EPOLLOUT), /**< Read/Write event */
	IO_HUP = EPOLLHUP, /**< Hang-up happened, can't write to descriptor */
	IO_RDHUP = EPOLLRDHUP, /**< Remote closed or shutdown the writing half */
	IO_CLOSE = (EPOLLHUP | EPOLLRDHUP), /**< Close event */
	IO_PRIORITY = EPOLLPRI, /**< Exceptional condition */
	IO_ERROR = EPOLLERR, /**< Error condition */
	IO_ALL = (IO_WR | IO_CLOSE | IO_PRIORITY | IO_ERROR)/**< All events */
};

/**
 * Supported trigger mechanisms
 */
enum SelectorFlag : uint32_t {
	TRIGGER_EDGE = EPOLLET, /**< Edge trigger */
	TRIGGER_ONCE = EPOLLONESHOT/**< One-shot notification */
};
//-----------------------------------------------------------------
/**
 * IO multiplexer: monitors multiple file descriptors to see if any one of them
 * is ready for performing an IO operation.
 * @note Provides abstraction of Linux's epoll(7) mechanism.
 */
class Selector {
public:
	/**
	 * Default constructor: creates an uninitialized selector. To initialize the
	 * object call Selector::initialize() explicitly.
	 */
	Selector() noexcept;
	/**
	 * Constructor: creates and initializes a new selector.
	 * @param maxEvents maximum number of events to return (should be greater
	 * than zero) by Selector::select().
	 * @param signal true to safely wait (see Selector::select()) until a signal
	 * is caught, false for default behavior (no signal safety).
	 */
	Selector(unsigned int maxEvents, bool signal);
	/**
	 * Destructor
	 */
	~Selector();
	/**
	 * Initializes the object (performs clean up if the object was previously
	 * initialized).
	 * @param maxEvents the maximum number of events to report in each call to
	 * Selector::select().
	 * @param signal true to safely wait (see Selector::select()) until a signal
	 * is caught, false for default behavior (no signal safety).
	 */
	void initialize(unsigned int maxEvents, bool signal);
	//-----------------------------------------------------------------
	/**
	 * Adds a new file descriptor to the interest list.
	 * @param fd file descriptor to monitor
	 * @param events events of interest (see Selector::events())
	 * @param handle pointer to the user data (see Selector::attachment())
	 */
	void add(int fd, uint32_t events, void *handle = nullptr);
	/**
	 * Modifies the settings for a file descriptor in the interest list.
	 * @param fd file descriptor being monitored (see Selector::add())
	 * @param events new events of interest (see Selector::events())
	 * @param handle user data's pointer (see Selector::attachment())
	 */
	void modify(int fd, uint32_t events, void *handle = nullptr);
	/**
	 * Removes a file descriptor from the interest list.
	 * @param fd file descriptor being monitored (see Selector::add())
	 */
	void remove(int fd);
	//-----------------------------------------------------------------
	/**
	 * Waits for IO events, timeout, or signal delivery.
	 * @param timeout wait period in milliseconds (set -1 to block indefinitely,
	 * 0 to return immediately even if no events are available).
	 * @return number of ready file descriptors (see Selector::next()), possibly
	 * zero if the call timed-out, or got interrupted by signal delivery.
	 */
	unsigned int select(int timeout = -1);
	/**
	 * Checks whether the selector got interrupted by a signal.
	 * @return true if the most recent Selector::select() call got interrupted
	 * by a signal, false otherwise.
	 */
	bool interrupted() const noexcept;
	/**
	 * Checks whether the selector got timed out while waiting for IO events.
	 * @return true if the most recent Selector::select() call got timed out,
	 * false otherwise.
	 */
	bool timedOut() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the next event description.
	 * @return pointer to an object describing the next event
	 */
	const SelectionEvent* next() noexcept;
	/**
	 * Returns user data associated with a ready file descriptor.
	 * @param se event description (see Selector::next())
	 * @return user data's pointer
	 */
	static void* attachment(const SelectionEvent *se) noexcept;
	/**
	 * Returns the events reported on a ready file descriptor.
	 * @param se event description (see Selector::next())
	 * @return events mask
	 */
	static uint32_t events(const SelectionEvent *se) noexcept;
private:
	void create();
	int close() noexcept;
private:
	sigset_t signals;
	sigset_t *mask { nullptr };
	int epfd { -1 };
	Buffer<SelectionEvent> selected;
	bool _interrupted { false };
	bool _timedOut { false };
};

} /* namespace wanhive */

#endif /* WH_BASE_SELECTOR_H_ */
