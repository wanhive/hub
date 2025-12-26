/*
 * Rsa.cpp
 *
 * RSA cipher
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Rsa.h"
#include <openssl/pem.h>

namespace wanhive {

Rsa::Rsa() noexcept :
		KeyPair { EVP_PKEY_RSA } {

}

Rsa::~Rsa() {

}

bool Rsa::encrypt(const unsigned char *data, unsigned int dataLength,
		unsigned char *encrypted, unsigned int *encryptedLength) const noexcept {
	if (getPublicKey() && (!dataLength || data) && encryptedLength) {
		auto ctx = EVP_PKEY_CTX_new(getPublicKey(), nullptr);
		if (!ctx) {
			return false;
		}

		if (EVP_PKEY_encrypt_init(ctx) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		size_t len = *encryptedLength;
		if (EVP_PKEY_encrypt(ctx, encrypted, &len, data, dataLength) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		*encryptedLength = len;

		EVP_PKEY_CTX_free(ctx);
		return true;
	} else {
		return false;
	}

}

bool Rsa::decrypt(const unsigned char *data, unsigned int dataLength,
		unsigned char *decrypted, unsigned int *decryptedLength) const noexcept {
	if (getPrivateKey() && (!dataLength || data) && decryptedLength) {
		auto ctx = EVP_PKEY_CTX_new(getPrivateKey(), nullptr);
		if (!ctx) {
			return false;
		}

		if (EVP_PKEY_decrypt_init(ctx) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		size_t len = *decryptedLength;
		if (EVP_PKEY_decrypt(ctx, decrypted, &len, data, dataLength) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		*decryptedLength = len;

		EVP_PKEY_CTX_free(ctx);
		return true;
	} else {
		return false;
	}
}

bool Rsa::sign(const unsigned char *data, unsigned int dataLength,
		unsigned char *signature, unsigned int *signatureLength) const noexcept {
	if (getPrivateKey() && (!dataLength || data) && signatureLength) {
		unsigned char md[SHA_DIGEST_LENGTH];
		unsigned int mdLength = 0;
		auto type = EVP_sha1();

		if (EVP_Digest(data, dataLength, md, &mdLength, type, nullptr) <= 0) {
			return false;
		}

		auto ctx = EVP_PKEY_CTX_new(getPrivateKey(), nullptr);
		if (!ctx) {
			return false;
		}

		if (EVP_PKEY_sign_init(ctx) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		if (EVP_PKEY_CTX_set_signature_md(ctx, type) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		size_t len = *signatureLength;
		if (EVP_PKEY_sign(ctx, signature, &len, md, mdLength) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		*signatureLength = len;

		EVP_PKEY_CTX_free(ctx);
		return true;
	} else {
		return false;
	}
}

bool Rsa::verify(const unsigned char *data, unsigned int dataLength,
		const unsigned char *signature,
		unsigned int signatureLength) const noexcept {
	if (getPublicKey() && (!dataLength || data) && signature) {
		unsigned char md[SHA_DIGEST_LENGTH];
		unsigned int mdLength = 0;
		auto type = EVP_sha1();

		if (EVP_Digest(data, dataLength, md, &mdLength, type, nullptr) <= 0) {
			return false;
		}

		auto ctx = EVP_PKEY_CTX_new(getPublicKey(), nullptr);
		if (!ctx) {
			return false;
		}

		if (EVP_PKEY_verify_init(ctx) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		if (EVP_PKEY_CTX_set_signature_md(ctx, type) <= 0) {
			EVP_PKEY_CTX_free(ctx);
			return false;
		}

		auto ret = EVP_PKEY_verify(ctx, signature, signatureLength, md,
				mdLength);

		EVP_PKEY_CTX_free(ctx);
		return (ret > 0);
	} else {
		return false;
	}
}

bool Rsa::generate(const char *privateKeyFile, const char *publicKeyFile,
		int bits, char *secret) noexcept {
	if (!privateKeyFile || !publicKeyFile) {
		return false;
	}

	auto rsa = EVP_RSA_gen(bits);
	if (!rsa) {
		return false;
	}

	auto status = store(privateKeyFile, rsa, false, secret, nullptr)
			&& store(publicKeyFile, rsa, true, nullptr, nullptr);
	EVP_PKEY_free(rsa);
	return status;
}

} /* namespace wanhive */
