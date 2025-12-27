/*
 * X25519.h
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
	//-----------------------------------------------------------------
	/**
	 * Computes the shared secret key.
	 * @return true on success, false on error
	 */
	bool compute() noexcept;
	/**
	 * Returns the key and it's length in bytes.
	 * @param length stores key's length in bytes
	 * @return shared key
	 */
	const unsigned char* get(size_t &length) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates PEM encoded key pair.
	 * @param privateKeyFile private key file's path
	 * @param publicKeyFile public key file's path
	 * @param secret private key's pass phrase
	 * @return true on success, false on error
	 */
	bool generate(const char *privateKeyFile, const char *publicKeyFile,
			char *secret = nullptr) noexcept;
private:
	unsigned char* resize(size_t size) noexcept;
	void clear() noexcept;
public:
	/*! Shared key size in bytes */
	static constexpr unsigned int KEY_SIZE = 32;
private:
	unsigned char *data { };
	size_t bytes { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_X25519_H_ */
