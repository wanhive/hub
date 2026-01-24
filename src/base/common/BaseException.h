/*
 * BaseException.h
 *
 * Base class for exceptions
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_BASEEXCEPTION_H_
#define WH_BASE_COMMON_BASEEXCEPTION_H_

namespace wanhive {
/**
 * Base class for exceptions
 */
class BaseException {
public:
	/**
	 * Virtual destructor
	 */
	virtual ~BaseException() = default;
	/**
	 * Returns a string that describes the error/exception.
	 * @return error description string
	 */
	virtual const char* what() const noexcept = 0;
	/**
	 * Returns an error code.
	 * @return error code
	 */
	virtual int errorCode() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_BASEEXCEPTION_H_ */
