/*
 * Hash.cpp
 *
 * SHA-512 secure hash abstraction
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Hash.h"
#include "../base/ds/Encoding.h"

namespace wanhive {

Hash::Hash() noexcept :
		sha(WH_SHA512) {
}

Hash::~Hash() {
}

void Hash::create(const void *block, unsigned int size, Digest *digest) noexcept {
	sha.create((const unsigned char*) block, size, (unsigned char*) digest);
}

bool Hash::verify(const Digest *digest, const void *block,
		unsigned int size) noexcept {
	return sha.verify((const unsigned char*) block, size,
			(const unsigned char*) digest);
}

unsigned int Hash::encode(const Digest *digest, EncodedDigest *enc) noexcept {
	return Encoding::encode(ENC_BASE64, (char*) enc, digest, Hash::SIZE,
			sizeof(EncodedDigest));
}

} /* namespace wanhive */
