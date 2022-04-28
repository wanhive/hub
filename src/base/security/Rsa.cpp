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
#include "../Storage.h"
#include <cstring>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

namespace wanhive {

Rsa::Rsa() noexcept :
		_public { }, _private { } {

}

Rsa::~Rsa() {
	reset();
}

bool Rsa::init(const char *privateKey, const char *publicKey, bool fromFile,
		char *password) noexcept {
	loadPublicKey(publicKey, fromFile);
	loadPrivateKey(privateKey, fromFile, password);

	return (!privateKey || _private) && (!publicKey || _public);
}

void Rsa::reset() noexcept {
	freePublicKey();
	freePrivateKey();
}

bool Rsa::loadPrivateKey(const char *privateKey, bool fromFile,
		char *password) noexcept {
	freePrivateKey();
	if (fromFile) {
		_private = createFromFile(privateKey, false, password);
	} else {
		_private = create(privateKey, false, password);
	}
	return _private != nullptr;
}

bool Rsa::loadPublicKey(const char *publicKey, bool fromFile) noexcept {
	freePublicKey();
	if (fromFile) {
		_public = createFromFile(publicKey, true, nullptr);
	} else {
		_public = create(publicKey, true, nullptr);
	}
	return _public != nullptr;
}

void Rsa::freePrivateKey() noexcept {
	destroyKey(_private);
	_private = nullptr;
}

void Rsa::freePublicKey() noexcept {
	destroyKey(_public);
	_public = nullptr;
}

bool Rsa::hasPrivateKey() const noexcept {
	return _private != nullptr;
}

bool Rsa::hasPublicKey() const noexcept {
	return _public != nullptr;
}

bool Rsa::encrypt(const unsigned char *data, unsigned int dataLength,
		unsigned char *encrypted, unsigned int *encryptedLength) const noexcept {
	if (_public && (!dataLength || data) && encryptedLength) {
		auto ctx = EVP_PKEY_CTX_new(_public, nullptr);
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
	if (_private && (!dataLength || data) && decryptedLength) {
		auto ctx = EVP_PKEY_CTX_new(_private, nullptr);
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
	if (_private && (!dataLength || data) && signatureLength) {
		unsigned char md[SHA_DIGEST_LENGTH];
		unsigned int mdLength = 0;
		auto type = EVP_sha1();

		if (EVP_Digest(data, dataLength, md, &mdLength, type, nullptr) <= 0) {
			return false;
		}

		auto ctx = EVP_PKEY_CTX_new(_private, nullptr);
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
		unsigned char *signature, unsigned int signatureLength) const noexcept {
	if (_public && (!dataLength || data) && signature) {
		unsigned char md[SHA_DIGEST_LENGTH];
		unsigned int mdLength = 0;
		auto type = EVP_sha1();

		if (EVP_Digest(data, dataLength, md, &mdLength, type, nullptr) <= 0) {
			return false;
		}

		auto ctx = EVP_PKEY_CTX_new(_public, nullptr);
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

bool Rsa::generateKeyPair(const char *privateKeyFile, const char *publicKeyFile,
		int bits, char *password) noexcept {
	if (!privateKeyFile || !publicKeyFile) {
		return false;
	}

	auto rsa = generateRSAKey(bits);
	if (!rsa) {
		return false;
	}

	auto status = generatePem(privateKeyFile, rsa, false, password, nullptr)
			&& generatePem(publicKeyFile, rsa, true, nullptr, nullptr);
	EVP_PKEY_free(rsa);
	return status;
}

EVP_PKEY* Rsa::create(const char *key, bool isPublicKey,
		char *password) noexcept {
	if (!key) {
		return nullptr;
	}

	auto keybio = BIO_new_mem_buf(key, -1);
	if (keybio == nullptr) {
		return nullptr;
	}

	EVP_PKEY *rsa = nullptr;
	if (isPublicKey) {
		rsa = PEM_read_bio_PUBKEY(keybio, nullptr, nullptr, nullptr);
	} else {
		rsa = PEM_read_bio_PrivateKey(keybio, nullptr, 0, password);
	}

	BIO_free_all(keybio);
	return rsa;
}

EVP_PKEY* Rsa::createFromFile(const char *filename, bool isPublicKey,
		char *password) noexcept {
	if (Storage::testFile(filename) != 1) {
		return nullptr;
	}

	//Open file for reading in text mode
	auto fp = Storage::openStream(filename, "r", false);
	if (fp == nullptr) {
		return nullptr;
	}

	EVP_PKEY *rsa = nullptr;
	if (isPublicKey) {
		rsa = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
	} else {
		rsa = PEM_read_PrivateKey(fp, nullptr, 0, password);
	}

	Storage::closeStream(fp);
	return rsa;
}

void Rsa::destroyKey(EVP_PKEY *rsa) noexcept {
	EVP_PKEY_free(rsa);
}

EVP_PKEY* Rsa::generateRSAKey(int bits) noexcept {
	return EVP_RSA_gen(bits);
}

EVP_PKEY* Rsa::generatePrivateKey(RSA *rsa) noexcept {
	if (!rsa) {
		return nullptr;
	}

	auto pkey = EVP_PKEY_new();
	if (!pkey || !EVP_PKEY_assign_RSA(pkey, rsa)) {
		EVP_PKEY_free(pkey);
		return nullptr;
	}

	//Return the private key structure
	return pkey;
}

bool Rsa::generatePem(const char *filename, EVP_PKEY *pKey, bool isPublicKey,
		char *password, const EVP_CIPHER *cipher) noexcept {
	if (!filename || !pKey) {
		return false;
	}

	auto pCipher = password ? ((cipher ? cipher : EVP_aes_256_cbc())) : nullptr;
	auto passPhraseLength = password ? strlen(password) : 0;

	//Open the file for writing in text mode
	auto pFile = Storage::openStream(filename, "w", true);
	if (!pFile) {
		return false;
	}

	int ret = 0;
	if (isPublicKey) {
		ret = PEM_write_PUBKEY(pFile, pKey);
	} else {
		ret = PEM_write_PrivateKey(pFile, pKey, pCipher,
				(unsigned char*) password, passPhraseLength, nullptr, nullptr);
	}

	Storage::closeStream(pFile);
	return (bool) ret;
}

} /* namespace wanhive */
