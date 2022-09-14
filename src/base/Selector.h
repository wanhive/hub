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
 * Treat this as an opaque object, use methods provided by the Selector to
 * fetch additional information.
 */
using SelectionEvent=epoll_event;

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
	 * Default constructor: call Selector::initialize() for initialization.
	 */
	Selector() noexcept;
	/**
	 * Constructor: initializes the object.
	 * @param maxEvents the maximum number of events to report in each call to
	 * Selector::select().
	 * @param signal true to safely wait (see Selector::select()) until a signal
	 * is caught, false for the default behavior (no signal safety).
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
	 * is caught, false for the default behavior (no signal safety).
	 */
	void initialize(unsigned int maxEvents, bool signal);
	//-----------------------------------------------------------------
	/**
	 * Starts monitoring a file descriptor for the given events
	 * @param fd file descriptor to monitor
	 * @param events events of interest (see Selector::events())
	 * @param handle pointer to the user data (see Selector::attachment())
	 */
	void add(int fd, uint32_t events, void *handle = nullptr);
	/**
	 * Modify the events and reference associated with the given file descriptor.
	 * @param fd file descriptor being monitored (see Selector::add())
	 * @param events new events of interest (see Selector::events())
	 * @param handle pointer to the user data (see Selector::attachment())
	 */
	void modify(int fd, uint32_t events, void *handle = nullptr);
	/**
	 * Stops monitoring of the given file descriptor
	 * @param fd file descriptor being monitored (see Selector::add())
	 */
	void remove(int fd);
	//-----------------------------------------------------------------
	/**
	 * Waits for IO events, timeout or signal.
	 * @param timeout the wait period in milliseconds (-1 to block indefinitely,
	 * 0 to return immediately even if no events are available).
	 * @return number of ready file descriptors (see Selector::next()), possibly
	 * zero if the call got interrupted due to timeout or signal delivery.
	 */
	unsigned int select(int timeout = -1);
	/**
	 * Checks if the selector got interrupted by signal delivery.
	 * @return true if the most recent call to Selector::select() got interrupted
	 * by a signal, false otherwise.
	 */
	bool interrupted() const noexcept;
	/**
	 * Checks if the selector got timed out while waiting for IO events.
	 * @return Returns true if the most recent call to Selector::select() got
	 * timed out, false otherwise.
	 */
	bool timedOut() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the next event.
	 * @return pointer to object describing the next event
	 */
	const SelectionEvent* next() noexcept;
	/**
	 * Returns pointer to the user data associated with a ready file descriptor.
	 * @param se object describing a ready file descriptor (see Selector::next())
	 * @return pointer value associated with the ready file descriptor
	 */
	static void* attachment(const SelectionEvent *se) noexcept;
	/**
	 * Returns the IO events reported on a ready file descriptor.
	 * @param se object describing a ready file descriptor (see Selector::next())
	 * @return IO events reported on the file descriptor
	 */
	static uint32_t events(const SelectionEvent *se) noexcept;
private:
	void create();
	int close() noexcept;
private:
	sigset_t signals;
	sigset_t *mask;
	int epfd;
	Buffer<SelectionEvent> selected;
	bool _interrupted;
	bool _timedOut;
};

} /* namespace wanhive */

#endif /* WH_BASE_SELECTOR_H_ */
