/*
 * Handle.h
 *
 * File handle
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_REACTOR_HANDLE_H_
#define WH_REACTOR_HANDLE_H_

namespace wanhive {
/**
 * File handle
 * Selectively exposes file descriptor to a module
 */
template<typename T> class Handle {
public:
	/**
	 * Constructor: assigns a file descriptor to this object.
	 * @param fd file descriptor's value
	 */
	Handle(int fd) noexcept :
			fd { fd } {
	}
	/**
	 * Destructor
	 */
	~Handle() = default;
private:
	/**
	 * Returns the associated file descriptor.
	 * @return the file descriptor
	 */
	operator int() const noexcept {
		return fd;
	}
private:
	int fd;
	friend T;
};

class Reactor;
using WatcherHandle=Handle<Reactor>;

} /* namespace wanhive */

#endif /* WH_REACTOR_HANDLE_H_ */
