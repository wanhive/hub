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
#include "../base/security/CryptoUtils.h"

namespace wanhive {

PKI::PKI() noexcept {

}

PKI::~PKI() {

}

bool PKI::initialize(const char *hostKey, const char *publicKey) noexcept {
	return rsa.setup(hostKey, publicKey);
}

bool PKI::loadHostKey(const char *hostKey) noexcept {
	return rsa.loadPrivateKey(hostKey) || !hostKey;
}

bool PKI::loadPublicKey(const char *publicKey) noexcept {
	return rsa.loadPublicKey(publicKey) || !publicKey;
}

bool PKI::hasHostKey() const noexcept {
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

void PKI::generate(const char *hostKey, const char *publicKey) {
	if (!Rsa { }.generate(hostKey, publicKey, KEY_LENGTH)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
