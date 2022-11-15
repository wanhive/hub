/*
 * Sha.h
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

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Supported message digest types
 */
enum DigestType {
	WH_SHA1, /**< SHA-1 */
	WH_SHA256,/**< SHA-256 */
	WH_SHA512 /**< SHA-512 */
};
//-----------------------------------------------------------------
/**
 * SHA message digest implementation. Supported message digest types have been
 * enumerated above.
 */
class Sha: private NonCopyable {
public:
	/**
	 * Constructor: assigns a message digest context.
	 * @param type message digest type
	 */
	Sha(DigestType type) noexcept;
	/**
	 * Destructor
	 */
	~Sha();
	/**
	 * Resets the message digest's context.
	 * @return true on success, false on error
	 */
	bool init() noexcept;
	/**
	 * Hashes the given data, can be called again to hash additional data.
	 * @param data input data for hashing
	 * @param dataLength input data's size in bytes
	 * @return true on success, false on error
	 */
	bool update(const void *data, size_t dataLength) noexcept;
	/**
	 * Retrieves the digest value.
	 * @param messageDigest buffer for storing the digest value
	 * @param size object for storing the digest size in bytes (can be nullptr)
	 * @return true on success, false on error
	 */
	bool final(unsigned char *messageDigest,
			unsigned int *size = nullptr) noexcept;
	/**
	 * Hashes the given data and returns the hash value.
	 * @param data input data for hashing
	 * @param dataLength input data's size in bytes
	 * @param messageDigest buffer for storing the digest value
	 * @param size object for storing the digest size in bytes (can be nullptr)
	 * @return true on success, false on error
	 */
	bool create(const unsigned char *data, size_t dataLength,
			unsigned char *messageDigest, unsigned int *size = nullptr) noexcept;
	/**
	 * Compares the given digest value with digest value of the given data.
	 * @param data input data for verification
	 * @param dataLength input data's size in bytes
	 * @param messageDigest the digest value for comparison
	 * @return true on successful match, false otherwise
	 */
	bool verify(const unsigned char *data, size_t dataLength,
			const unsigned char *messageDigest) noexcept;
	/**
	 * Returns the expected digest size.
	 * @return digest size in bytes
	 */
	unsigned int length() const noexcept;
	/**
	 * Returns the expected digest size for the given type.
	 * @param type the message digest type
	 * @return digest size in bytes
	 */
	static constexpr unsigned int length(DigestType type) noexcept {
		switch (type) {
		case WH_SHA1:
			return SHA_DIGEST_LENGTH;
		case WH_SHA256:
			return SHA256_DIGEST_LENGTH;
		default:
			return SHA512_DIGEST_LENGTH;
		}
	}
private:
	const EVP_MD* selectType() const noexcept;
private:
	EVP_MD_CTX *ctx;
	const DigestType type;
	const unsigned int _length; //Digest length in bytes
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SHA_H_ */
