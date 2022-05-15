/*
 * Handler.h
 *
 * Generic handler interface
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_REACTOR_HANDLER_H_
#define WH_REACTOR_HANDLER_H_

namespace wanhive {
/**
 * The generic data handler/processor interface.
 */
template<typename T> class Handler {
public:
	/**
	 * The virtual destructor.
	 */
	virtual ~Handler() = default;

	/**
	 * Handles (processes) the given object.
	 * @param t describes the object to handle/process
	 * @return true on success, false otherwise
	 */
	virtual bool handle(T *t) noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_HANDLER_H_ */
