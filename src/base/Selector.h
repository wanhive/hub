/*
 * Selector.h
 *
 * Linux's epoll based IO multiplexer
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
enum SelectorEvent {
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
enum SelectorFlag {
	TRIGGER_EDGE = EPOLLET, TRIGGER_ONCE = EPOLLONESHOT
};
//-----------------------------------------------------------------
/**
 * Abstraction of Linux epoll(7) mechanism
 * Thread safe at class level
 */
class Selector {
public:
	//Doesn't initialize the object, call Selector::initialize
	Selector() noexcept;
	/*
	 * <maxEvents> is the maximum number of events to be reported in each cycle.
	 * If <signal> is true then the selector handles asynchronous signal delivery
	 * atomically, i.e. the selector can be safely interrupted by signal.
	 */
	Selector(unsigned int maxEvents, bool signal);
	~Selector();
	/*
	 * Initialize the selector, <maxEvents> is the maximum number of IO events
	 * to be reported in each cycle. If <signal> is true then the selector can
	 * be safely interrupted by asynchronously delivered signal.
	 */
	void initialize(unsigned int maxEvents, bool signal);
	/*
	 * Start monitoring the file descriptor <fd> for <events> and preserve
	 * the <handle> for future reference
	 */
	void add(int fd, uint32_t events, void *handle = nullptr);
	/*
	 * Modify the <events> and <handle> associated with the file descriptor <fd>
	 */
	void modify(int fd, uint32_t events, void *handle = nullptr);
	/*
	 * Remove the file descriptor <fd> from the Selector
	 */
	void remove(int fd);
	/*
	 * Waits for an IO event, timeout or a signal, returns the number of ready
	 * file descriptors (possibly zero). <timeout> is specified in milliseconds,
	 * to block indefinitely set this value to -1, to return immediately set
	 * this value to zero (0).
	 */
	unsigned int select(int timeout = -1);

	//Returns true if Selector::select got interrupted by a signal
	bool interrupted() const noexcept;
	//Returns true if Selector::select got timed out
	bool timedOut() const noexcept;
	//Returns data associated with the next ready file descriptor
	SelectionEvent* next() noexcept;
	//Returns the handle associated with a ready file descriptor
	static void* attachment(const SelectionEvent *se) noexcept;
	//Returns the IO events reported on the associated file descriptor
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
