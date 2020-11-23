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
		type(type), _length(length(type)) {
}

Sha::~Sha() {

}

bool Sha::init() noexcept {
	switch (type) {
	case WH_SHA1:
		return SHA1_Init(&ctx.sha);
	case WH_SHA256:
		return SHA256_Init(&ctx.sha256);
	case WH_SHA512:
		return SHA512_Init(&ctx.sha512);
	default:
		return false;
	}
}

bool Sha::update(const void *data, size_t dataLength) noexcept {
	switch (type) {
	case WH_SHA1:
		return SHA1_Update(&ctx.sha, data, dataLength);
	case WH_SHA256:
		return SHA256_Update(&ctx.sha256, data, dataLength);
	case WH_SHA512:
		return SHA512_Update(&ctx.sha512, data, dataLength);
	default:
		return false;
	}
}

bool Sha::final(unsigned char *messageDigest) noexcept {
	switch (type) {
	case WH_SHA1:
		return SHA1_Final(messageDigest, &ctx.sha);
	case WH_SHA256:
		return SHA256_Final(messageDigest, &ctx.sha256);
	case WH_SHA512:
		return SHA512_Final(messageDigest, &ctx.sha512);
	default:
		return false;
	}
}

unsigned char* Sha::create(const unsigned char *data, size_t dataLength,
		unsigned char *messageDigest) noexcept {
	switch (type) {
	case WH_SHA1:
		return SHA1(data, dataLength, messageDigest);
	case WH_SHA256:
		return SHA256(data, dataLength, messageDigest);
	case WH_SHA512:
		return SHA512(data, dataLength, messageDigest);
	default:
		return nullptr;
	}
}

bool Sha::verify(const unsigned char *data, size_t dataLength,
		const unsigned char *messageDigest) noexcept {
	if (data && dataLength && messageDigest) {
		unsigned char md[SHA512_DIGEST_LENGTH];
		create(data, dataLength, md);
		return (memcmp(md, messageDigest, length()) == 0);
	} else {
		return false;
	}
}

unsigned int Sha::length() const noexcept {
	return _length;
}

} /* namespace wanhive */
