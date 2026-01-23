/*
 * PKI.cpp
 *
 * Asymmetric cryptography facility
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "PKI.h"
#include "../base/common/Exception.h"

namespace wanhive {

PKI::PKI() noexcept {

}

PKI::~PKI() {

}

bool PKI::initialize(const char *privateKey, const char *publicKey) noexcept {
	return rsa.setup(privateKey, publicKey);
}

bool PKI::loadPrivateKey(const char *key) noexcept {
	return rsa.loadPrivateKey(key) || !key;
}

bool PKI::loadPublicKey(const char *key) noexcept {
	return rsa.loadPublicKey(key) || !key;
}

bool PKI::hasPrivateKey() const noexcept {
	return rsa.hasPrivateKey();
}

bool PKI::hasPublicKey() const noexcept {
	return rsa.hasPublicKey();
}

bool PKI::encrypt(const void *plaintext, unsigned int size,
		CipherText *ciphertext) noexcept {
	auto len = ENCRYPTED_LENGTH;
	return (size <= MAX_PT_LEN)
			&& rsa.encrypt((const unsigned char*) plaintext, size,
					(unsigned char*) ciphertext, len);
}

bool PKI::decrypt(const CipherText *ciphertext, void *plaintext,
		unsigned int *size) noexcept {
	auto len = ENCODING_LENGTH;
	auto ret = rsa.decrypt((const unsigned char*) ciphertext, ENCRYPTED_LENGTH,
			(unsigned char*) plaintext, len);
	if (!ret) {
		return false;
	} else if (size) {
		*size = len;
		return true;
	} else {
		return true;
	}
}

bool PKI::sign(const void *data, unsigned int size,
		Signature *signature) noexcept {
	auto len = SIGNATURE_LENGTH;
	return rsa.sign((const unsigned char*) data, size,
			(unsigned char*) signature, len) && (len == SIGNATURE_LENGTH);
}

bool PKI::verify(const void *data, unsigned int size,
		const Signature *signature) noexcept {
	return rsa.verify((unsigned char*) data, size, (unsigned char*) signature,
			SIGNATURE_LENGTH);
}

void PKI::generate(const char *privateKey, const char *publicKey) {
	if (!Rsa { }.generate(privateKey, publicKey, KEY_LENGTH)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
