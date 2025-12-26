/*
 * Ed25519.cpp
 *
 * Ed25519 cipher
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Ed25519.h"

namespace wanhive {

Ed25519::Ed25519() noexcept :
		KeyPair { EVP_PKEY_ED25519 } {

}

Ed25519::~Ed25519() {
	EVP_MD_CTX_free(mdctx);
}

bool Ed25519::sign(const unsigned char *data, unsigned int dataLength,
		unsigned char *signature, unsigned int *signatureLength) noexcept {
	if (getPrivateKey() && (!dataLength || data) && signatureLength) {
		size_t len = *signatureLength;
		auto ctx = mdContext();
		//Sign the data in a single shot
		auto success = (ctx != nullptr)
				&& (EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr,
						getPrivateKey()) == 1)
				&& (EVP_DigestSign(ctx, signature, &len, data, dataLength) == 1)
				&& (*signatureLength = len);
		return success;
	} else {
		return false;
	}
}

bool Ed25519::verify(const unsigned char *data, unsigned int dataLength,
		const unsigned char *signature, unsigned int signatureLength) noexcept {
	if (getPublicKey() && (!dataLength || data) && signature) {
		auto ctx = mdContext();
		//Verify the data in a single shot
		auto success = (ctx != nullptr)
				&& (EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr,
						getPublicKey()) == 1)
				&& (EVP_DigestVerify(ctx, signature, signatureLength, data,
						dataLength) == 1);
		return success;
	} else {
		return false;
	}
}

bool Ed25519::generate(const char *privateKeyFile, const char *publicKeyFile,
		char *secret) noexcept {
	if (!privateKeyFile || !publicKeyFile) {
		return false;
	}

	auto pkey = generate();
	if (!pkey) {
		return false;
	}

	auto status = store(privateKeyFile, pkey, false, secret, nullptr)
			&& store(publicKeyFile, pkey, true, nullptr, nullptr);
	EVP_PKEY_free(pkey);
	return status;
}

EVP_MD_CTX* Ed25519::mdContext() noexcept {
	if (mdctx) {
		EVP_MD_CTX_reset(mdctx);
	} else {
		mdctx = EVP_MD_CTX_new();
	}

	return mdctx;
}

EVP_PKEY* Ed25519::generate() noexcept {
	EVP_PKEY_CTX *ctx { };
	EVP_PKEY *pkey { };
	auto success = (ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr))
			&& (EVP_PKEY_keygen_init(ctx) == 1)
			&& (EVP_PKEY_keygen(ctx, &pkey) == 1);
	EVP_PKEY_CTX_free(ctx);
	if (success && pkey) {
		return pkey;
	} else {
		return nullptr;
	}
}

} /* namespace wanhive */
