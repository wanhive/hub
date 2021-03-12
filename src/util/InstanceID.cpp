/*
 * InstanceID.cpp
 *
 * Cryptographically secure secret which can used for handshaking
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "InstanceID.h"
#include "Random.h"
#include "../base/common/defines.h"
#include <cstring>

namespace wanhive {

InstanceID::InstanceID() {
	memset(buffer, 0, sizeof(buffer));
	Random().bytes(buffer, sizeof(buffer));
}

InstanceID::~InstanceID() {
	memset(buffer, 0, sizeof(buffer));
}

void InstanceID::generateNonce(Hash &hash, uint64_t salt, uint64_t id,
		Digest *nonce) const noexcept {
	if (nonce) {
		uint64_t block[WH_ARRAYLEN(buffer) + 2];
		block[0] = salt;
		block[1] = id;
		memcpy(&block[2], buffer, sizeof(buffer));
		hash.create(block, sizeof(block), nonce);
		memset(block, 0, sizeof(block));
	}
}

bool InstanceID::verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
		const Digest *nonce) const noexcept {
	if (nonce) {
		uint64_t block[WH_ARRAYLEN(buffer) + 2];
		block[0] = salt;
		block[1] = id;
		memcpy(&block[2], buffer, sizeof(buffer));
		auto ret = hash.verify(nonce, block, sizeof(block));
		memset(block, 0, sizeof(block));
		return ret;
	} else {
		return false;
	}
}

} /* namespace wanhive */
