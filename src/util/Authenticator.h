/*
 * Authenticator.h
 *
 * SRP-6a based mutual authenticator
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_AUTHENTICATOR_H_
#define WH_UTIL_AUTHENTICATOR_H_
#include "../base/ds/State.h"
#include "../base/security/Srp.h"

namespace wanhive {
/**
 * SRP-6a based authenticator
 * @ref http://srp.stanford.edu/design.html
 */
class Authenticator: public State, private Srp {
public:
	/**
	 * Constructor: creates a new authenticator object
	 * @param isHost true for host, false for user
	 */
	Authenticator(bool isHost = false) noexcept;
	/**
	 * Destructor
	 */
	~Authenticator();
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
	 * @param nonce user's public ephemeral value
	 * @param nonceLength nonce length in bytes
	 * @param salt user's salt (hexadecimal)
	 * @param verifier password verifier (hexadecimal)
	 * @return true if the user's identity is generated at the host,
	 * false otherwise.
	 */
	bool identify(unsigned long long identity, const unsigned char *nonce,
			unsigned int nonceLength, const char *salt,
			const char *verifier) noexcept;
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
	 * @param passwordLength password length in bytes
	 * @param salt user's salt
	 * @param saltLength salt length in bytes
	 * @param nonce host's public ephemeral key
	 * @param nonceLength nonce length in bytes
	 * @param rounds password hashing rounds
	 * @return true if the user's proof (M) generated at the user's end,
	 * false otherwise.
	 */
	bool createIdentity(unsigned long long identity,
			const unsigned char *password, unsigned int passwordLength,
			const unsigned char *salt, unsigned int saltLength,
			const unsigned char *nonce, unsigned int nonceLength,
			unsigned int rounds = 1) noexcept;
	/**
	 * STEP 3: Host authenticates the user (user's identity is established).
	 * User -> Host:  M
	 * Host: verifies M
	 *
	 * @param proof user's proof
	 * @param length proof's length in bytes
	 * @return true if the user's identity is established at the host's end,
	 * false otherwise.
	 */
	bool authenticateUser(const unsigned char *proof,
			unsigned int length) noexcept;
	/**
	 * STEP 4: User authenticates the host (host's identity is established).
	 * Host -> User: H(AMK)
	 * User: verifies H(AMK)
	 *
	 * @param proof host's proof
	 * @param length proof's length in bytes
	 * @return true if host's identity is established at the user's end,
	 * false otherwise.
	 */
	bool authenticateHost(const unsigned char *proof,
			unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the most recent authentication call's status.
	 * @return returns true if the most recent authentication attempt
	 * (STEP 3 at host, STEP 4 at user) succeeded, false otherwise.
	 */
	bool isAuthenticated() const noexcept;
	/**
	 * Returns user's identity.
	 *
	 * @return if STEP 1 succeeded then the returned value contains the user's
	 * identity.
	 */
	unsigned long long getIdentity() const noexcept;
	/**
	 * Returns the public ephemeral value (nonce).
	 * User: generates the user nonce A (preparation for STEP 1)
	 * Host: returns the host nonce generated during STEP 1 (preparation for
	 * STEP 2, always succeeds).
	 *
	 * @param binary pointer to the nonce will be stored here
	 * @param bytes nonce length in byes
	 * @return true on success, false otherwise.
	 */
	bool generateNonce(const unsigned char *&binary,
			unsigned int &bytes) noexcept;
	/**
	 * Returns the user's proof (M).
	 * Host: returns the M generated during STEP 1 (always succeeds)
	 * User: returns the M generated during STEP 2 (preparation for STEP 3,
	 * always succeeds)
	 *
	 * @param binary pointer to the proof will be stored here
	 * @param bytes proof's length in bytes
	 * @return true on success, false otherwise
	 */
	bool generateUserProof(const unsigned char *&binary,
			unsigned int &bytes) noexcept;
	/**
	 * Returns the host's proof (AMK).
	 * Host: returns the AMK generated during STEP 1 (preparation for STEP 4,
	 * always succeeds)
	 * User: returns the AMK generated during STEP 2 (always succeeds)
	 *
	 * @param binary pointer to the proof will be stored here
	 * @param bytes proof's length in bytes
	 * @return true on success, false otherwise
	 */
	bool generateHostProof(const unsigned char *&binary,
			unsigned int &bytes) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the salt.
	 * Host: returns the salt generated after STEP 1 (Preparation for STEP 2)
	 * User: returns the salt received in STEP 2
	 * General: returns the random salt generated by a call to generateVerifier()
	 *
	 * @param binary pointer to the generated salt will be stored here
	 * @param bytes salt's length in bytes
	 */
	void getSalt(const unsigned char *&binary, unsigned int &bytes) noexcept;
	/**
	 * Returns the password verifier generated by a call to generateVerifier().
	 *
	 * @param binary pointer to the verifier will be stored here
	 * @param bytes verifier's length in bytes
	 */
	void getPasswordVerifier(const unsigned char *&binary,
			unsigned int &bytes) noexcept;
	/**
	 * Computes a random salt and password verifier (see getSalt() and
	 * getPasswordVerifier()).
	 * s is chosen randomly
	 * x = H(s, p)
	 * v = g^x (computes password verifier)
	 *
	 * @param identity user's identity
	 * @param password user's password
	 * @param passwordLength password length in bytes
	 * @param rounds password hash rounds
	 * @return true on success, false otherwise
	 */
	bool generateVerifier(const char *identity, const unsigned char *password,
			unsigned int passwordLength, unsigned int rounds) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Host: generates fake nonce (host's public ephemeral key) during STEP 1
	 * to hide a failed identification request.
	 * request.
	 *
	 * @param binary pointer to the fake nonce will be stored here
	 * @param bytes fake nonce's length in bytes
	 */
	void generateFakeNonce(const unsigned char *&binary,
			unsigned int &bytes) noexcept;
	/**
	 * Host: generates fake salt during STEP 1 to hide a failed identification
	 * request.
	 *
	 * @param identity user's identity
	 * @param salt a value-result argument for passing on a key to initiate the
	 * salt generation and storing the result (pointer to salt) after completion.
	 * @param length a value-result argument for passing on the key's length and
	 * storing the generated salt's length in bytes.
	 */
	void generateFakeSalt(unsigned long long identity,
			const unsigned char *&salt, unsigned int &length) noexcept;
private:
	unsigned long long id { 0 };
	bool authenticated { false };
	bool isHost;
};

} /* namespace wanhive */

#endif /* WH_UTIL_AUTHENTICATOR_H_ */
