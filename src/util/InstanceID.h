/*
 * InstanceID.h
 *
 * Secure instance identifier
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
 * Secure and unique identifier
 */
class InstanceID: private NonCopyable {
public:
	/**
	 * Constructor: creates a secure identifier.
	 */
	InstanceID();
	/**
	 * Destructor
	 */
	~InstanceID();
	//-----------------------------------------------------------------
	/**
	 * Generates cryptographic nonce using a unique (salt, id) pair.
	 * @param hash cryptographic hash function provider
	 * @param salt random data
	 * @param id identifier
	 * @param nonce stores the generated nonce
	 */
	void generateNonce(Hash &hash, uint64_t salt, uint64_t id,
			Digest *nonce) const noexcept;
	/**
	 * Verifies a cryptographic nonce.
	 * @param hash cryptographic hash function provider
	 * @param salt random data
	 * @param id identifier
	 * @param nonce pseudo-random nonce
	 * @return true on success, false on error
	 */
	bool verifyNonce(Hash &hash, uint64_t salt, uint64_t id,
			const Digest *nonce) const noexcept;
private:
	uint64_t buffer[4]; //256 bits of the instance id
};

} /* namespace wanhive */

#endif /* WH_UTIL_INSTANCEID_H_ */
