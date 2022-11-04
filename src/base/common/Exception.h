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
 * Enumeration of error codes for application-generated exception
 */
enum ExceptionType {
	EX_NULL, /**< Null reference */
	EX_INDEXOUTOFBOUNDS,/**< Invalid index */
	EX_INVALIDPARAM, /**< Invalid parameter */
	EX_ALLOCFAILED, /**< Memory allocation failed */
	EX_UNDERFLOW, /**< Container underflow */
	EX_OVERFLOW, /**< Container overflow */
	EX_INVALIDRANGE, /**< Invalid range/bounds */
	EX_INVALIDOPERATION,/**< Invalid operation */
	EX_INVALIDSTATE, /**< Invalid application state */
	EX_RESOURCE, /**< Invalid resource */
	EX_SECURITY /**< Security constraints violation */
};
//-----------------------------------------------------------------
/**
 * Application generated exceptions
 */
class Exception: public BaseException {
public:
	/**
	 * Constructor: creates a new exception.
	 * @param type the error code
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
