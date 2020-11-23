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
#if OPENSSL_VERSION_NUMBER < 0x10100000L
		SSL_library_init();
#else
		OPENSSL_init_ssl(0, nullptr);
#endif
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();
		//OPENSSL_config(NULL);
#if defined(OPENSSL_THREADS) && OPENSSL_VERSION_NUMBER < 0x10100000L
		locksSetup();
#endif
	}

	~initializer() {
#if defined(OPENSSL_THREADS) && OPENSSL_VERSION_NUMBER < 0x10100000L
		locksCleanup();
#endif
		CRYPTO_cleanup_all_ex_data();
		ERR_free_strings();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
		ERR_remove_thread_state(0);
#endif
		EVP_cleanup();
	}
#if defined(OPENSSL_THREADS) && OPENSSL_VERSION_NUMBER < 0x10100000L
	static unsigned long threadIdFunction() noexcept {
		return (unsigned long) pthread_self();
	}

	static void lockingFunction(int mode, int type, const char *file,
			int line) noexcept {
		if (mode & CRYPTO_LOCK) {
			pthread_mutex_lock(&(mutexes[type]));
		} else {
			pthread_mutex_unlock(&(mutexes[type]));
		}
	}

	bool locksSetup(void) noexcept {
		if (!(mutexes = (pthread_mutex_t *) OPENSSL_malloc(
								CRYPTO_num_locks() * sizeof(pthread_mutex_t)))) {
			return false;
		}

		for (int i = 0; i < CRYPTO_num_locks(); i++) {
			pthread_mutex_init(&mutexes[i], nullptr);
		}
		CRYPTO_set_id_callback(threadIdFunction);
		CRYPTO_set_locking_callback(lockingFunction);
		return true;
	}

	bool locksCleanup(void) noexcept {
		if (!mutexes) {
			return false;
		}

		CRYPTO_set_id_callback(nullptr);
		CRYPTO_set_locking_callback(nullptr);
		for (int i = 0; i < CRYPTO_num_locks(); i++) {
			pthread_mutex_destroy(&mutexes[i]);
		}

		OPENSSL_free(mutexes);
		mutexes = nullptr;
		return true;
	}
private:
	//All the mutexes available to OpenSSL
	static pthread_mutex_t *mutexes;
#endif
};
#if defined(OPENSSL_THREADS) && OPENSSL_VERSION_NUMBER < 0x10100000L
pthread_mutex_t *CryptoUtils::initializer::mutexes = nullptr;
#endif
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
