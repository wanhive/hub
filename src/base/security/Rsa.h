/*
 * Rsa.h
 *
 * RSA cipher
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_RSA_H_
#define WH_BASE_SECURITY_RSA_H_
#include "KeyPair.h"

namespace wanhive {
/**
 * RSA public key cryptography
 */
class Rsa: public KeyPair {
public:
	/**
	 * Constructor: creates an empty object (no key pair).
	 */
	Rsa() noexcept;
	/**
	 * Destructor
	 */
	~Rsa();
	//-----------------------------------------------------------------
	/**
	 * Encrypts the given plain text. Uses RSA_PKCS1_OAEP_PADDING (EME-OAEP as
	 * defined in PKCS #1 v2.0).
	 * @param data input data
	 * @param dataLength input data's size in bytes
	 * @param encrypted encrypted data output buffer. Set to nullptr to
	 * calculate the maximum buffer size.
	 * @param encryptedLength buffer size in bytes as input and encrypted data's
	 * size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool encrypt(const unsigned char *data, unsigned int dataLength,
			unsigned char *encrypted,
			unsigned int *encryptedLength) const noexcept;
	/**
	 * Decrypts the given encrypted data. Uses RSA_PKCS1_OAEP_PADDING (EME-OAEP
	 * as defined in PKCS #1 v2.0).
	 * @param data encrypted data
	 * @param dataLength encrypted data's size in bytes
	 * @param decrypted plain text output buffer. Set to nullptr to calculate
	 * the maximum buffer size.
	 * @param decryptedLength buffer size in bytes as input and plain text's
	 * size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool decrypt(const unsigned char *data, unsigned int dataLength,
			unsigned char *decrypted,
			unsigned int *decryptedLength) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Signs (PKCS #1 v2.0 SHA-1) the given data.
	 * @param data data for signing
	 * @param dataLength data's size in bytes
	 * @param signature digital signature output buffer. Set to nullptr to
	 * calculate the maximum buffer size.
	 * @param signatureLength buffer size in bytes as input and signature's
	 * size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool sign(const unsigned char *data, unsigned int dataLength,
			unsigned char *signature,
			unsigned int *signatureLength) const noexcept;
	/**
	 * Verifies (PKCS #1 v2.0 SHA-1) the given data.
	 * @param data verified data
	 * @param dataLength data's size in bytes
	 * @param signature digital signature
	 * @param signatureLength digital signature's size in bytes
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const unsigned char *data, unsigned int dataLength,
			const unsigned char *signature,
			unsigned int signatureLength) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates PEM encoded key pair.
	 * @param privateKeyFile private key file's path
	 * @param publicKeyFile public key file's path
	 * @param bits key size in bits
	 * @param secret private key's pass phrase
	 * @return true on success, false on error
	 */
	bool generate(const char *privateKeyFile, const char *publicKeyFile,
			int bits, char *secret = nullptr) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_RSA_H_ */
