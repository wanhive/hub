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
		type { type }, _length { length(type) }, ctx { } {

}

Sha::~Sha() {
	EVP_MD_CTX_free(ctx);
}

bool Sha::init() noexcept {
	if (!ctx) {
		return initContext();
	} else {
		return EVP_DigestInit_ex(ctx, nullptr, nullptr);
	}
}

bool Sha::update(const void *data, size_t dataLength) noexcept {
	if (ctx) {
		return EVP_DigestUpdate(ctx, data, dataLength);
	} else {
		return false;
	}
}

bool Sha::final(unsigned char *messageDigest, unsigned int *size) noexcept {
	if (ctx) {
		return EVP_DigestFinal_ex(ctx, messageDigest, size);
	} else {
		return false;
	}
}

bool Sha::create(const unsigned char *data, size_t dataLength,
		unsigned char *messageDigest, unsigned int *size) noexcept {
	const EVP_MD *md { };
	switch (type) {
	case WH_SHA1:
		md = EVP_sha1();
		break;
	case WH_SHA256:
		md = EVP_sha256();
		break;
	case WH_SHA512:
		md = EVP_sha512();
		break;
	default:
		break;
	}

	return EVP_Digest(data, dataLength, messageDigest, size, md, nullptr);
}

bool Sha::verify(const unsigned char *data, size_t dataLength,
		const unsigned char *messageDigest) noexcept {
	if (data && dataLength && messageDigest) {
		unsigned char md[EVP_MAX_MD_SIZE];
		return create(data, dataLength, md)
				&& (memcmp(md, messageDigest, length()) == 0);
	} else {
		return false;
	}
}

unsigned int Sha::length() const noexcept {
	return _length;
}

bool Sha::initContext() noexcept {
	if (!ctx) {
		ctx = EVP_MD_CTX_new();
	}

	switch (type) {
	case WH_SHA1:
		return EVP_DigestInit(ctx, EVP_sha1());
	case WH_SHA256:
		return EVP_DigestInit(ctx, EVP_sha256());
	case WH_SHA512:
		return EVP_DigestInit(ctx, EVP_sha512());
	default:
		return false;
	}
}

} /* namespace wanhive */
