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

bool Hash::create(const void *data, size_t bytes, Digest &digest) noexcept {
	return sha.create(data, bytes, Hash::bytes(digest));
}

bool Hash::create(const void *data, size_t bytes, Digest *digest) noexcept {
	return sha.create(data, bytes, Hash::bytes(digest));
}

bool Hash::verify(const Digest &digest, const void *data, size_t bytes) noexcept {
	return sha.verify(data, bytes, Hash::bytes(digest));
}

bool Hash::verify(const Digest *digest, const void *data, size_t bytes) noexcept {
	return sha.verify(data, bytes, Hash::bytes(digest));
}

unsigned int Hash::encode(const Digest &digest, EncodedDigest &encoded) noexcept {
	return Encoding::encode(ENC_BASE64, Hash::bytes(encoded),
			Hash::bytes(digest), Hash::SIZE, sizeof(EncodedDigest));
}

unsigned int Hash::encode(const Digest *digest, EncodedDigest *encoded) noexcept {
	return Encoding::encode(ENC_BASE64, Hash::bytes(encoded),
			Hash::bytes(digest), Hash::SIZE, sizeof(EncodedDigest));
}

unsigned char* Hash::bytes(Digest &digest) noexcept {
	return digest;
}

unsigned char* Hash::bytes(Digest *digest) noexcept {
	return digest ? (*digest) : nullptr;
}

const unsigned char* Hash::bytes(const Digest &digest) noexcept {
	return digest;
}

const unsigned char* Hash::bytes(const Digest *digest) noexcept {
	return digest ? (*digest) : nullptr;
}

char* Hash::bytes(EncodedDigest &encoded) noexcept {
	return encoded;
}

char* Hash::bytes(EncodedDigest *encoded) noexcept {
	return encoded ? (*encoded) : nullptr;
}

const char* Hash::bytes(const EncodedDigest &encoded) noexcept {
	return encoded;
}

const char* Hash::bytes(const EncodedDigest *encoded) noexcept {
	return encoded ? (*encoded) : nullptr;
}

} /* namespace wanhive */
