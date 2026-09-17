/*
 * MLDsa.cpp
 *
 * ML-DSA cipher
 *
 *
 * Copyright (C) 2026 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "MLDsa.h"
namespace {

constexpr const int NIDS[] = {
NID_ML_DSA_44,
NID_ML_DSA_65,
NID_ML_DSA_87 };

constexpr const char *NAMES[] = { "ML-DSA-44", "ML-DSA-65", "ML-DSA-87" };

}  // namespace
namespace wanhive {

MLDsa::MLDsa(MLDsaParam param) noexcept :
		KeyPair { EVP_PKEY_NONE, NAMES[param] } {

}

MLDsa::~MLDsa() {
	EVP_MD_CTX_free(mdctx);
}

bool MLDsa::sign(const unsigned char *data, size_t dataLength,
		unsigned char *signature, size_t &signatureLength) noexcept {
	if (getPrivateKey() && (!dataLength || data)) {
		auto len = signatureLength;
		auto ctx = mdContext();
		//Sign the data in a single shot
		auto success = (ctx != nullptr)
				&& (EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr,
						getPrivateKey()) == 1)
				&& (EVP_DigestSign(ctx, signature, &len, data, dataLength) == 1)
				&& (signatureLength = len);
		return success;
	} else {
		return false;
	}
}

bool MLDsa::verify(const unsigned char *data, size_t dataLength,
		const unsigned char *signature, size_t signatureLength) noexcept {
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

bool MLDsa::generate(const char *privateKey, const char *publicKey,
		char *secret) noexcept {
	return KeyPair::generate(privateKey, publicKey, 0, secret, nullptr);
}

EVP_MD_CTX* MLDsa::mdContext() noexcept {
	if (mdctx) {
		EVP_MD_CTX_reset(mdctx);
	} else {
		mdctx = EVP_MD_CTX_new();
	}

	return mdctx;
}

} /* namespace wanhive */
