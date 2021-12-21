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
#include "Message.h"
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

	const PKI* get() const noexcept;
	void set(const PKI *pki) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Message signing and authentication functions which return true on
	 * success and false on error. If no PKI object key is provided (nullptr)
	 * then all the following methods are no-op and always return true.
	 */

	/*
	 * Signs the message of <length> bytes contained in <msg>. The message
	 * length is updated on success and the final message length is returned
	 * in <length> (value-result argument).
	 */
	bool sign(unsigned char *msg, unsigned int &length) noexcept;
	/*
	 * Validates and signs the message <msg>. Message's length is updated
	 * on success.
	 */
	bool sign(Message *msg) noexcept;

	/*
	 * Verifies a signed message of <length> bytes contained in <msg>.
	 * Returns true on successful verification, false otherwise.
	 */
	bool verify(const unsigned char *msg, unsigned int length) noexcept;
	/*
	 * Verifies the message consistency and the signature. Returns true on
	 * success (signature verified), false otherwise.
	 */
	bool verify(const Message *msg) noexcept;
private:
	const PKI *pki;
};

} /* namespace wanhive */

#endif /* WH_UTIL_TRUST_H_ */
