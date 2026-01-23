/*
 * PKI.h
 *
 * Asymmetric cryptography facility
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_PKI_H_
#define WH_UTIL_PKI_H_
#include "../base/security/Rsa.h"

namespace wanhive {
//-----------------------------------------------------------------
//Length of RSA key in bits
#undef WH_PKI_KEY_LENGTH
#define WH_PKI_KEY_LENGTH 3072
//Length of RSA encrypted data in bytes
#undef WH_PKI_ENCODING_LEN
#define WH_PKI_ENCODING_LEN ((WH_PKI_KEY_LENGTH) / 8)
//-----------------------------------------------------------------
/*! RSA signature */
using Signature = unsigned char[WH_PKI_ENCODING_LEN];
/*! RSA encrypted data */
using CipherText = unsigned char[WH_PKI_ENCODING_LEN];
//-----------------------------------------------------------------
/**
 * Asymmetric cryptography facility
 */
class PKI {
public:
	/**
	 * Default constructor: doesn't set up a key pair.
	 */
	PKI() noexcept;
	/**
	 * Destructor
	 */
	~PKI();
	//-----------------------------------------------------------------
	/**
	 * Loads key pair from PEM-encoded files (discards existing keys).
	 * @param privateKey private key file's path (can be nullptr)
	 * @param publicKey public key file's path (can be nullptr)
	 * @return true on success, false otherwise
	 */
	bool initialize(const char *privateKey, const char *publicKey) noexcept;
	/**
	 * Loads private key from PEM-encoded file (discards existing key).
	 * @param key private key file's path (can be nullptr)
	 * @return true on success, false otherwise
	 */
	bool loadPrivateKey(const char *key) noexcept;
	/**
	 * Loads public key from PEM-encoded file (discards existing key).
	 * @param key public key file's path (can be nullptr)
	 * @return true on success, false otherwise
	 */
	bool loadPublicKey(const char *key) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks private key's availability.
	 * @return true if a host (private) key exists, false otherwise
	 */
	bool hasPrivateKey() const noexcept;
	/**
	 * Checks public key's availability.
	 * @return true if a public key exists, false otherwise
	 */
	bool hasPublicKey() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs public key encryption. Cannot encrypt data blocks larger than
	 * PKI::MAX_PT_LEN bytes.
	 * @param plaintext data for encryption
	 * @param size input data's size in bytes
	 * @param ciphertext output (encrypted data) buffer
	 * @return true on success, false otherwise
	 */
	bool encrypt(const void *plaintext, unsigned int size,
			CipherText *ciphertext) noexcept;
	/**
	 * Performs private key decryption.
	 * @param ciphertext encrypted data
	 * @param plaintext output (original data) buffer. It's capacity should be
	 * at least PKI::ENCODING_LENGTH bytes.
	 * @param size stores output data's size in bytes
	 * @return true on success, false otherwise
	 */
	bool decrypt(const CipherText *ciphertext, void *plaintext,
			unsigned int *size = nullptr) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs private key signing.
	 * @param data signature creation data
	 * @param size data's size in bytes
	 * @param signature output (digital signature) buffer
	 * @return true on success, false otherwise
	 */
	bool sign(const void *data, unsigned int size,
			Signature *signature) noexcept;
	/**
	 * Performs signature verification using the public key.
	 * @param data verifiable data
	 * @param size data's size in bytes
	 * @param signature digital signature
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const void *data, unsigned int size,
			const Signature *signature) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates and stores key pair as PEM-encoded text files.
	 * @param privateKey private key file's path
	 * @param publicKey public key file's path
	 */
	static void generate(const char *privateKey, const char *publicKey);
public:
	/*! Key size in bits */
	static constexpr unsigned int KEY_LENGTH = WH_PKI_KEY_LENGTH;
	/*! Size of encrypted data in bytes **/
	static constexpr unsigned int ENCODING_LENGTH = WH_PKI_ENCODING_LEN;
	/*! Signature size in bytes */
	static constexpr unsigned int SIGNATURE_LENGTH = WH_PKI_ENCODING_LEN;
	/*! Size of encrypted data in bytes **/
	static constexpr unsigned int ENCRYPTED_LENGTH = WH_PKI_ENCODING_LEN;
	/*! Maximum data size (bytes) which can be encrypted */
	static constexpr unsigned int MAX_PT_LEN = (ENCODING_LENGTH)
			- ((2 * 160 / 8) + 2);
private:
	Rsa rsa;
};

#undef WH_PKI_KEY_LENGTH
#undef WH_PKI_ENCODING_LEN

} /* namespace wanhive */

#endif /* WH_UTIL_PKI_H_ */
