/*
 * @file MLKem.h
 *
 * ML-KEM cipher
 *
 *
 * Copyright (C) 2026 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_MLKEM_H_
#define WH_BASE_SECURITY_MLKEM_H_
#include "KeyPair.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Standardized parameters
 */
enum MLKemParam {
	ML_KEM_512, /**< Level 1 */
	ML_KEM_768, /**< Level 3 */
	ML_KEM_1024 /**< Level 5 */
};

/**
 * ML-KEM cipher
 */
class MLKem: public KeyPair {
public:
	/**
	 * Constructor: creates an empty object (no key pair).
	 */
	MLKem(MLKemParam param = ML_KEM_512) noexcept;
	/**
	 * Destructor
	 */
	~MLKem();
	//-----------------------------------------------------------------
	/**
	 * Performs a public key encapsulation operation.
	 * @param key shared secret output buffer
	 * @param keyLength shared secret buffer size in bytes as input and
	 * generated secret's size in bytes as output (value-result argument).
	 * @param wrapped encapsulated key output buffer, set to nullptr to
	 * calculate the maximum buffer sizes.
	 * @param wrappedLength encapsulated key buffer size in bytes as input and
	 * encapsulated data's size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool encapsulate(unsigned char *key, size_t &keyLength,
			unsigned char *wrapped, size_t &wrappedLength) const noexcept;
	/**
	 * Performs a private key de-capsulation operation.
	 * @param wrapped encapsulated key
	 * @param wrappedLength encapsulated key's length
	 * @param key shared secret output buffer, set to nullptr to calculate
	 * the maximum buffer size.
	 * @param keyLength shared secret buffer size in bytes as input and
	 * generated secret's size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool decapsulate(const unsigned char *wrapped, size_t wrappedLength,
			unsigned char *key, size_t &keyLength) const noexcept;
	//-----------------------------------------------------------------
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
	/*! Shared secret output size in bytes */
	static constexpr unsigned int SECRET_SIZE = 32;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_MLKEM_H_ */
