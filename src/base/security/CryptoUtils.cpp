/*
 * CryptoUtils.cpp
 *
 * Cipher initialization and error handling routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "CryptoUtils.h"
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

namespace wanhive {

class CryptoUtils::initializer {
public:
	initializer() noexcept {
		OPENSSL_init_ssl(0, nullptr);
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
	}

	~initializer() {
		CRYPTO_cleanup_all_ex_data();
		ERR_free_strings();
		EVP_cleanup();
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
	ERR_error_string_n(error, buffer, length);
}

void CryptoUtils::clearErrors() noexcept {
	ERR_clear_error();
}

} /* namespace wanhive */
