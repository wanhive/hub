/*
 * PKI.cpp
 *
 * Asymmetric cryptography
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

PKI::PKI(const char *privateKey, const char *publicKey) {
	if (!setup(privateKey, publicKey)) {
		throw Exception(EX_SECURITY);
	}
}

PKI::~PKI() {

}

bool PKI::setup(const char *privateKey, const char *publicKey) noexcept {
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

bool PKI::encrypt(const Data &plaintext, Cache &ciphertext) noexcept {
	return (plaintext.length <= PAYLOAD_LENGTH)
			&& (!ciphertext.base || ciphertext.length >= ENCRYPTED_LENGTH)
			&& rsa.encrypt(plaintext.base, plaintext.length, ciphertext.base,
					ciphertext.length);
}

bool PKI::decrypt(const Data &ciphertext, Cache &plaintext) noexcept {
	return (!plaintext.base || plaintext.length >= ENCODING_LENGTH)
			&& (ciphertext.base && ciphertext.length == ENCRYPTED_LENGTH)
			&& rsa.decrypt(ciphertext.base, ciphertext.length, plaintext.base,
					plaintext.length);
}

bool PKI::sign(const Data &message, Cache &signature) noexcept {
	return (!signature.base || signature.length >= SIGNATURE_LENGTH)
			&& rsa.sign(message.base, message.length, signature.base,
					signature.length);
}

bool PKI::verify(const Data &message, const Data &signature) noexcept {
	return (signature.base) && (signature.length == SIGNATURE_LENGTH)
			&& rsa.verify(message.base, message.length, signature.base,
					signature.length);
}

size_t PKI::payload() const noexcept {
	return PAYLOAD_LENGTH;
}

size_t PKI::fingerprint(bool &fixed) const noexcept {
	fixed = true;
	return SIGNATURE_LENGTH;
}

int PKI::algorithm() const noexcept {
	return rsa.type();
}

void PKI::generate(const char *privateKey, const char *publicKey) {
	if (!Rsa { }.generate(privateKey, publicKey, KEY_LENGTH)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
