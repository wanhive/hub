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
#include"../common/NonCopyable.h"
#include <openssl/evp.h>

namespace wanhive {
/**
 * RSA public key cryptography
 */
class Rsa: private NonCopyable {
public:
	/**
	 * Default constructor
	 */
	Rsa() noexcept;
	/**
	 * Destructor
	 */
	~Rsa();
	//-----------------------------------------------------------------
	/**
	 * Loads private and/or public keys.
	 * @param privateKey private key's source (can be nullptr)
	 * @param publicKey public key's source (can be nullptr)
	 * @param fromFile true for treating the key sources as pathnames to PEM
	 * encoded files, false for treating the key sources as Base-16 inputs.
	 * @param password private key's password (can be nullptr)
	 * @return true on success, false on error
	 */
	bool init(const char *privateKey, const char *publicKey, bool fromFile,
			char *password) noexcept;
	/**
	 * Clears the private and public keys.
	 */
	void reset() noexcept;
	/**
	 * Loads the private key.
	 * @param privateKey private key's source (can be nullptr)
	 * @param fromFile true for treating the key source as pathname to a PEM
	 * encoded file, false for treating the key source as Base-16 input.
	 * @param password private key's password (can be nullptr)
	 * @return true on success, false on error
	 */
	bool loadPrivateKey(const char *privateKey, bool fromFile,
			char *password) noexcept;
	/**
	 * Loads the public key.
	 * @param publicKey public key's source (can be nullptr)
	 * @param fromFile true for treating the key source as pathname to a PEM
	 * encoded file, false for treating the key source as Base-16 input.
	 * @return true on success, false on error
	 */
	bool loadPublicKey(const char *publicKey, bool fromFile) noexcept;
	/**
	 * Clears the private key.
	 */
	void freePrivateKey() noexcept;
	/**
	 * Clears the public key.
	 */
	void freePublicKey() noexcept;
	/**
	 * Checks whether a private key exists.
	 * @return true if private key exists, false otherwise
	 */
	bool hasPrivateKey() const noexcept;
	/**
	 * Checks whether a public key exists.
	 * @return true if public key exists, false otherwise
	 */
	bool hasPublicKey() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Encrypts the given plain text. Uses RSA_PKCS1_OAEP_PADDING (EME-OAEP as
	 * defined in PKCS #1 v2.0).
	 * @param data input data
	 * @param dataLength input data's size in bytes
	 * @param encrypted buffer for storing the encrypted data. Set to nullptr to
	 * calculate the maximum buffer size.
	 * @param encryptedLength value-result argument for providing the output
	 * buffer's size in bytes and returning the encrypted data's size in bytes.
	 * @return true on success, false on error
	 */
	bool encrypt(const unsigned char *data, unsigned int dataLength,
			unsigned char *encrypted,
			unsigned int *encryptedLength) const noexcept;
	/**
	 * Decrypts the given encrypted data. Uses RSA_PKCS1_OAEP_PADDING (EME-OAEP
	 * as defined in PKCS #1 v2.0).
	 * @param data encrypted data
	 * @param dataLength encrypted data's size
	 * @param decrypted buffer for storing the plain text output. Set to nullptr
	 * to calculate the maximum buffer size.
	 * @param decryptedLength value-result argument for providing the output
	 * buffer's size in bytes and returning the decrypted data's size in bytes.
	 * @return true on success, false on error
	 */
	bool decrypt(const unsigned char *data, unsigned int dataLength,
			unsigned char *decrypted,
			unsigned int *decryptedLength) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Signs (PKCS #1 v2.0 SHA-1) the given data.
	 * @param data data to be signed
	 * @param dataLength data's size in bytes
	 * @param signature buffer for storing the digital signature. Set to nullptr
	 * to calculate the maximum buffer size.
	 * @param signatureLength object for storing signature's size in bytes
	 * @return true on success, false on error
	 */
	bool sign(const unsigned char *data, unsigned int dataLength,
			unsigned char *signature,
			unsigned int *signatureLength) const noexcept;
	/**
	 * Verifies (PKCS #1 v2.0 SHA-1) the given data.
	 * @param data the verified data
	 * @param dataLength data's size n bytes
	 * @param signature digital signature
	 * @param signatureLength signature's size in bytes
	 * @return true if verification was successful, false otherwise
	 */
	bool verify(const unsigned char *data, unsigned int dataLength,
			unsigned char *signature,
			unsigned int signatureLength) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates PEM encoded key pair.
	 * @param privateKeyFile pathname of the private key file
	 * @param publicKeyFile pathname of the public key file
	 * @param bits key's size in bits
	 * @param password private key's password (can be nullptr)
	 * @return true on success, false on error
	 */
	static bool generateKeyPair(const char *privateKeyFile,
			const char *publicKeyFile, int bits,
			char *password = nullptr) noexcept;
private:
	//<key> is a NUL-terminated ASCII string, can be nullptr
	static EVP_PKEY* create(const char *key, bool isPublicKey,
			char *password) noexcept;
	//filename can be nullptr (results in noop)
	static EVP_PKEY* createFromFile(const char *filename, bool isPublicKey,
			char *password) noexcept;
	//Validates the keys
	static bool verifyRSAKey(const EVP_PKEY *key) noexcept;
	static void destroyKey(EVP_PKEY *key) noexcept;
	//Used for key pair generation
	static EVP_PKEY* generateRSAKey(int bits) noexcept;
	//If a password is provided and cipher is nullptr then AES256-CBC is used
	static bool generatePem(const char *filename, EVP_PKEY *key,
			bool isPublicKey, char *password = nullptr,
			const EVP_CIPHER *cipher = nullptr) noexcept;
private:
	EVP_PKEY *_public { };
	EVP_PKEY *_private { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_RSA_H_ */
