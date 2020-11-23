/*
 * BaseException.h
 *
 * Base class for all exceptions
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
 * Base class for all exceptions
 */
class BaseException {
public:
	virtual ~BaseException() = default;
	virtual const char* what() const noexcept = 0;
	virtual int errorCode() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_BASEEXCEPTION_H_ */
