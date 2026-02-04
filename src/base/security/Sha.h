/**
 * @file Sha.h
 *
 * SHA secure hash
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_SHA_H_
#define WH_BASE_SECURITY_SHA_H_
#include "../common/NonCopyable.h"
#include <openssl/evp.h>
#include <openssl/sha.h>

/*! @namespace wanhive */
namespace wanhive {
//-----------------------------------------------------------------
/**
 * Supported hash functions
 */
enum HashType {
	WH_SHA1, /**< SHA-1 */
	WH_SHA256,/**< SHA-256 */
	WH_SHA512 /**< SHA-512 */
};
//-----------------------------------------------------------------
/**
 * SHA secure hash
 */
class Sha: private NonCopyable {
public:
	/**
	 * Constructor: assigns a hash function type.
	 * @param type hash function type
	 */
	Sha(HashType type) noexcept;
	/**
	 * Destructor
	 */
	~Sha();
	/**
	 * Initializes or resets the digest context.
	 * @return true on success, false on error
	 */
	bool init() noexcept;
	/**
	 * Updates the digest context with additional data.
	 * @param data input data
	 * @param bytes data size in bytes
	 * @return true on success, false on error
	 */
	bool update(const void *data, size_t bytes) noexcept;
	/**
	 * Completes the message digest operation and returns the digest value.
	 * @param digest stores the message digest
	 * @param size stores the message digest size in bytes
	 * @return true on success, false on error
	 */
	bool final(unsigned char *digest, unsigned int *size = nullptr) noexcept;
	/**
	 * Generates message digest from input data.
	 * @param data input data
	 * @param bytes data size in bytes
	 * @param digest stores the message digest
	 * @param size stores the message digest size in bytes
	 * @return true on success, false on error
	 */
	bool create(const void *data, size_t bytes, unsigned char *digest,
			unsigned int *size = nullptr) noexcept;
	/**
	 * Verifies input data's message digest.
	 * @param data input data
	 * @param bytes data size in bytes
	 * @param digest message digest
	 * @return true on successful match, false otherwise
	 */
	bool verify(const void *data, size_t bytes,
			const unsigned char *digest) noexcept;
	/**
	 * Returns the expected message digest size.
	 * @return digest size in bytes
	 */
	unsigned int length() const noexcept;
	/**
	 * Returns the message digest size for a hash function.
	 * @param type hash function identifier
	 * @return digest size in bytes
	 */
	static constexpr unsigned int length(HashType type) noexcept {
		switch (type) {
		case WH_SHA1:
			return SHA_DIGEST_LENGTH;
		case WH_SHA256:
			return SHA256_DIGEST_LENGTH;
		case WH_SHA512:
			return SHA512_DIGEST_LENGTH;
		default:
			return MAX_MD_SIZE;
		}
	}
private:
	const EVP_MD* type() const noexcept;
public:
	/*! Maximum message digest size in bytes */
	static constexpr unsigned int MAX_MD_SIZE = EVP_MAX_MD_SIZE;
private:
	EVP_MD_CTX *ctx;
	const HashType _type;
	const unsigned int _length;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SHA_H_ */
