/*
 * Verifier.cpp
 *
 * System for mutual authentication
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Verifier.h"
#include <cstring>

namespace {

constexpr unsigned int DEFAULT_ROUNDS = 1;

}  // namespace

namespace wanhive {

Verifier::Verifier(bool host) noexcept :
		Srp { SRP_3072, WH_SHA512 }, _host { host } {
}

Verifier::~Verifier() {

}

bool Verifier::identify(unsigned long long identity, const char *secret,
		const char *salt, const Data &nonce) noexcept {
	if (!_host) {
		return false;
	}

	auto success = initialize() && loadSalt(salt)
			&& loadPasswordVerifier(secret) && loadHostSecret()
			&& loadHostNonce() && loadUserNonce(nonce.base, nonce.length)
			&& loadRandomScramblingParameter() && loadSessionKey(true)
			&& generateUserEvidence() && generateHostEvidence();

	if (success) {
		this->_identity = identity;
	}

	return success;
}

bool Verifier::scramble(unsigned long long identity, const Data &password,
		const Data &salt, const Data &nonce, unsigned int rounds) noexcept {
	if (_host || !password.base || !password.length || !salt.base
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
		this->_identity = identity;
	}

	return success;
}

bool Verifier::verify(const Data &proof) noexcept {
	if (_host) {
		return verifyUser(proof);
	} else {
		return verifyHost(proof);
	}
}

bool Verifier::verified() const noexcept {
	return _verified;
}

unsigned long long Verifier::identity() const noexcept {
	return _identity;
}

bool Verifier::nonce(Data &data) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	if (_host) {
		Srp::getHostNonce(bytes, length);
		data = { bytes, length };
		return true;
	} else {
		auto ret = initialize() && loadUserSecret() && loadUserNonce();
		getUserNonce(bytes, length);
		data = { bytes, length };
		return ret;
	}
}

bool Verifier::userProof(Data &data) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	getUserProof(bytes, length);
	data = { bytes, length };
	return true;
}

bool Verifier::hostProof(Data &data) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	getHostProof(bytes, length);
	data = { bytes, length };
	return true;
}

void Verifier::salt(Data &data) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	Srp::getSalt(bytes, length);
	data = { bytes, length };
}

void Verifier::secret(Data &data) noexcept {
	const unsigned char *bytes { };
	unsigned int length { };
	Srp::getPasswordVerifier(bytes, length);
	data = { bytes, length };
}

bool Verifier::compute(const char *identity, const Data &password,
		unsigned int rounds) noexcept {
	if (!rounds) {
		rounds = DEFAULT_ROUNDS;
	}
	return initialize() && loadSalt()
			&& loadPrivateKey(identity, password.base, password.length, rounds)
			&& loadPasswordVerifier();
}

void Verifier::fakeNonce(Data &data) noexcept {
	if (initialize() && Srp::generateFakeNonce()) {
		const unsigned char *bytes { };
		unsigned int length { };
		Srp::getFakeNonce(bytes, length);
		data = { bytes, length };
	} else {
		data = { nullptr, 0 };
	}
}

void Verifier::fakeSalt(unsigned long long identity, Data &data) noexcept {
	char identityString[32];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "@*%llu*@", identity);

	if (Srp::generateFakeSalt(identityString, data.base, data.length)) {
		const unsigned char *bytes { };
		unsigned int length { };
		Srp::getFakeSalt(bytes, length);
		data = { bytes, length };
	} else {
		data = { nullptr, 0 };
	}
}

bool Verifier::verifyUser(const Data &proof) noexcept {
	if (!_host || !proof.base || !proof.length || (proof.length != keySize())) {
		return false;
	} else if (verifyUserProof(proof.base, proof.length)) {
		_verified = true;
		return true;
	} else {
		_verified = false;
		return false;
	}
}

bool Verifier::verifyHost(const Data &proof) noexcept {
	if (_host || !proof.base || !proof.length || (proof.length != keySize())) {
		return false;
	} else if (verifyHostProof(proof.base, proof.length)) {
		_verified = true;
		return true;
	} else {
		_verified = false;
		return false;
	}
}

} /* namespace wanhive */
