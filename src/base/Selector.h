/**
 * @file Selector.h
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
#include "common/NonCopyable.h"
#include "ds/Buffer.h"
#include <sys/epoll.h>

/*! @namespace wanhive */
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
 * IO multiplexer: monitors multiple file descriptors for IO events.
 * @note Provides abstraction of Linux's epoll(7) mechanism.
 */
class Selector: private NonCopyable {
public:
	/**
	 * Constructor: creates a new selector.
	 */
	Selector() noexcept;
	/**
	 * Constructor: creates and initializes a new selector.
	 * @param events maximum number of IO events to report
	 * @param signal true for signal safety, false for default behavior
	 */
	Selector(unsigned int events, bool signal);
	/**
	 * Destructor
	 */
	~Selector();
	/**
	 * Initializes the object (performs clean up if the object was previously
	 * initialized).
	 * @param events maximum number of IO events to report
	 * @param signal true true for signal safety, false for default behavior
	 */
	void initialize(unsigned int events, bool signal);
	//-----------------------------------------------------------------
	/**
	 * Adds a new file descriptor to the interest list.
	 * @param fd file descriptor
	 * @param events events of interest
	 * @param handle user data
	 */
	void add(int fd, uint32_t events, void *handle = nullptr);
	/**
	 * Modifies a file descriptor's settings in the interest list.
	 * @param fd file descriptor
	 * @param events new events of interest
	 * @param handle user data
	 */
	void modify(int fd, uint32_t events, void *handle = nullptr);
	/**
	 * Removes a file descriptor from the interest list.
	 * @param fd file descriptor
	 */
	void remove(int fd);
	//-----------------------------------------------------------------
	/**
	 * Waits for IO events, signal delivery, or timeout.
	 * @param timeout wait period in milliseconds. Set to -1 (default) to block
	 * indefinitely, set to zero (0) for non-blocking operation.
	 * @return number of ready file descriptors, possibly zero (0) if the call
	 * got interrupted by signal delivery or timeout expired.
	 */
	unsigned int select(int timeout = -1);
	/**
	 * Checks whether the selector got timed out.
	 * @return true if timeout expired, false otherwise.
	 */
	bool expired() const noexcept;
	/**
	 * Checks whether the selector got interrupted by a signal.
	 * @return true if interrupted, false otherwise.
	 */
	bool interrupted() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the next event description.
	 * @return event description (nullptr if none available)
	 */
	const SelectionEvent* next() noexcept;
	/**
	 * Returns the user data associated with an event description.
	 * @param se event description
	 * @return user data
	 */
	static void* attachment(const SelectionEvent *se) noexcept;
	/**
	 * Returns the IO events associated with an event description.
	 * @param se event description
	 * @return bitmap of events
	 */
	static uint32_t events(const SelectionEvent *se) noexcept;
private:
	void create();
	int close() noexcept;
private:
	sigset_t signals;
	sigset_t *mask { };
	int epfd { -1 };
	Buffer<SelectionEvent> selected;
	bool _interrupted { };
	bool _expired { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SELECTOR_H_ */
