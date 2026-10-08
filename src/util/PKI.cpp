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

PKI::PKI(const char *privateKey, const char *publicKey, char *secret,
		bool text) {
	if (!setup(privateKey, publicKey, secret, text)) {
		throw Exception(EX_SECURITY);
	}
}

PKI::~PKI() {

}

bool PKI::setup(const char *privateKey, const char *publicKey, char *secret,
		bool text) noexcept {
	return rsa.setup(privateKey, publicKey, secret, text);
}

bool PKI::loadPrivateKey(const char *key, char *secret, bool text) noexcept {
	return rsa.loadPrivateKey(key, secret, text);
}

bool PKI::loadPublicKey(const char *key, bool text) noexcept {
	return rsa.loadPublicKey(key, text);
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

void PKI::generate(const char *privateKey, const char *publicKey,
		char *secret) {
	if (!Rsa { }.generate(privateKey, publicKey, KEY_LENGTH, secret)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
