/*
 * Hash.h
 *
 * SHA-512 secure hash abstraction
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
	Hash() noexcept;
	~Hash();
public:
	/*
	 * Hash/verify block of given size using SHA2-512.
	 * None of the two functions must throw exception
	 */
	void create(const void *block, unsigned int size, Digest *digest) noexcept;
	bool verify(const Digest *digest, const void *block,
			unsigned int size) noexcept;
	static unsigned int encode(const Digest *digest,
			EncodedDigest *enc) noexcept;
public:
	static constexpr unsigned int SIZE = Sha::length(WH_SHA512); //64 bytes
private:
	Sha sha;
};

} /* namespace wanhive */

#endif /* WH_UTIL_HASH_H_ */
