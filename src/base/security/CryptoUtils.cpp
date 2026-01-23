/*
 * CryptoUtils.cpp
 *
 * OpenSSL initializer and error handler
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "CryptoUtils.h"
#include <openssl/err.h>

namespace wanhive {

class CryptoUtils::initializer {
public:
	initializer() noexcept {

	}

	~initializer() {

	}
};

const CryptoUtils::initializer CryptoUtils::_init;

unsigned long CryptoUtils::getError() noexcept {
	return ERR_get_error();
}

void CryptoUtils::getErrorMessage(char *buffer, unsigned int length) noexcept {
	getErrorMessage(getError(), buffer, length);
}

void CryptoUtils::getErrorMessage(unsigned long error, char *buffer,
		unsigned int length) noexcept {
	if (buffer && length) {
		ERR_error_string_n(error, buffer, length);
	}
}

void CryptoUtils::clearErrors() noexcept {
	ERR_clear_error();
}

} /* namespace wanhive */
