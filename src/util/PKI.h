/*
 * PKI.h
 *
 * RSA based asymmetric cryptography
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
using PKIEncryptedData = unsigned char[WH_PKI_ENCODING_LEN];
//-----------------------------------------------------------------
/**
 * Asymmetric cryptography facility based on RSA
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
	 * Initializes the object (discards existing keys).
	 * @param hostKey host (private) key string (nullptr to ignore)
	 * @param publicKey public key string (nullptr to ignore)
	 * @param fromFile true to treat the key strings as PEM file paths, false to
	 * treat the strings as base-16 encoded keys.
	 * @return true on success, false otherwise
	 */
	bool initialize(const char *hostKey, const char *publicKey, bool fromFile =
			true) noexcept;

	/**
	 * Initializes the public key (discards existing key).
	 * @param publicKey public key string
	 * @param fromFile true to treat the key string as PEM file path, false to
	 * treat the string as base-16 encoded key.
	 * @return true on success, false otherwise
	 */
	bool loadPublicKey(const char *publicKey, bool fromFile = true) noexcept;
	/**
	 * Initializes the host (private) key (discards existing key).
	 * @param hostKey host (private) key string
	 * @param fromFile true to treat the key string as PEM file path, false to
	 * treat the string as base-16 encoded key.
	 * @return true on success, false otherwise
	 */
	bool loadHostKey(const char *hostKey, bool fromFile = true) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks public key's availability.
	 * @return true if a public key exists, false otherwise
	 */
	bool hasPublicKey() const noexcept;
	/**
	 * Checks host (private) key's availability.
	 * @return true if a host (private) key exists, false otherwise
	 */
	bool hasHostKey() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs public key encryption. Cannot encrypt data blocks larger than
	 * PKI::MAX_PT_LEN bytes.
	 * @param block data for encryption
	 * @param size input data's size in bytes
	 * @param target output (encrypted data) buffer
	 * @return true on success, false otherwise
	 */
	bool encrypt(const void *block, unsigned int size,
			PKIEncryptedData *target) noexcept;
	/**
	 * Performs private key decryption.
	 * @param block encrypted data
	 * @param result output (decrypted data) buffer. It's capacity should be at
	 * least PKI::ENCODING_LENGTH bytes.
	 * @param size stores output data's size in bytes
	 * @return true on success, false otherwise
	 */
	bool decrypt(const PKIEncryptedData *block, void *result,
			unsigned int *size = nullptr) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs private key signing.
	 * @param block data for signing
	 * @param size data's size in bytes
	 * @param sig output (signature) buffer
	 * @return true on success, false otherwise
	 */
	bool sign(const void *block, unsigned int size, Signature *sig) noexcept;
	/**
	 * Performs signature verification using the public key.
	 * @param block verified data
	 * @param len data's size in bytes
	 * @param sig digital signature
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const void *block, unsigned int len,
			const Signature *sig) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates and stores key pair in PEM-encoded files.
	 * @param hostKey host (private) key file's path
	 * @param publicKey public key file's path
	 */
	static void generate(const char *hostKey, const char *publicKey);
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
