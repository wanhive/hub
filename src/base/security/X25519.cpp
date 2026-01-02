/*
 * X25519.cpp
 *
 * X25519 key exchange
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "X25519.h"

namespace wanhive {

X25519::X25519() noexcept :
		KeyPair { EVP_PKEY_X25519, "X25519" } {

}

X25519::~X25519() {
	clear();
}

bool X25519::compute() noexcept {
	EVP_PKEY_CTX *ctx { };
	unsigned char *buf { };
	size_t len { };
	auto success = (ctx = EVP_PKEY_CTX_new(getPrivateKey(), nullptr))
			&& (EVP_PKEY_derive_init(ctx) == 1)
			&& (EVP_PKEY_derive_set_peer(ctx, getPublicKey()) == 1)
			&& (EVP_PKEY_derive(ctx, nullptr, &len) == 1) && (len == KEY_SIZE)
			&& (buf = resize(len)) && (EVP_PKEY_derive(ctx, buf, &len));
	EVP_PKEY_CTX_free(ctx);
	return success;
}

const unsigned char* X25519::get(size_t &length) const noexcept {
	length = this->bytes;
	return data;
}

bool X25519::generate(const char *privateKey, const char *publicKey,
		char *secret) noexcept {
	return KeyPair::generate(privateKey, publicKey, 0, secret, nullptr);
}

unsigned char* X25519::resize(size_t size) noexcept {
	if (size == bytes) {
		return data;
	}

	auto tmp = OPENSSL_clear_realloc(data, bytes, size);
	if (tmp) {
		data = static_cast<unsigned char*>(tmp);
		bytes = size;
	} else {
		clear();
	}

	return data;
}

void X25519::clear() noexcept {
	OPENSSL_clear_free(data, bytes);
	data = nullptr;
	bytes = 0;
}

} /* namespace wanhive */
