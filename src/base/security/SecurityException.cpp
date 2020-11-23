/*
 * SecurityException.cpp
 *
 * Exceptions and errors generated during cryptographic operations
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "SecurityException.h"
#include "CryptoUtils.h"

namespace wanhive {

SecurityException::SecurityException() noexcept {
	error = CryptoUtils::getError();
}

SecurityException::~SecurityException() {

}

const char* SecurityException::what() const noexcept {
	CryptoUtils::getErrorMessage(error, (char*) buffer, MSG_LEN);
	return buffer;
}

int SecurityException::errorCode() const noexcept {
	return error;
}

} /* namespace wanhive */
