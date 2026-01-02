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
		KeyPair { EVP_PKEY_ED25519, "ED25519" } {

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

bool Ed25519::generate(const char *privateKey, const char *publicKey,
		char *secret) noexcept {
	return KeyPair::generate(privateKey, publicKey, 0, secret, nullptr);
}

EVP_MD_CTX* Ed25519::mdContext() noexcept {
	if (mdctx) {
		EVP_MD_CTX_reset(mdctx);
	} else {
		mdctx = EVP_MD_CTX_new();
	}

	return mdctx;
}

} /* namespace wanhive */
