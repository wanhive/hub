/*
 * Exception.cpp
 *
 * Application generated exceptions
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Exception.h"

namespace {

const char *exceptionMessages[] = { "Null reference", "Index out of bounds",
		"Invalid parameter(s)", "Memory allocation failed",
		"Container underflow", "Container overflow", "Invalid range",
		"Invalid operation", "Invalid state", "Resource unavailable",
		"Security violation" };

}  // namespace

namespace wanhive {

Exception::Exception(ExceptionType type) noexcept {
	this->type = type;
}

Exception::~Exception() {

}

const char* Exception::what() const noexcept {
	return exceptionMessages[type];
}

int Exception::errorCode() const noexcept {
	return this->type;
}

} /* namespace wanhive */
