/*
 * PKI.cpp
 *
 * RSA based asymmetric cryptography
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

bool PKI::initialize(const char *hostKey, const char *publicKey,
		bool fromFile) noexcept {
	return rsa.init(hostKey, publicKey, fromFile, nullptr);
}

bool PKI::loadPublicKey(const char *publicKey, bool fromFile) noexcept {
	return rsa.loadPublicKey(publicKey, fromFile) || !publicKey;
}

bool PKI::loadHostKey(const char *hostKey, bool fromFile) noexcept {
	return rsa.loadPrivateKey(hostKey, fromFile, nullptr) || !hostKey;
}

bool PKI::hasPublicKey() const noexcept {
	return rsa.hasPublicKey();
}

bool PKI::hasHostKey() const noexcept {
	return rsa.hasPrivateKey();
}

bool PKI::encrypt(const void *block, unsigned int size,
		PKIEncryptedData *target) const noexcept {
	unsigned int encLen = ENCRYPTED_LENGTH;
	return (size <= MAX_PT_LEN)
			&& rsa.encrypt((const unsigned char*) block, size,
					(unsigned char*) target, &encLen);
}

bool PKI::decrypt(const PKIEncryptedData *block, void *result,
		unsigned int *size) const noexcept {
	unsigned int decLen = ENCODING_LENGTH;
	auto ret = rsa.decrypt((const unsigned char*) block, ENCRYPTED_LENGTH,
			(unsigned char*) result, &decLen);
	if (!ret) {
		return false;
	} else if (size) {
		*size = decLen;
		return true;
	} else {
		return true;
	}
}

bool PKI::sign(const void *block, unsigned int size,
		Signature *sig) const noexcept {
	unsigned int sigLen = SIGNATURE_LENGTH;
	return rsa.sign((const unsigned char*) block, size, (unsigned char*) sig,
			&sigLen) && (sigLen == SIGNATURE_LENGTH);
}

bool PKI::verify(const void *block, unsigned int len,
		const Signature *sig) const noexcept {
	return rsa.verify((unsigned char*) block, len, (unsigned char*) sig,
			SIGNATURE_LENGTH);
}

void PKI::generateKeyPair(const char *hostKey, const char *publicKey) {
	if (!Rsa::generateKeyPair(hostKey, publicKey, KEY_LENGTH)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
