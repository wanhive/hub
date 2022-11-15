/*
 * InstanceID.h
 *
 * Cryptographically secure secret for handshaking
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
#include "../base/common/NonCopyable.h"

namespace wanhive {
/**
 * A unique and secret identifier.
 */
class InstanceID: private NonCopyable {
public:
	/**
	 * Default constructor: creates a secure instance ID.
	 */
	InstanceID();
	/**
	 * Destructor
	 */
	~InstanceID();
	//-----------------------------------------------------------------
	/**
	 * Generates a nonce suitable for secure handshaking.
	 * @param hash the object that provides the hash function
	 * @param salt the salt
	 * @param id the identifier, (salt, id) pair should be unique.
	 * @param nonce the object for storing the generated nonce
	 */
	void generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	/**
	 * Verifies a nonce.
	 * @param hash the object that provides the hash function
	 * @param salt the salt
	 * @param id the identifier
	 * @param nonce the nonce for verification
	 * @return true on successful verification, false otherwise
	 */
	bool verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
			const Digest *nonce) const noexcept;
private:
	uint64_t buffer[4]; //256 bits of the instance id
};

} /* namespace wanhive */

#endif /* WH_UTIL_INSTANCEID_H_ */
