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

//RSA signature
using Signature = unsigned char[WH_PKI_ENCODING_LEN];
//RSA encrypted data
using PKIEncryptedData = unsigned char[WH_PKI_ENCODING_LEN];
//-----------------------------------------------------------------
/**
 * The asymmetric cryptography facility based on RSA
 * Objects of this class can be re-initialized
 */
class PKI {
public:
	/**
	 * Default constructor: doesn't set up the keys. Call PKI::intialize()
	 * explicitly to set up the public and private keys.
	 */
	PKI() noexcept;
	/**
	 * Destructor
	 */
	~PKI();
	//-----------------------------------------------------------------
	/**
	 * Initializes the object (discards the existing keys if the object was
	 * initialized previously).
	 * @param hostKey the host (private) key. Can be nullptr, in which case the
	 * associated capabilities will not be available.
	 * @param publicKey the public key. Can be nullptr, in which case the
	 * associated capabilities will not be available.
	 * @param fromFile true if the <hostKey> and <publicKey> arguments must be
	 * treated as pathnames to PEM-formatted files, false if those arguments
	 * must be treated as base-16 encoded keys.
	 * @return true on success, false otherwise
	 */
	bool initialize(const char *hostKey, const char *publicKey, bool fromFile =
			true) noexcept;

	/**
	 * Initializes the public key (discards the existing one).
	 * @param publicKey the public key
	 * @param fromFile true if <publicKey> argument must be treated as pathname
	 * to a pem-formatted file, false if <publicKey> must be treated as base-16
	 * encoded key.
	 * @return true on success, false otherwise
	 */
	bool loadPublicKey(const char *publicKey, bool fromFile = true) noexcept;
	/**
	 * Initializes the host (private) key (discards the existing one).
	 * @param hostKey the host (private) key
	 * @param fromFile true if <hostKey> argument must be treated as pathname
	 * to a pem-formatted file, false if <hostKey> must be treated as base-16
	 * encoded key.
	 * @return true on success, false otherwise
	 */
	bool loadHostKey(const char *hostKey, bool fromFile = true) noexcept;

	/**
	 * Checks the existence of public key.
	 * @return true if this object contains a public key, false otherwise
	 */
	bool hasPublicKey() const noexcept;
	/**
	 * Checks the existence of host (private) key.
	 * @return true if this object contains a host (private) key, false otherwise
	 */
	bool hasHostKey() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs public key encryption. Cannot encrypt data blocks larger than
	 * PKI::MAX_PT_LEN bytes.
	 * @param block the data block to encrypt
	 * @param size size of the data block to encrypt in bytes. This value
	 * should not exceed PKI::MAX_PT_LEN bytes.
	 * @param target object for storing the output (encrypted data)
	 * @return true on success, false otherwise
	 */
	bool encrypt(const void *block, unsigned int size,
			PKIEncryptedData *target) const noexcept;
	/**
	 * Performs private key decryption.
	 * @param block the encrypted data
	 * @param result buffer for storing the output (decrypted data). It's
	 * capacity should be at least PKI::ENCODING_LENGTH bytes.
	 * @param size the output size (bytes) is written here (can be nullptr)
	 * @return true on success, false otherwise
	 */
	bool decrypt(const PKIEncryptedData *block, void *result,
			unsigned int *size = nullptr) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs private key signing.
	 * @param block the data to be signed
	 * @param size size of data to be signed in bytes
	 * @param sig object for storing the output (signature)
	 * @return true on success, false otherwise
	 */
	bool sign(const void *block, unsigned int size,
			Signature *sig) const noexcept;
	/**
	 * Performs signature verification using the public key.
	 * @param block the verified data (the data believed originally signed)
	 * @param len the verified data length in bytes
	 * @param sig the signature
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const void *block, unsigned int len,
			const Signature *sig) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates key pair in PEM format and store them in files.
	 * @param hostKey pathname of the host (private) key file
	 * @param publicKey pathname of the public key file
	 */
	static void generateKeyPair(const char *hostKey, const char *publicKey);
public:
	/** Key size in bits */
	static constexpr unsigned int KEY_LENGTH = WH_PKI_KEY_LENGTH;
	/** Size of encrypted data in bytes **/
	static constexpr unsigned int ENCODING_LENGTH = WH_PKI_ENCODING_LEN;
	/** Signature size in bytes */
	static constexpr unsigned int SIGNATURE_LENGTH = WH_PKI_ENCODING_LEN;
	/** Size of encrypted data in bytes **/
	static constexpr unsigned int ENCRYPTED_LENGTH = WH_PKI_ENCODING_LEN;
	/** Maximum data size (bytes) which can be encrypted */
	static constexpr unsigned int MAX_PT_LEN = (ENCODING_LENGTH)
			- ((2 * 160 / 8) + 2);
private:
	Rsa rsa;
};

#undef WH_PKI_KEY_LENGTH
#undef WH_PKI_ENCODING_LEN

} /* namespace wanhive */

#endif /* WH_UTIL_PKI_H_ */
