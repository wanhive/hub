/*
 * InstanceID.h
 *
 * Cryptographically secure secret which can used for handshaking
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_INSTANCEID_H_
#define WH_UTIL_INSTANCEID_H_
#include "Hash.h"

namespace wanhive {
/**
 * A unique and secret identifier.
 */
class InstanceID {
public:
	InstanceID();
	~InstanceID();
	/*
	 * Generates a nonce suitable for handshaking and stores it inside <nonce>.
	 * (<salt>, <id>) pair must be unique and <nonce> should be a valid pointer.
	 */
	void generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	/*
	 * Verifies the <nonce>, returns true on successful verification.
	 */
	bool verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
			const Digest *nonce) const noexcept;
private:
	uint64_t buffer[4]; //256 bits of the instance id
};

} /* namespace wanhive */

#endif /* WH_UTIL_INSTANCEID_H_ */
