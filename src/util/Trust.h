/*
 * Trust.h
 *
 * Message signing and verification
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_TRUST_H_
#define WH_UTIL_TRUST_H_
#include "PKI.h"

namespace wanhive {
/**
 * Message signing and verification
 */
class Trust {
public:
	Trust() noexcept;
	Trust(const PKI *pki) noexcept;
	~Trust();

	//Returns the key pair (can be nullptr)
	const PKI* get() const noexcept;
	//Assigns a key pair (can be nullptr, see below)
	void set(const PKI *pki) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Message signing and verification functions which return true on
	 * success and false on error. If no key pair is provided (nullptr)
	 * then the following methods do absolutely nothing and return true.
	 */

	/*
	 * Signs the message of <length> bytes contained in <msg>. Message length
	 * is updated on success and the final message length is returned via
	 * <length> which is a value-result argument.
	 */
	bool sign(unsigned char *msg, unsigned int &length) const noexcept;

	/*
	 * Verifies a signed message of <length> bytes contained in <msg>.
	 * Returns true on successful verification, false otherwise.
	 */
	bool verify(const unsigned char *msg, unsigned int length) const noexcept;
private:
	const PKI *pki;
};

} /* namespace wanhive */

#endif /* WH_UTIL_TRUST_H_ */
