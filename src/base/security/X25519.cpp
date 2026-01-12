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

}

bool X25519::compute(unsigned char *data, unsigned int *bytes) noexcept {
	EVP_PKEY_CTX *ctx { };
	unsigned char *buf { };
	size_t len { bytes ? *bytes : 0 };
	auto success = (bytes != nullptr)
			&& (ctx = EVP_PKEY_CTX_new(getPrivateKey(), nullptr))
			&& (EVP_PKEY_derive_init(ctx) == 1)
			&& (EVP_PKEY_derive_set_peer(ctx, getPublicKey()) == 1)
			&& (EVP_PKEY_derive(ctx, buf, &len)) && (*bytes = len);
	EVP_PKEY_CTX_free(ctx);
	return success;
}

bool X25519::generate(const char *privateKey, const char *publicKey,
		char *secret) noexcept {
	return KeyPair::generate(privateKey, publicKey, 0, secret, nullptr);
}

} /* namespace wanhive */
