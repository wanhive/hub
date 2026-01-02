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
 * RSA asymmetric cipher
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
	 * Encrypts a plain-text. Uses RSA_PKCS1_OAEP_PADDING (EME-OAEP as defined
	 * in PKCS #1 v2.0).
	 * @param data plain-text input
	 * @param dataLength plain-text's size in bytes
	 * @param encrypted output buffer for cipher-text. Set to nullptr to
	 * calculate the maximum buffer size.
	 * @param encryptedLength buffer size in bytes as input and encrypted data's
	 * size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool encrypt(const unsigned char *data, unsigned int dataLength,
			unsigned char *encrypted,
			unsigned int *encryptedLength) const noexcept;
	/**
	 * Decrypts a cipher-text. Uses RSA_PKCS1_OAEP_PADDING (EME-OAEP as defined
	 * in PKCS #1 v2.0).
	 * @param data cipher-text input
	 * @param dataLength cipher-text's size in bytes
	 * @param decrypted output buffer for plain-text. Set to nullptr to
	 * calculate the maximum buffer size.
	 * @param decryptedLength buffer size in bytes as input and plain-text's
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
	 * @param signature output buffer for digital signature. Set to nullptr to
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
	 * @param data verifiable data
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
	 * @param privateKey private key file's path
	 * @param publicKey public key file's path
	 * @param bits key size in bits
	 * @param secret private key's pass phrase
	 * @return true on success, false on error
	 */
	bool generate(const char *privateKey, const char *publicKey, int bits,
			char *secret = nullptr) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_RSA_H_ */
