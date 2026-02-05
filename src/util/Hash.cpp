/*
 * Hash.cpp
 *
 * Cryptographic hashing
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

bool Hash::create(const void *data, size_t bytes, Digest *digest) noexcept {
	return sha.create(data, bytes, *digest);
}

bool Hash::verify(const Digest *digest, const void *data, size_t bytes) noexcept {
	return sha.verify(data, bytes, *digest);
}

unsigned int Hash::encode(const Digest *digest, EncodedDigest *encoded) noexcept {
	return Encoding::encode(ENC_BASE64, *encoded, *digest, Hash::SIZE,
			sizeof(EncodedDigest));
}

} /* namespace wanhive */
