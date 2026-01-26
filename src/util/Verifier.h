/**
 * @file Verifier.h
 *
 * System for mutual authentication
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_VERIFIER_H_
#define WH_UTIL_VERIFIER_H_
#include "../base/ds/BufferVector.h"
#include "../base/ds/State.h"
#include "../base/security/Srp.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * System for mutual authentication
 * @note Based on the SRP-6a authentication protocol
 */
class Verifier: public State, private Srp {
public:
	/**
	 * Constructor: creates a mutual authenticator.
	 * @param host true for host, false for user
	 */
	Verifier(bool host = false) noexcept;
	/**
	 * Destructor
	 */
	~Verifier();
	//-----------------------------------------------------------------
	/**
	 * STEP 1: Generates user's identity at the host's end.
	 * User -> Host:  I, A = g^a (identifies self, a = random number)
	 * Host:  B = kv + g^b
	 * Host:  u = H(A, B)
	 * Host:  load s and v
	 * Host:  S = (Av^u) ^ b (computes session key)
	 * Host:  K = H(S)
	 * Host: calculate M
	 *
	 * @param identity user's identity
	 * @param secret password verifier (hexadecimal)
	 * @param salt user's salt (hexadecimal)
	 * @param nonce user's public ephemeral value
	 * @return true on success, false on error
	 */
	bool identify(unsigned long long identity, const char *secret,
			const char *salt, const Data &nonce) noexcept;
	/**
	 * STEP 2: Calculates user's proof at the user's end.
	 * Host -> User:  s, B = kv + g^b (sends salt and nonce, b = random number)
	 * User:  u = H(A, B)
	 * User:  x = H(s, p)                 (user enters password)
	 * User:  S = (B - kg^x) ^ (a + ux)   (computes session key)
	 * User:  K = H(S)
	 * User: calculate M
	 *
	 * @param identity user's identity
	 * @param password user's secret password
	 * @param salt user's salt
	 * @param nonce host's public ephemeral key
	 * @param rounds password hashing rounds
	 * @return true on success, false on error
	 */
	bool scramble(unsigned long long identity, const Data &password,
			const Data &salt, const Data &nonce,
			unsigned int rounds = 1) noexcept;
	/**
	 * Host and user carry out mutual authentication.
	 * STEP 3: Host authenticates the user:
	 * User -> Host:  M
	 * Host: verifies M
	 * STEP 4: User authenticates the host:
	 * Host -> User: H(AMK)
	 * User: verifies H(AMK)
	 *
	 * @param proof shared state
	 * @return true on success, false on error
	 */
	bool verify(const Data &proof) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the most recent authentication's (STEPS 3 & 4) result.
	 * @return true on success, false on error
	 */
	bool verified() const noexcept;
	/**
	 * Returns user's identity saved during STEP 1.
	 * @return user's identity
	 */
	unsigned long long identity() const noexcept;
	/**
	 * Returns the public ephemeral value (nonce).
	 * User: generates user's nonce A (preparation for STEP 1)
	 * Host: returns host's nonce generated during STEP 1 (preparation for
	 * STEP 2).
	 *
	 * @param data stores the nonce
	 * @return true on success (always succeeds at the host), false on error
	 */
	bool nonce(Data &data) noexcept;
	/**
	 * Returns user's proof (M).
	 * Host: returns the M generated during STEP 1
	 * User: returns the M generated during STEP 2 (preparation for STEP 3)
	 *
	 * @param data stores user's proof
	 * @return true on success (always succeeds), false on error
	 */
	bool userProof(Data &data) noexcept;
	/**
	 * Returns host's proof (AMK).
	 * Host: returns the AMK generated during STEP 1 (preparation for STEP 4)
	 * User: returns the AMK generated during STEP 2
	 *
	 * @param data stores host's proof
	 * @return true on success (always succeeds), false on error
	 */
	bool hostProof(Data &data) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the salt.
	 * Host: returns the salt generated after STEP 1 (Preparation for STEP 2)
	 * User: returns the salt received in STEP 2
	 * General: returns the salt computed alongside the password verifier
	 *
	 * @param data stores the salt
	 */
	void salt(Data &data) noexcept;
	/**
	 * Returns the computed password verifier.
	 * @param data stores the password verifier
	 */
	void secret(Data &data) noexcept;
	/**
	 * Computes a random salt and a password verifier.
	 * s is chosen randomly
	 * x = H(s, p)
	 * v = g^x (computes password verifier)
	 *
	 * @param identity user's identity
	 * @param password user's password
	 * @param rounds password hash rounds
	 * @return true on success, false on error
	 */
	bool compute(const char *identity, const Data &password,
			unsigned int rounds) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Host: generates fake nonce during STEP 1 to hide a failed identification.
	 * @param data stores the fake nonce
	 */
	void fakeNonce(Data &data) noexcept;
	/**
	 * Host: generates fake salt during STEP 1 to hide a failed identification.
	 * @param identity user's identity
	 * @param data salt generator's seed as input and the generated salt as
	 * output (value-result argument).
	 */
	void fakeSalt(unsigned long long identity, Data &data) noexcept;
private:
	bool verifyUser(const Data &proof) noexcept;
	bool verifyHost(const Data &proof) noexcept;
private:
	const bool _host;
	unsigned long long _identity { };
	bool _verified { };
};

} /* namespace wanhive */

#endif /* WH_UTIL_VERIFIER_H_ */
