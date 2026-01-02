/*
 * Ed25519.h
 *
 * Ed25519 cipher
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_ED25519_H_
#define WH_BASE_SECURITY_ED25519_H_
#include "KeyPair.h"

namespace wanhive {
/**
 * Ed25519 digital signature facility
 */
class Ed25519: public KeyPair {
public:
	/**
	 * Constructor: creates an empty object (no key pair).
	 */
	Ed25519() noexcept;
	/**
	 * Destructor
	 */
	~Ed25519();
	//-----------------------------------------------------------------
	/**
	 * Signs the given data.
	 * @param data data for signing
	 * @param dataLength data's size in bytes
	 * @param signature digital signature output buffer. Set to nullptr to
	 * calculate the maximum buffer size.
	 * @param signatureLength buffer size in bytes as input and signature's
	 * size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool sign(const unsigned char *data, unsigned int dataLength,
			unsigned char *signature, unsigned int *signatureLength) noexcept;
	/**
	 * Verifies the given data.
	 * @param verified data
	 * @param dataLength data's size in bytes
	 * @param signature digital signature
	 * @param signatureLength signature's size in bytes
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const unsigned char *data, unsigned int dataLength,
			const unsigned char *signature,
			unsigned int signatureLength) noexcept;
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
private:
	EVP_MD_CTX* mdContext() noexcept;
public:
	/*! Private and public key sizes in bytes */
	static constexpr unsigned int KEY_SIZE = 32;
	/*! Signature size in bytes */
	static constexpr unsigned int SIGNATURE_SIZE = 64;
private:
	EVP_MD_CTX *mdctx { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_ED25519_H_ */
