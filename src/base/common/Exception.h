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
enum ExceptionType {
	EX_NULL,
	EX_INDEXOUTOFBOUNDS,
	EX_INVALIDPARAM,
	EX_ALLOCFAILED,
	EX_UNDERFLOW,
	EX_OVERFLOW,
	EX_INVALIDRANGE,
	EX_INVALIDOPERATION,
	EX_INVALIDSTATE,
	EX_RESOURCE,
	EX_SECURITY
};
//-----------------------------------------------------------------
/**
 * Application generated exceptions
 */
class Exception: public BaseException {
public:
	Exception(ExceptionType type) noexcept;
	virtual ~Exception() override;
	const char* what() const noexcept override;
	int errorCode() const noexcept override;
private:
	int type;
	static const char *exceptionMessages[];
};

} /* namespace wanhive */
#endif /* WH_BASE_COMMON_EXCEPTION_H_ */
