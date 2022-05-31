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
//Treat this as an opaque object, use the methods provided by Selector
using SelectionEvent=epoll_event;
//Events listed in the epoll man page
enum SelectorEvent : uint32_t {
	IO_READ = EPOLLIN, IO_WRITE = EPOLLOUT, IO_WR = (EPOLLIN | EPOLLOUT),
	//Hang-up happened, can't write to the descriptor
	IO_HUP = EPOLLHUP,
	//Remote peer closed the socket or shut down the writing half of the socket
	IO_RDHUP = EPOLLRDHUP,
	IO_CLOSE = (EPOLLHUP | EPOLLRDHUP),
	IO_PRIORITY = EPOLLPRI,
	IO_ERROR = EPOLLERR,
	IO_ALL = (IO_WR | IO_CLOSE | IO_PRIORITY | IO_ERROR)
};

//Trigger mechanism
enum SelectorFlag : uint32_t {
	TRIGGER_EDGE = EPOLLET, TRIGGER_ONCE = EPOLLONESHOT
};
//-----------------------------------------------------------------
/**
 * Abstraction of Linux epoll(7) mechanism
 * Thread safe at class level
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
	 * @param signal true for atomic handling of asynchronous signal delivery i.e.
	 * Selector::select()can be safely interrupted by signal, false for default
	 * behavior (signal delivery may not get noticed).
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
	 * @param signal true for atomic handling of asynchronous signal delivery i.e.
	 * Selector::select()can be safely interrupted by signal, false for default
	 * behavior (signal delivery may not get noticed).
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
	 * @param timeout the wait timeout value in milliseconds, set -1 to block
	 * indefinitely, set 0 to return immediately even if no events are available.
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
	 * Returns information about the next ready file descriptor that has some
	 * events available.
	 * @return object describing a ready file descriptor
	 */
	SelectionEvent* next() noexcept;
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
