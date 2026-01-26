/**
 * @file X25519.h
 *
 * X25519 key exchange
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_X25519_H_
#define WH_BASE_SECURITY_X25519_H_
#include "KeyPair.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * X25519 key exchange
 */
class X25519: public KeyPair {
public:
	/**
	 * Constructor: creates an empty object (no key pair).
	 */
	X25519() noexcept;
	/**
	 * Destructor
	 */
	~X25519();
	/**
	 * Computes the shared secret key.
	 * @param data output buffer for the shared key. Set to nullptr to calculate
	 * the maximum buffer size.
	 * @param bytes buffer size in bytes as input and shared key's size in bytes
	 * as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool compute(unsigned char *data, unsigned int &bytes) noexcept;
	/**
	 * Generates PEM encoded key pair.
	 * @param privateKey private key file's path
	 * @param publicKey public key file's path
	 * @param secret private key's pass phrase
	 * @return true on success, false on error
	 */
	bool generate(const char *privateKey, const char *publicKey, char *secret =
			nullptr) noexcept;
public:
	/*! Key size in bytes */
	static constexpr unsigned int KEY_SIZE = 32;
	/*! Shared secret key size in bytes */
	static constexpr unsigned int SECRET_SIZE = 32;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_X25519_H_ */
