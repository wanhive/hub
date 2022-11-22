/*
 * Authenticator.cpp
 *
 * SRP-6a based mutual authenticator
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Authenticator.h"
#include <cstring>

namespace {
constexpr unsigned int DEFAULT_ROUNDS = 1;

}  // namespace

namespace wanhive {

Authenticator::Authenticator(bool isHost) noexcept :
		Srp(SRP_3072, WH_SHA512), isHost(isHost) {
}

Authenticator::~Authenticator() {

}

bool Authenticator::identify(unsigned long long identity, const char *verifier,
		const char *salt, const Data &nonce) noexcept {
	if (!isHost) {
		return false;
	}

	auto success = initialize() && loadSalt(salt)
			&& loadPasswordVerifier(verifier) && loadHostSecret()
			&& loadHostNonce() && loadUserNonce(nonce.base, nonce.length)
			&& loadRandomScramblingParameter() && loadSessionKey(true)
			&& generateUserEvidence() && generateHostEvidence();

	if (success) {
		this->id = identity;
	}

	return success;
}

bool Authenticator::createIdentity(unsigned long long identity,
		const Data &password, const Data &salt, const Data &nonce,
		unsigned int rounds) noexcept {
	if (isHost || !password.base || !password.length || !salt.base
			|| !salt.length || !nonce.base || !nonce.length
			|| salt.length > groupSize() || nonce.length > groupSize()) {
		return false;
	}

	char identityString[32];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "%llu", identity);
	if (!rounds) {
		rounds = DEFAULT_ROUNDS;
	}
	auto success = loadSalt(salt.base, salt.length)
			&& loadPrivateKey(identityString, password.base, password.length,
					rounds) && loadPasswordVerifier()
			&& loadHostNonce(nonce.base, nonce.length)
			&& loadRandomScramblingParameter() && loadSessionKey(false)
			&& generateUserEvidence() && generateHostEvidence();

	if (success) {
		this->id = identity;
	}

	return success;
}

bool Authenticator::authenticateUser(const Data &proof) noexcept {
	if (!isHost || !proof.base || !proof.length
			|| (proof.length != keySize())) {
		return false;
	} else if (verifyUserProof(proof.base, proof.length)) {
		authenticated = true;
		return true;
	} else {
		authenticated = false;
		return false;
	}
}

bool Authenticator::authenticateHost(const Data &proof) noexcept {
	if (isHost || !proof.base || !proof.length || (proof.length != keySize())) {
		return false;
	} else if (verifyHostProof(proof.base, proof.length)) {
		authenticated = true;
		return true;
	} else {
		authenticated = false;
		return false;
	}
}

bool Authenticator::isAuthenticated() const noexcept {
	return authenticated;
}

unsigned long long Authenticator::getIdentity() const noexcept {
	return id;
}

bool Authenticator::generateNonce(Data &nonce) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	if (isHost) {
		Srp::getHostNonce(bytes, length);
		nonce = { bytes, length };
		return true;
	} else {
		auto ret = initialize() && loadUserSecret() && loadUserNonce();
		getUserNonce(bytes, length);
		nonce = { bytes, length };
		return ret;
	}
}

bool Authenticator::generateUserProof(Data &proof) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	getUserProof(bytes, length);
	proof = { bytes, length };
	return true;
}

bool Authenticator::generateHostProof(Data &proof) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	getHostProof(bytes, length);
	proof = { bytes, length };
	return true;
}

void Authenticator::getSalt(Data &salt) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	Srp::getSalt(bytes, length);
	salt = { bytes, length };
}

void Authenticator::getPasswordVerifier(Data &verifier) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	Srp::getPasswordVerifier(bytes, length);
	verifier = { bytes, length };
}

bool Authenticator::generateVerifier(const char *identity, const Data &password,
		unsigned int rounds) noexcept {
	if (!rounds) {
		rounds = DEFAULT_ROUNDS;
	}
	return initialize() && loadSalt()
			&& loadPrivateKey(identity, password.base, password.length, rounds)
			&& loadPasswordVerifier();
}

void Authenticator::generateFakeNonce(Data &nonce) noexcept {
	if (initialize() && Srp::generateFakeNonce()) {
		const unsigned char *bytes { };
		unsigned int length { };
		Srp::getFakeNonce(bytes, length);
		nonce = { bytes, length };
	} else {
		nonce = { nullptr, 0 };
	}
}

void Authenticator::generateFakeSalt(unsigned long long identity,
		Data &salt) noexcept {
	char identityString[32];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "@*%llu*@", identity);

	if (Srp::generateFakeSalt(identityString, salt.base, salt.length)) {
		const unsigned char *bytes { };
		unsigned int length { };
		Srp::getFakeSalt(bytes, length);
		salt = { bytes, length };
	} else {
		salt = { nullptr, 0 };
	}
}

} /* namespace wanhive */
