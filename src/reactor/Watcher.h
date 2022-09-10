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
#include "Handle.h"

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
	WATCHER_MULTICAST = 64 /**< Multicasting enabled */
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
	 * Default constructor: doesn't assign valid file descriptor
	 */
	Watcher() noexcept;
	/**
	 * Constructor: assigns a file descriptor
	 * @param fd the file descriptor to associate with this object
	 */
	Watcher(int fd) noexcept;
	/**
	 * Destructor
	 */
	virtual ~Watcher();
	//-----------------------------------------------------------------
	/**
	 * Arms the watcher, it usually involves enabling and configuring the IO
	 * capabilities of the watcher, in most case this one will be a no-op.
	 */
	virtual void start()=0;
	/**
	 * Disarms the watcher, it usually involves temporarily suspending the IO
	 * capabilities of the watcher, in most case this one will be a no-op.
	 */
	virtual void stop() noexcept = 0;
	/**
	 * The callback function for processing the pending IO events.
	 * @param arg a generic argument
	 * @return true if IO events are still pending, false otherwise
	 */
	virtual bool callback(void *arg) noexcept = 0;
	/**
	 * Publishes something to this watcher.
	 * @param arg a generic pointer to an object
	 * @return true if the operation was successful, false otherwise
	 */
	virtual bool publish(void *arg) noexcept = 0;
	//-----------------------------------------------------------------
	/**
	 * Multicasting: adds subscription to the given topic.
	 * @param index the topic identifier
	 */
	virtual void setTopic(unsigned int index) noexcept;
	/**
	 * Multicasting: removes subscription from the given topic.
	 * @param index the topic identifier
	 */
	virtual void clearTopic(unsigned int index) noexcept;
	/**
	 * Multicasting: tests subscription to a topic.
	 * @param index the topic identifier
	 * @return true if the topic is subscribed, false otherwise
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
	 * Checks whether the watcher has pending jobs (see Descriptor::isReady()).
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
