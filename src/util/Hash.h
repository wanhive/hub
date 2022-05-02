/*
 * Hash.h
 *
 * SHA512 based message digest
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

namespace wanhive {
//-----------------------------------------------------------------
//We are using SHA-512, 64-bytes (512-bits) digest
using Digest=unsigned char[64];
//Base 64 encoding of a raw Digest, 89 bytes are used including the NUL-terminator
using EncodedDigest=char[128];
//-----------------------------------------------------------------
/**
 * SHA512 based message digest
 */
class Hash {
public:
	/**
	 * Constructor: initializes the SHA-512 algorithm
	 */
	Hash() noexcept;

	/**
	 * Destructor
	 */
	~Hash();

	/**
	 * Hashes a block of data
	 *
	 * @param block the input data
	 * @param size size of the input data in bytes
	 * @param digest the object for storing the digest value
	 * @return true on success, false otherwise
	 */
	bool create(const void *block, unsigned int size, Digest *digest) noexcept;

	/**
	 * Verifies a digest value with the original data
	 *
	 * @param digest the digest value to verify
	 * @param block the reference data
	 * @param size reference data's size in bytes
	 * @return
	 */
	bool verify(const Digest *digest, const void *block,
			unsigned int size) noexcept;

	/**
	 * Base-64 encodes the given digest value
	 * @param digest the digest value to encode
	 * @param enc the object for storing the result
	 *
	 * @return length of the output (excluding the nul-terminator)
	 */
	static unsigned int encode(const Digest *digest,
			EncodedDigest *enc) noexcept;
public:
	/** The output size in bytes (64 bytes) **/
	static constexpr unsigned int SIZE = Sha::length(WH_SHA512);
private:
	Sha sha;
};

} /* namespace wanhive */

#endif /* WH_UTIL_HASH_H_ */
