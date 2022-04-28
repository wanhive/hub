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

Sha::Sha(DigestType type) noexcept :
		ctx { }, type { type }, _length { length(type) } {

}

Sha::~Sha() {
	EVP_MD_CTX_free(ctx);
}

bool Sha::init() noexcept {
	if (!ctx) {
		return (ctx = EVP_MD_CTX_new()) && EVP_DigestInit(ctx, selectType());
	} else {
		return EVP_DigestInit_ex(ctx, nullptr, nullptr);
	}
}

bool Sha::update(const void *data, size_t dataLength) noexcept {
	if (ctx && (!dataLength || data)) {
		return EVP_DigestUpdate(ctx, data, dataLength);
	} else {
		return false;
	}
}

bool Sha::final(unsigned char *messageDigest, unsigned int *size) noexcept {
	if (ctx && messageDigest) {
		return EVP_DigestFinal_ex(ctx, messageDigest, size);
	} else {
		return false;
	}
}

bool Sha::create(const unsigned char *data, size_t dataLength,
		unsigned char *messageDigest, unsigned int *size) noexcept {
	if (!dataLength || (data && messageDigest)) {
		return EVP_Digest(data, dataLength, messageDigest, size, selectType(),
				nullptr);
	} else {
		return false;
	}
}

bool Sha::verify(const unsigned char *data, size_t dataLength,
		const unsigned char *messageDigest) noexcept {
	if (messageDigest) {
		unsigned char md[EVP_MAX_MD_SIZE];
		unsigned int len;
		return create(data, dataLength, md, &len)
				&& (memcmp(md, messageDigest, len) == 0);
	} else {
		return false;
	}
}

unsigned int Sha::length() const noexcept {
	return _length;
}

const EVP_MD* Sha::selectType() const noexcept {
	switch (type) {
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
