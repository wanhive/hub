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

namespace wanhive {
enum WatcherFlag : uint32_t {
	//First three flags are modified by the Reactor (don't modify directly)
	WATCHER_RUNNING = 1,
	WATCHER_INVALID = 2,
	WATCHER_READY = 4,
	//Registration status
	WATCHER_ACTIVE = 8,
	//IO status flags
	WATCHER_IN = 16, //Holds incoming data
	WATCHER_OUT = 32, //Holds outgoing data
	//Watcher is a multicast address
	WATCHER_MULTICAST = 64
};

/**
 * Reactor pattern implementation (resource descriptor and request handler)
 * Ref: http://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf
 * Thread safe at class level
 */
class Watcher: public Descriptor {
public:
	Watcher() noexcept;
	Watcher(int fd) noexcept;
	virtual ~Watcher();
	//-----------------------------------------------------------------
	//Rearm the watcher
	virtual void start()=0;
	//Disarm the watcher (usually, shouldn't close the descriptor)
	virtual void stop() noexcept = 0;
	//Callback when an IO event is reported
	virtual bool callback(void *arg) noexcept = 0;
	//Publish something to this watcher
	virtual bool publish(void *arg) noexcept = 0;
	//-----------------------------------------------------------------
	/**
	 * Adapters for multicasting
	 */
	//Set subscription to the topic <index>
	virtual void setTopic(unsigned int index) noexcept;
	//Clear subscription to the topic <index>
	virtual void clearTopic(unsigned int index) noexcept;
	//Test whether the topic <index> is subscribed or not
	virtual bool testTopic(unsigned int index) const noexcept;
	//-----------------------------------------------------------------
	//Return true if the underlying descriptor can do some work
	bool isReady() const noexcept;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_WATCHER_H_ */
