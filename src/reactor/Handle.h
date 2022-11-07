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
#include "../base/common/reflect.h"

namespace wanhive {
/**
 * Generic handle that selectively exposes a value to a module.
 * @tparam F friend class' name
 * @tparam V data type
 */
template<typename F, typename V = int> class Handle {
public:
	/**
	 * Constructor: assigns a value to this object.
	 * @param v the value
	 */
	Handle(const V &data) noexcept :
			data { data } {

	}
	/**
	 * Destructor
	 */
	~Handle() = default;
private:
	/**
	 * Returns the associated value.
	 * @return the value
	 */
	const V& value() const noexcept {
		return data;
	}
private:
	WH_POD_ASSERT(V);
	V data;
	friend F;
};

/**
 * File handle that selectively exposes file descriptor to a module.
 * @tparam F friend class' name
 */
template<typename F> class Handle<F, int> {
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
	friend F;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_HANDLE_H_ */
