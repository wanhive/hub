/*
 * Watcher.h
 *
 * Resource descriptor and request handler
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_REACTOR_WATCHER_H_
#define WH_REACTOR_WATCHER_H_
#include "Descriptor.h"
#include "../base/ds/Handle.h"

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Watcher flags
 */
enum WatcherFlag : uint32_t {
	WATCHER_RUNNING = 1, /**< Used by reactor */
	WATCHER_INVALID = 2, /**< Used by reactor */
	WATCHER_READY = 4, /**< Used by reactor */
	WATCHER_ACTIVE = 8, /**< Authorization status */
	WATCHER_IN = 16, /**< Holds incoming data */
	WATCHER_OUT = 32, /**< Holds outgoing data */
	WATCHER_MULTICAST = 64, /**< Multicasting enabled */
	WATCHER_CRITICAL = 128, /**< Critical component */
	WATCHER_FLAG1 = 256, /**< User defined flag-1 */
	WATCHER_FLAG2 = 512 /**< User defined flag-2 */
};
/**
 * Watcher configuration options
 */
enum WatcherOption {
	WATCHER_READ_BUFFER_MAX, /**< Read buffer's maximum size */
	WATCHER_WRITE_BUFFER_MAX /**< Write buffer's maximum size */
};
//-----------------------------------------------------------------
//Reactor-specific file handle
class Reactor;
using WatcherHandle=Handle<Reactor>;
//-----------------------------------------------------------------
/**
 * Reactor pattern implementation (resource descriptor and request handler)
 * @ref http://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf
 */
class Watcher: public Descriptor {
public:
	/**
	 * Constructor: creates a watcher.
	 */
	Watcher() noexcept;
	/**
	 * Constructor: creates a watcher and assigns a file descriptor.
	 * @param fd file descriptor
	 */
	Watcher(int fd) noexcept;
	/**
	 * Destructor: releases the managed resources.
	 */
	virtual ~Watcher();
	//-----------------------------------------------------------------
	/**
	 * Arms the watcher, it usually involves setting up and configuring the
	 * associated file descriptor.
	 */
	virtual void start() = 0;
	/**
	 * Disarms the watcher, it usually involves temporarily suspending the IO
	 * capabilities of the associated file descriptor.
	 */
	virtual void stop() noexcept = 0;
	/**
	 * Processes the pending IO events.
	 * @param arg additional argument
	 * @return true if further processing is required, false otherwise
	 */
	virtual bool callback(void *arg) noexcept = 0;
	/**
	 * Publishes something to this watcher.
	 * @param arg published object's pointer
	 * @return true on success, false on error
	 */
	virtual bool publish(void *arg) noexcept = 0;
	//-----------------------------------------------------------------
	/**
	 * Group communication: adds subscription to a given topic.
	 * @param index topic's identifier
	 */
	virtual void setTopic(unsigned int index) noexcept;
	/**
	 * Group communication: removes subscription from a given topic.
	 * @param index topic's identifier
	 */
	virtual void clearTopic(unsigned int index) noexcept;
	/**
	 * Group communication: tests subscription to a given topic.
	 * @param index topic's identifier
	 * @return true if a subscription exists, false otherwise
	 */
	virtual bool testTopic(unsigned int index) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Runtime configuration: returns a configurable value or limit.
	 * @param name option's name
	 * @return option's value
	 */
	virtual unsigned long long getOption(int name) const noexcept;
	/**
	 * Runtime configuration: sets a configurable value or limit.
	 * @param name option's name
	 * @param value option's value
	 */
	virtual void setOption(int name, unsigned long long value) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether the watcher has pending jobs.
	 * @return true if the watcher has pending jobs, false otherwise
	 */
	bool isReady() const noexcept;
	/**
	 * Returns a file handle to the reactor.
	 * @return the file handle
	 */
	WatcherHandle getHandle() const noexcept;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_WATCHER_H_ */
