/*
 * Sha.cpp
 *
 * SHA secure hash
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Sha.h"
#include <cstring>

namespace wanhive {

Sha::Sha(HashType type) noexcept :
		ctx { }, _type { type }, _length { length(type) } {

}

Sha::~Sha() {
	EVP_MD_CTX_free(ctx);
}

bool Sha::init() noexcept {
	if (!ctx) {
		return (ctx = EVP_MD_CTX_new()) && (EVP_DigestInit(ctx, get()) > 0);
	} else {
		return (EVP_DigestInit_ex(ctx, nullptr, nullptr) > 0);
	}
}

bool Sha::update(const void *data, unsigned int bytes) noexcept {
	if (ctx && (!bytes || data)) {
		return (EVP_DigestUpdate(ctx, data, bytes) > 0);
	} else {
		return false;
	}
}

bool Sha::final(unsigned char *digest, unsigned int *size) noexcept {
	if (ctx && digest) {
		return (EVP_DigestFinal_ex(ctx, digest, size) > 0);
	} else {
		return false;
	}
}

bool Sha::create(const void *data, unsigned int bytes, unsigned char *digest,
		unsigned int *size) noexcept {
	if (!bytes || (data && digest)) {
		return (EVP_Digest(data, bytes, digest, size, get(), nullptr) > 0);
	} else {
		return false;
	}
}

bool Sha::verify(const void *data, unsigned int bytes,
		const unsigned char *digest) noexcept {
	if (digest) {
		unsigned char md[MAX_MD_SIZE];
		unsigned int len { };
		return create(data, bytes, md, &len) && (memcmp(md, digest, len) == 0);
	} else {
		return false;
	}
}

unsigned int Sha::length() const noexcept {
	return _length;
}

const EVP_MD* Sha::get() const noexcept {
	switch (_type) {
	case WH_SHA1:
		return EVP_sha1();
	case WH_SHA256:
		return EVP_sha256();
	case WH_SHA512:
		return EVP_sha512();
	default:
		return nullptr;
	}
}

} /* namespace wanhive */
