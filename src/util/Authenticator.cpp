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
		Srp(SRP_3072, WH_SHA512), id(0), isHost(isHost), authenticated(false) {
}

Authenticator::~Authenticator() {

}

bool Authenticator::identify(unsigned long long identity,
		const unsigned char *nonce, unsigned int nonceLength, const char *salt,
		const char *verifier) noexcept {
	if (!isHost) {
		return false;
	}

	auto success = initialize() && loadSalt(salt)
			&& loadPasswordVerifier(verifier) && loadHostSecret()
			&& loadHostNonce() && loadUserNonce(nonce, nonceLength)
			&& loadRandomScramblingParameter() && loadSessionKey(true)
			&& generateUserEvidence() && generateHostEvidence();

	if (success) {
		this->id = identity;
	}

	return success;
}

bool Authenticator::createIdentity(unsigned long long identity,
		const unsigned char *password, unsigned int passwordLength,
		const unsigned char *salt, unsigned int saltLength,
		const unsigned char *nonce, unsigned int nonceLength,
		unsigned int rounds) noexcept {
	if (isHost || !password || !passwordLength || !salt || !saltLength || !nonce
			|| !nonceLength || saltLength > groupSize()
			|| nonceLength > groupSize()) {
		return false;
	}

	char identityString[32];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "%llu", identity);
	if (!rounds) {
		rounds = DEFAULT_ROUNDS;
	}
	auto success = loadSalt(salt, saltLength)
			&& loadPrivateKey(identityString, password, passwordLength, rounds)
			&& loadPasswordVerifier() && loadHostNonce(nonce, nonceLength)
			&& loadRandomScramblingParameter() && loadSessionKey(false)
			&& generateUserEvidence() && generateHostEvidence();

	if (success) {
		this->id = identity;
	}

	return success;
}

bool Authenticator::authenticateUser(const unsigned char *proof,
		unsigned int length) noexcept {
	if (!isHost || !proof || !length || (length != keySize())) {
		return false;
	} else if (verifyUserProof(proof, length)) {
		authenticated = true;
		return true;
	} else {
		authenticated = false;
		return false;
	}
}

bool Authenticator::authenticateHost(const unsigned char *proof,
		unsigned int length) noexcept {
	if (isHost || !proof || !length || (length != keySize())) {
		return false;
	} else if (verifyHostProof(proof, length)) {
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

bool Authenticator::generateNonce(const unsigned char *&binary,
		unsigned int &bytes) noexcept {
	if (isHost) {
		Srp::getHostNonce(binary, bytes);
		return true;
	} else {
		return initialize() && loadUserSecret() && loadUserNonce()
				&& (getUserNonce(binary, bytes), true);
	}
}

bool Authenticator::generateUserProof(const unsigned char *&binary,
		unsigned int &bytes) noexcept {
	return (getUserProof(binary, bytes), true);
}

bool Authenticator::generateHostProof(const unsigned char *&binary,
		unsigned int &bytes) noexcept {
	return (getHostProof(binary, bytes), true);
}

void Authenticator::getSalt(const unsigned char *&binary,
		unsigned int &bytes) noexcept {
	Srp::getSalt(binary, bytes);
}

void Authenticator::getPasswordVerifier(const unsigned char *&binary,
		unsigned int &bytes) noexcept {
	Srp::getPasswordVerifier(binary, bytes);
}

bool Authenticator::generateVerifier(const char *identity,
		const unsigned char *password, unsigned int passwordLength,
		unsigned int rounds) noexcept {
	if (!rounds) {
		rounds = DEFAULT_ROUNDS;
	}
	return initialize() && loadSalt()
			&& loadPrivateKey(identity, password, passwordLength, rounds)
			&& loadPasswordVerifier();
}

void Authenticator::generateFakeNonce(const unsigned char *&binary,
		unsigned int &bytes) noexcept {
	if (initialize() && Srp::generateFakeNonce()) {
		Srp::getFakeNonce(binary, bytes);
	} else {
		binary = nullptr;
		bytes = 0;
	}
}

void Authenticator::generateFakeSalt(unsigned long long identity,
		const unsigned char *&s, unsigned int &sLength) noexcept {
	char identityString[32];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "@*%llu*@", identity);

	if (Srp::generateFakeSalt(identityString, s, sLength)) {
		Srp::getFakeSalt(s, sLength);
	} else {
		s = nullptr;
		sLength = 0;
	}
}

} /* namespace wanhive */
