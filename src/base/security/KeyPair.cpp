/*
 * KeyPair.cpp
 *
 * Key pair for asymmetric cryptography
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "KeyPair.h"
#include "../Storage.h"
#include <openssl/pem.h>

namespace wanhive {

KeyPair::KeyPair(int nid, const char *name) noexcept :
		nid { nid }, name { name } {

}

KeyPair::~KeyPair() {
	reset();
}

bool KeyPair::setup(const char *privateKey, const char *publicKey, char *secret,
		bool memory) noexcept {
	auto success = loadPublicKey(publicKey, memory)
			&& loadPrivateKey(privateKey, secret, memory);
	if (success) {
		return true;
	} else {
		reset();
		return false;
	}
}

void KeyPair::reset() noexcept {
	freePublicKey();
	freePrivateKey();
}

bool KeyPair::loadPrivateKey(const char *privateKey, char *secret,
		bool memory) noexcept {
	freePrivateKey();
	if (!privateKey) {
		return true;
	} else if (memory) {
		_private = fromMemory(privateKey, false, secret);
	} else {
		_private = fromFile(privateKey, false, secret);
	}
	return _private != nullptr;
}

bool KeyPair::loadPublicKey(const char *publicKey, bool memory) noexcept {
	freePublicKey();
	if (!publicKey) {
		return true;
	} else if (memory) {
		_public = fromMemory(publicKey, true, nullptr);
	} else {
		_public = fromFile(publicKey, true, nullptr);
	}
	return _public != nullptr;
}

void KeyPair::freePrivateKey() noexcept {
	if (_private != _public) {
		EVP_PKEY_free(_private);
	}
	_private = nullptr;
}

void KeyPair::freePublicKey() noexcept {
	if (_public != _private) {
		EVP_PKEY_free(_public);
	}
	_public = nullptr;
}

bool KeyPair::hasPrivateKey() const noexcept {
	return _private != nullptr;
}

bool KeyPair::hasPublicKey() const noexcept {
	return _public != nullptr;
}

bool KeyPair::validate(const EVP_PKEY *pkey) const noexcept {
	return pkey && (EVP_PKEY_get_base_id(pkey) == nid);
}

bool KeyPair::isPrivateKey(EVP_PKEY *pkey) const noexcept {
	EVP_PKEY_CTX *ctx { };
	auto status = validate(pkey) && (ctx = EVP_PKEY_CTX_new(pkey, nullptr))
			&& (EVP_PKEY_private_check(ctx) == 1);
	EVP_PKEY_CTX_free(ctx);
	return status;
}

bool KeyPair::isPublicKey(EVP_PKEY *pkey) const noexcept {
	EVP_PKEY_CTX *ctx { };
	auto status = validate(pkey) && (ctx = EVP_PKEY_CTX_new(pkey, nullptr))
			&& (EVP_PKEY_public_check(ctx) == 1);
	EVP_PKEY_CTX_free(ctx);
	return status;
}

EVP_PKEY* KeyPair::getPrivateKey() const noexcept {
	return _private;
}

bool KeyPair::setPrivateKey(EVP_PKEY *pkey) noexcept {
	if (pkey == _private) {
		return true;
	} else if (!pkey || isPrivateKey(pkey)) {
		freePrivateKey();
		_private = pkey;
		return true;
	} else {
		return false;
	}
}

EVP_PKEY* KeyPair::getPublicKey() const noexcept {
	return _public;
}

bool KeyPair::setPublicKey(EVP_PKEY *pkey) noexcept {
	if (pkey == _public) {
		return true;
	} else if (!pkey || isPublicKey(pkey)) {
		freePublicKey();
		_public = pkey;
		return true;
	} else {
		return false;
	}
}

EVP_PKEY* KeyPair::generate(size_t bits) const noexcept {
	if (!name) {
		return nullptr;
	} else if (bits) {
		return EVP_PKEY_Q_keygen(nullptr, nullptr, name, bits);
	} else {
		return EVP_PKEY_Q_keygen(nullptr, nullptr, name);
	}
}

EVP_PKEY* KeyPair::generate(size_t bits) noexcept {
	EVP_PKEY *pkey { };
	if (!name) {
		return nullptr;
	} else if (bits) {
		pkey = EVP_PKEY_Q_keygen(nullptr, nullptr, name, bits);
	} else {
		pkey = EVP_PKEY_Q_keygen(nullptr, nullptr, name);
	}

	if (pkey) {
		reset();
		_private = _public = pkey;
	}

	return pkey;
}

bool KeyPair::generate(const char *privateKey, const char *publicKey,
		size_t bits, char *secret, const EVP_CIPHER *cipher) const noexcept {
	if (!privateKey || !publicKey) {
		return false;
	}

	auto pkey = generate(bits);
	if (!pkey) {
		return false;
	}

	auto status = store(privateKey, pkey, false, secret, cipher)
			&& store(publicKey, pkey, true, nullptr, nullptr);
	EVP_PKEY_free(pkey);
	return status;
}

bool KeyPair::store(const char *path, EVP_PKEY *pkey, bool isPublic,
		char *secret, const EVP_CIPHER *cipher) noexcept {
	if (!path || !pkey) {
		return false;
	}

	auto file = Storage::openStream(path, "w");
	if (!file) {
		return false;
	}

	int ret = 0;
	if (isPublic) {
		ret = PEM_write_PUBKEY(file, pkey);
	} else {
		cipher = secret ? ((cipher ? cipher : EVP_aes_256_cbc())) : nullptr;
		auto slen = secret ? strlen(secret) : 0;
		ret = PEM_write_PrivateKey(file, pkey, cipher,
				(const unsigned char*) secret, slen, nullptr, nullptr);
	}

	Storage::closeStream(file);
	return (ret == 1);
}

EVP_PKEY* KeyPair::fromMemory(const char *key, bool isPublicKey,
		char *secret) noexcept {
	if (!key) {
		return nullptr;
	}

	auto keybio = BIO_new_mem_buf(key, -1);
	if (keybio == nullptr) {
		return nullptr;
	}

	EVP_PKEY *pkey = nullptr;
	if (isPublicKey) {
		pkey = PEM_read_bio_PUBKEY(keybio, nullptr, nullptr, nullptr);
	} else {
		pkey = PEM_read_bio_PrivateKey(keybio, nullptr, 0, secret);
	}

	if (!validate(pkey)) {
		EVP_PKEY_free(pkey);
		pkey = nullptr;
	}

	BIO_free_all(keybio);
	return pkey;
}

EVP_PKEY* KeyPair::fromFile(const char *path, bool isPublic,
		char *secret) noexcept {
	if (Storage::testFile(path) != 1) {
		return nullptr;
	}

	auto fp = Storage::openStream(path, "r");
	if (fp == nullptr) {
		return nullptr;
	}

	EVP_PKEY *pkey = nullptr;
	if (isPublic) {
		pkey = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
	} else {
		pkey = PEM_read_PrivateKey(fp, nullptr, 0, secret);
	}

	if (!validate(pkey)) {
		EVP_PKEY_free(pkey);
		pkey = nullptr;
	}

	Storage::closeStream(fp);
	return pkey;
}

} /* namespace wanhive */
