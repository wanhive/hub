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
	 * Loads private and/or public keys.
	 * @param privateKey private key (can be nullptr)
	 * @param publicKey public key (can be nullptr)
	 * @param fromFile true to treat the keys as PEM-encoded file names, false
	 * to treat them as PEM-encoded strings.
	 * @param secret private key's pass phrase (can be nullptr)
	 * @return true on success, false on error
	 */
	bool init(const char *privateKey, const char *publicKey, bool fromFile,
			char *secret = nullptr) noexcept;
	/**
	 * Clears the private and public keys.
	 */
	void reset() noexcept;
	/**
	 * Loads the private key.
	 * @param privateKey private key (can be nullptr)
	 * @param fromFile true to treat the key as PEM-encoded file name, false to
	 * treat it as a PEM-encoded string.
	 * @param secret private key's pass phrase (can be nullptr)
	 * @return true on success, false on error
	 */
	bool loadPrivateKey(const char *privateKey, bool fromFile, char *secret =
			nullptr) noexcept;
	/**
	 * Loads the public key.
	 * @param publicKey public key (can be nullptr)
	 * @param fromFile true to treat the key as PEM-encoded file name, false to
	 * treat it as a PEM-encoded string.
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
	EVP_PKEY* generate(size_t bits = 0) const noexcept;
	/**
	 * generates a new asymmetric key pair and stores them. On error, the old
	 * keys remain preserved.
	 * @param bits key size in bits
	 * @return generated key, nullptr on error
	 */
	EVP_PKEY* generate(size_t bits = 0) noexcept;
	/**
	 * Generates a key pair and stores them in PEM-encoded text files.
	 * @param privateKeyFile private key file's path
	 * @param publicKeyFile public key file's path
	 * @param bits key size in bits
	 * @param secret optional pass phrase for private key
	 * @param cipher optional encryption cipher (default: AES256-CBC)
	 *  @return true on success, false on error
	 */
	bool generate(const char *privateKeyFile, const char *publicKeyFile,
			size_t bits, char *secret, const EVP_CIPHER *cipher) const noexcept;
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
	EVP_PKEY* create(const char *key, bool isPublic, char *secret) noexcept;
	EVP_PKEY* load(const char *path, bool isPublic, char *secret) noexcept;
private:
	const int nid;
	const char *name;
	EVP_PKEY *_private { };
	EVP_PKEY *_public { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_KEYPAIR_H_ */
