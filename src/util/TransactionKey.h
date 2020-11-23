/*
 * TransactionKey.h
 *
 * Asymmetric key and nonce used during mutual authentication
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_TRANSACTIONKEY_H_
#define WH_UTIL_TRANSACTIONKEY_H_
#include "Hash.h"
#include "PKI.h"

namespace wanhive {
/**
 * Key and nonce
 */
struct TransactionKey {
	const PKI *keys;
	Digest *nonce;
};

}  // namespace wanhive

#endif /* WH_UTIL_TRANSACTIONKEY_H_ */
