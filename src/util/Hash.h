/**
 * @file Hash.h
 *
 * Cryptographic hashing
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_HASH_H_
#define WH_UTIL_HASH_H_
#include "../base/security/Sha.h"

/*! @namespace wanhive */
namespace wanhive {
//-----------------------------------------------------------------
/*! SHA-512, 64-bytes (512-bits) digest */
using Digest = unsigned char[64];
/*! Base-64 encoding of a raw Digest (89 bytes used) */
using EncodedDigest = char[128];
//-----------------------------------------------------------------
/**
 * Cryptographic hashing
 */
class Hash {
public:
	/**
	 * Constructor: initializes SHA-512 hash function.
	 */
	Hash() noexcept;
	/**
	 * Destructor.
	 */
	~Hash();
	//-----------------------------------------------------------------
	/**
	 * Generates input data's message digest.
	 * @param data input data
	 * @param bytes data size in bytes
	 * @param digest stores the message digest
	 * @return true on success, false otherwise
	 */
	bool create(const void *data, size_t bytes, Digest &digest) noexcept;
	/**
	 * Generates input data's message digest.
	 * @param data input data
	 * @param bytes data size in bytes
	 * @param digest stores the message digest
	 * @return true on success, false otherwise
	 */
	bool create(const void *data, size_t bytes, Digest *digest) noexcept;
	/**
	 * Verifies input data's message digest.
	 * @param digest message digest
	 * @param data input data
	 * @param bytes data size in bytes
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const Digest &digest, const void *data, size_t bytes) noexcept;
	/**
	 * Verifies input data's message digest.
	 * @param digest message digest
	 * @param data input data
	 * @param bytes data size in bytes
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const Digest *digest, const void *data, size_t bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Base-64 encodes a message digest.
	 * @param digest message digest
	 * @param encoded stores the output
	 * @return output size in bytes (excluding the nul-terminator)
	 */
	static unsigned int encode(const Digest &digest,
			EncodedDigest &encoded) noexcept;
	/**
	 * Base-64 encodes a message digest.
	 * @param digest message digest
	 * @param encoded stores the output
	 * @return output size in bytes (excluding the nul-terminator)
	 */
	static unsigned int encode(const Digest *digest,
			EncodedDigest *encoded) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Type casts Digest to raw bytes.
	 * @param digest message digest
	 * @return pointer to raw bytes
	 */
	static unsigned char* bytes(Digest &digest) noexcept;
	/**
	 * Type casts Digest to raw bytes.
	 * @param digest message digest
	 * @return pointer to raw bytes
	 */
	static unsigned char* bytes(Digest *digest) noexcept;
	/**
	 * Type casts Digest to raw bytes.
	 * @param digest message digest
	 * @return pointer to raw bytes
	 */
	static const unsigned char* bytes(const Digest &digest) noexcept;
	/**
	 * Type casts Digest to raw bytes.
	 * @param digest message digest
	 * @return pointer to raw bytes
	 */
	static const unsigned char* bytes(const Digest *digest) noexcept;
	/**
	 * Type casts EncodedDigest to raw bytes.
	 * @param digest base-64 encoded message digest
	 * @return pointer to raw bytes
	 */
	static char* bytes(EncodedDigest &encoded) noexcept;
	/**
	 * Type casts EncodedDigest to raw bytes.
	 * @param digest base-64 encoded message digest
	 * @return pointer to raw bytes
	 */
	static char* bytes(EncodedDigest *encoded) noexcept;
	/**
	 * Type casts EncodedDigest to raw bytes.
	 * @param digest base-64 encoded message digest
	 * @return pointer to raw bytes
	 */
	static const char* bytes(const EncodedDigest &encoded) noexcept;
	/**
	 * Type casts EncodedDigest to raw bytes.
	 * @param digest base-64 encoded message digest
	 * @return pointer to raw bytes
	 */
	static const char* bytes(const EncodedDigest *encoded) noexcept;
public:
	/*! Message digest size in bytes **/
	static constexpr unsigned int SIZE = Sha::length(WH_SHA512);
	/*! Message digest's base-64 encoding size in bytes **/
	static constexpr unsigned int ENCODING_SIZE = 89;
private:
	Sha sha;
};

} /* namespace wanhive */

#endif /* WH_UTIL_HASH_H_ */
