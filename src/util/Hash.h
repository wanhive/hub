/*
 * Hash.h
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

namespace wanhive {
//-----------------------------------------------------------------
/*! SHA-512, 64-bytes (512-bits) digest */
using Digest = unsigned char[64];
/*! Base 64 encoding of a raw Digest */
using EncodedDigest = char[128];
//-----------------------------------------------------------------
/**
 * Cryptographic hashing facility
 */
class Hash {
public:
	/**
	 * Constructor: initializes the SHA-512 algorithm.
	 */
	Hash() noexcept;
	/**
	 * Destructor.
	 */
	~Hash();
	//-----------------------------------------------------------------
	/**
	 * Hashes a block of data.
	 * @param data input data
	 * @param size input data's size in bytes
	 * @param digest stores the message digest
	 * @return true on success, false otherwise
	 */
	bool create(const void *data, unsigned int size, Digest *digest) noexcept;
	/**
	 * Verifies a message digest with the original data.
	 * @param digest message digest
	 * @param data reference data
	 * @param size data's size in bytes
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const Digest *digest, const void *data,
			unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Base-64 encodes a message digest.
	 * @param digest message digest
	 * @param encoded stores the output
	 * @return output's length in bytes (excluding the nul-terminator)
	 */
	static unsigned int encode(const Digest *digest,
			EncodedDigest *encoded) noexcept;
public:
	/*! Message digest size in bytes **/
	static constexpr unsigned int SIZE = Sha::length(WH_SHA512);
private:
	Sha sha;
};

} /* namespace wanhive */

#endif /* WH_UTIL_HASH_H_ */
