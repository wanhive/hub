/*
 * SystemException.cpp
 *
 * Exceptions and errors generated by system calls
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "SystemException.h"
#include <cerrno>
#include <cstring>

namespace wanhive {

SystemException::SystemException() noexcept :
		error { errno } {
	errorMessage = getErrorMessage(error, buffer, sizeof(buffer));
}

SystemException::SystemException(int type) noexcept :
		error { type } {
	errorMessage = getErrorMessage(type, buffer, sizeof(buffer));
}

SystemException::~SystemException() {

}

const char* SystemException::what() const noexcept {
	return errorMessage;
}

int SystemException::errorCode() const noexcept {
	return this->error;
}

const char* SystemException::getErrorMessage(int error, char *buffer,
		unsigned int size) noexcept {
	/* Check for the XSI-compliant version */
#if (_POSIX_C_SOURCE >= 200112L) && !  _GNU_SOURCE
	if (strerror_r(error, buffer, size) == 0) {
		return buffer;
	} else {
		return "Unknown system error";
	}
#else
	return strerror_r(error, buffer, size);
#endif
}

} /* namespace wanhive */
