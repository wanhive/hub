/*
 * MLKem.cpp
 *
 * ML-KEM cipher
 *
 *
 * Copyright (C) 2026 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MLKem.h"

namespace {

constexpr const int NIDS[] = {
NID_ML_KEM_512,
NID_ML_KEM_768,
NID_ML_KEM_1024 };

constexpr const char *NAMES[] = { "ML-KEM-512", "ML-KEM-768", "ML-KEM-1024" };

}  // namespace

namespace wanhive {

MLKem::MLKem(MLKemParam param) noexcept:
		KeyPair { EVP_PKEY_NONE, NAMES[param] } {

}

MLKem::~MLKem() {

}

bool MLKem::encapsulate(unsigned char *key, size_t &keyLength,
		unsigned char *wrapped, size_t &wrappedLength) const noexcept {
	EVP_PKEY_CTX *ctx { };
	auto success = (ctx = EVP_PKEY_CTX_new(getPublicKey(), nullptr))
			&& (EVP_PKEY_encapsulate_init(ctx, nullptr) == 1)
			&& (EVP_PKEY_encapsulate(ctx, wrapped, &wrappedLength, key,
					&keyLength) == 1);
	EVP_PKEY_CTX_free(ctx);
	return success;
}

bool MLKem::decapsulate(const unsigned char *wrapped, size_t wrappedLength,
		unsigned char *key, size_t &keyLength) const noexcept {
	EVP_PKEY_CTX *ctx { };
	auto success = (ctx = EVP_PKEY_CTX_new(getPrivateKey(), nullptr))
			&& (EVP_PKEY_decapsulate_init(ctx, nullptr) == 1)
			&& (EVP_PKEY_decapsulate(ctx, key, &keyLength, wrapped,
					wrappedLength) == 1);
	EVP_PKEY_CTX_free(ctx);
	return success;
}

bool MLKem::generate(const char *privateKey, const char *publicKey,
		char *secret) noexcept {
	return KeyPair::generate(privateKey, publicKey, 0, secret, nullptr);
}

} /* namespace wanhive */
