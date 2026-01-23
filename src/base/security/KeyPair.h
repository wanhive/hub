/*
 * KeyPair.h
 *
 * Key pair for asymmetric cryptography
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_KEYPAIR_H_
#define WH_BASE_SECURITY_KEYPAIR_H_
#include "../common/NonCopyable.h"
#include <openssl/evp.h>

namespace wanhive {
/**
 * Key pair for asymmetric cryptography
 */
class KeyPair: private NonCopyable {
public:
	/**
	 * Constructor: assigns a NID (numeric identifier) and a name.
	 * @param nid numeric identifier
	 * @param name cipher's name
	 */
	KeyPair(int nid, const char *name) noexcept;
	/**
	 * Destructor
	 */
	~KeyPair();
	//-----------------------------------------------------------------
	/**
	 * Loads private and/or public keys (discards the existing keys).
	 * @param privateKey private key (can be nullptr)
	 * @param publicKey public key (can be nullptr)
	 * @param secret private key's pass phrase
	 * @param memory true to treat the key data as PEM-encoded strings, false to
	 * treat them as PEM-encoded file paths.
	 * @return true on success, false on error
	 */
	bool setup(const char *privateKey, const char *publicKey, char *secret =
			nullptr, bool memory = false) noexcept;
	/**
	 * Disposes of the existing keys.
	 */
	void reset() noexcept;
	/**
	 * Loads a private key (discards the existing key).
	 * @param key private key (can be nullptr)
	 * @param secret private key's pass phrase
	 * @param memory true to treat the key data as PEM-encoded string, false to
	 * treat it as a PEM-encoded file's path.
	 * @return true on success, false on error
	 */
	bool loadPrivateKey(const char *key, char *secret = nullptr, bool memory =
			false) noexcept;
	/**
	 * Loads a public key (discards the existing key).
	 * @param key public key (can be nullptr)
	 * @param memory true to treat the key data as PEM-encoded string, false to
	 * treat it as a PEM-encoded file's path.
	 * @return true on success, false on error
	 */
	bool loadPublicKey(const char *key, bool memory = false) noexcept;
	/**
	 * Disposes of the private key.
	 */
	void freePrivateKey() noexcept;
	/**
	 * Disposes of the public key.
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
protected:
	/**
	 * Checks a key's compatibility.
	 * @param pkey asymmetric key
	 * @return true on a positive match, false otherwise
	 */
	bool validate(const EVP_PKEY *pkey) const noexcept;
	/**
	 * Validates the private components of a key.
	 * @param pkey asymmetric key
	 * @return true on success, false on error
	 */
	bool isPrivateKey(EVP_PKEY *pkey) const noexcept;
	/**
	 * Validates the public key component of a key.
	 * @param pkey asymmetric key
	 * @return true on success, false on error
	 */
	bool isPublicKey(EVP_PKEY *pkey) const noexcept;
	/**
	 * Returns the public key.
	 * @return public key
	 */
	EVP_PKEY* getPrivateKey() const noexcept;
	/**
	 * Sets a new private key.
	 * @param pkey private key
	 * @return true on success, false on error
	 */
	bool setPrivateKey(EVP_PKEY *pkey) noexcept;
	/**
	 * Returns the private key.
	 * @return private key
	 */
	EVP_PKEY* getPublicKey() const noexcept;
	/**
	 * Sets a new public key.
	 * @param pkey public key
	 * @return true on success, false on error
	 */
	bool setPublicKey(EVP_PKEY *pkey) noexcept;
	/**
	 * generates a new asymmetric key pair.
	 * @param bits key size in bits
	 * @return generated key, nullptr on error
	 */
	EVP_PKEY* generate(unsigned int bits = 0) const noexcept;
	/**
	 * generates a new asymmetric key pair and stores them. On error, the old
	 * keys remain preserved.
	 * @param bits key size in bits
	 * @return generated key, nullptr on error
	 */
	EVP_PKEY* generate(unsigned int bits = 0) noexcept;
	/**
	 * Generates a key pair and stores them in PEM-encoded text files.
	 * @param privateKey private key file's path
	 * @param publicKey public key file's path
	 * @param bits key size in bits
	 * @param secret optional pass phrase for private key
	 * @param cipher optional encryption cipher (default: AES256-CBC)
	 *  @return true on success, false on error
	 */
	bool generate(const char *privateKey, const char *publicKey,
			unsigned int bits, char *secret,
			const EVP_CIPHER *cipher) const noexcept;
	/**
	 * Returns the cryptographic length of a key in bits.
	 * @param pkey asymmetric key
	 * @return cryptographic key length in bits, 0 on error
	 */
	static unsigned int bits(const EVP_PKEY *pkey) noexcept;
	/**
	 * Returns the maximum suitable output buffer size in bytes for various
	 * operations done with a key.
	 * @param pkey asymmetric key
	 * @return maximum buffer size in bytes, 0 on error
	 */
	static unsigned int size(const EVP_PKEY *pkey) noexcept;
	/**
	 * Returns the number of security bits of a key.
	 * @param pkey asymmetric key
	 * @return security bits, 0 on error
	 */
	static unsigned int security(const EVP_PKEY *pkey) noexcept;
	/**
	 * Stores a key as PEM-encoded text file.
	 * @param path PEM file's path
	 * @param pkey public or private key
	 * @param isPublic true for public key, false for private key
	 * @param secret optional pass phrase for private key
	 * @param cipher optional encryption cipher (default: AES256-CBC)
	 * @return true on success, false on error
	 */
	static bool store(const char *path, EVP_PKEY *pkey, bool isPublic,
			char *secret, const EVP_CIPHER *cipher) noexcept;
private:
	EVP_PKEY* fromMemory(const char *key, bool isPublic, char *secret) noexcept;
	EVP_PKEY* fromFile(const char *path, bool isPublic, char *secret) noexcept;
private:
	const int nid;
	const char *name;
	EVP_PKEY *_private { };
	EVP_PKEY *_public { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_KEYPAIR_H_ */
