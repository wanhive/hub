/*
 * Exception.h
 *
 * Application generated exceptions
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_EXCEPTION_H_
#define WH_BASE_COMMON_EXCEPTION_H_
#include "BaseException.h"

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Enumeration of application-generated exception types
 */
enum ExceptionType {
	EX_NULL, /**< Null reference */
	EX_INDEX, /**< Invalid index */
	EX_ARGUMENT, /**< Invalid argument */
	EX_MEMORY, /**< Memory operation failed */
	EX_UNDERFLOW, /**< Container underflow */
	EX_OVERFLOW, /**< Container overflow */
	EX_RANGE, /**< Invalid range/bounds */
	EX_OPERATION, /**< Invalid operation */
	EX_STATE, /**< Invalid application state */
	EX_RESOURCE, /**< Invalid resource */
	EX_SECURITY /**< Security constraints violation */
};
//-----------------------------------------------------------------
/**
 * Application generated exception
 */
class Exception final: public BaseException {
public:
	/**
	 * Constructor: creates a new exception.
	 * @param type exception type
	 */
	Exception(ExceptionType type) noexcept;
	/**
	 * Destructor
	 */
	~Exception();
	const char* what() const noexcept override;
	int errorCode() const noexcept override;
private:
	int type;
};

} /* namespace wanhive */
#endif /* WH_BASE_COMMON_EXCEPTION_H_ */
