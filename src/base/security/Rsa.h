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
#include <openssl/rsa.h>

namespace wanhive {
/**
 * RSA public Key cryptography
 * Thread safe at class level
 */
class Rsa {
public:
	Rsa() noexcept;
	~Rsa();
	//=================================================================
	/*
	 * Initialization Routines
	 * Following methods can be called multiple times on the same object
	 * keys and password can be nullptr
	 */
	bool init(const char *privateKey, const char *publicKey, bool fromFile,
			char *password) noexcept;
	void reset() noexcept;

	bool loadPrivateKey(const char *privateKey, bool fromFile,
			char *password) noexcept;
	bool loadPublicKey(const char *publicKey, bool fromFile) noexcept;
	void freePrivateKey() noexcept;
	void freePublicKey() noexcept;
	bool hasPrivateKey() const noexcept;
	bool hasPublicKey() const noexcept;
	//=================================================================
	/*
	 * Rsa Encryptor and Decryptor
	 * The following two methods return true on success, false on error
	 * Use RSA_PKCS1_OAEP_PADDING (EME-OAEP as defined in PKCS #1 v2.0)
	 * Can be safely called without initialization
	 */
	bool encrypt(const unsigned char *data, unsigned int dataLength,
			unsigned char *encrypted,
			unsigned int *encryptedLength) const noexcept;
	bool decrypt(const unsigned char *data, unsigned int dataLength,
			unsigned char *decrypted,
			unsigned int *decryptedLength) const noexcept;
	//=================================================================
	/*
	 * Rsa signer and verifier
	 * The following two methods return true on success, false on error
	 * PKCS #1 v2.0 SHA-1 signer and verifier
	 * Can be safely called without initialization
	 */
	bool sign(const unsigned char *data, unsigned int dataLength,
			unsigned char *signature,
			unsigned int *signatureLength) const noexcept;
	bool verify(const unsigned char *data, unsigned int dataLength,
			unsigned char *signature,
			unsigned int signatureLength) const noexcept;
	//=================================================================
	/*
	 * Generates PEM encoded RSA key pair
	 * passPhrase can be nullptr
	 */
	static bool generateKeyPair(const char *privateKeyFile,
			const char *publicKeyFile, int bits,
			char *password = nullptr) noexcept;
	//=================================================================
private:
	//<key> is a NUL-terminated ASCII string, can be nullptr
	static EVP_PKEY* create(const char *key, bool isPublicKey,
			char *password) noexcept;
	//filename can be nullptr (results in noop)
	static EVP_PKEY* createFromFile(const char *filename, bool isPublicKey,
			char *password) noexcept;
	static bool verifyRSAKey(EVP_PKEY *key) noexcept;
	static void destroyKey(EVP_PKEY *key) noexcept;
	//=================================================================
	/**
	 * Used for key pair generation
	 */
	static EVP_PKEY* generateRSAKey(int bits) noexcept;
	//If a password is provided and cipher is nullptr then AES256-CBC is used
	static bool generatePem(const char *filename, EVP_PKEY *key,
			bool isPublicKey, char *password = nullptr,
			const EVP_CIPHER *cipher = nullptr) noexcept;
private:
	EVP_PKEY *_public;
	EVP_PKEY *_private;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_RSA_H_ */
