/**
 * @file Srp.h
 *
 * SRP-6a implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_SRP_H_
#define WH_BASE_SECURITY_SRP_H_
#include "Sha.h"
#include "../common/NonCopyable.h"
#include <openssl/bn.h>

/*! @namespace wanhive */
namespace wanhive {
//-----------------------------------------------------------------
/**
 * Supported group sizes
 */
enum SrpGroup {
	SRP_1024,/**< 1024 bits */
	SRP_1536,/**< 1536 bits */
	SRP_2048,/**< 2048 bits */
	SRP_3072,/**< 3072 bits */
	SRP_4096,/**< 4096 bits */
	SRP_6144,/**< 6144 bits */
	SRP_8192 /**< 8192 bits */
};
//-----------------------------------------------------------------
/**
 * RFC 5054 compliant SRP-6a implementation
 * @ref http://srp.stanford.edu/design.html
 * @ref https://tools.ietf.org/html/rfc5054
 */
class Srp: private NonCopyable {
public:
	/**
	 * Constructor: assigns the SRP-6a context.
	 * @param type group type
	 * @param hf hash function type
	 */
	Srp(SrpGroup type = SRP_1024, HashType hf = WH_SHA1) noexcept;
	/**
	 * Destructor
	 */
	~Srp();
	/**
	 * Initializes the SRP-6a context.
	 * @return true on success, false on error
	 */
	bool initialize() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates a random salt.
	 * @param bytes salt's size in bytes (0 for default size)
	 * @return true on success, false on error
	 */
	bool loadSalt(unsigned int bytes = 0) noexcept;
	/**
	 * Loads the given hexadecimal number as salt.
	 * @param salt salt's value
	 * @return true on success, false on error
	 */
	bool loadSalt(const char *salt) noexcept;
	/**
	 * Loads the given big-endian formatted positive integer as salt.
	 * @param salt salt's value
	 * @param length salt's size in bytes
	 * @return true on success, false on error
	 */
	bool loadSalt(const unsigned char *salt, unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates the private key.
	 * @param I NUL-terminated user name
	 * @param p user's clear text password
	 * @param pLength password length in bytes
	 * @param rounds number of password hash rounds
	 * @return true on success, false otherwise
	 */
	bool loadPrivateKey(const char *I, const unsigned char *p,
			unsigned int pLength, unsigned int rounds = 1) noexcept;
	/**
	 * Generates password verifier from the private key.
	 * @return true on success, false otherwise
	 */
	bool loadPasswordVerifier() noexcept;
	/**
	 * Loads the given hexadecimal number as password verifier.
	 * @param verifier verifier's value
	 * @return true on success, false otherwise
	 */
	bool loadPasswordVerifier(const char *verifier) noexcept;
	/**
	 * Loads the given big-endian formatted positive integer as verifier
	 * @param verifier verifier's value
	 * @param length verifier's size in bytes
	 * @return true on success, false otherwise
	 */
	bool loadPasswordVerifier(const unsigned char *verifier,
			unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates user's secret ephemeral value.
	 * @param bytes secret value's size in bytes (0 for default size)
	 * @return true on success, false otherwise
	 */
	bool loadUserSecret(unsigned int bytes = 0) noexcept;
	/**
	 * Loads the given hexadecimal number as user's secret ephemeral value.
	 * @param nonce secret's value
	 * @return true on success, false otherwise
	 */
	bool loadUserSecret(const char *nonce) noexcept;
	/**
	 * Loads the given big-endian formatted positive integer as user's secret
	 * ephemeral value.
	 * @param nonce secret's value
	 * @param length secret's size in bytes
	 * @return true on success, false otherwise
	 */
	bool loadUserSecret(const unsigned char *nonce,
			unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates host's secret ephemeral value.
	 * @param bytes secret value's size in bytes (0 for default size)
	 * @return true on success, false otherwise
	 */
	bool loadHostSecret(unsigned int bytes = 0) noexcept;
	/**
	 * Loads the given hexadecimal number as host's secret ephemeral value.
	 * @param nonce secret's value
	 * @return true on success, false otherwise
	 */
	bool loadHostSecret(const char *nonce) noexcept;
	/**
	 * Loads the given big-endian formatted positive integer as host's secret
	 * ephemeral value.
	 * @param nonce secret's value
	 * @param length secret's size in bytes
	 * @return true on success, false otherwise
	 */
	bool loadHostSecret(const unsigned char *nonce,
			unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates user's public ephemeral value from user's secret.
	 * @return true on success, false otherwise
	 */
	bool loadUserNonce() noexcept;
	/**
	 * Loads the given hexadecimal number as user's public ephemeral value.
	 * @param nonce public ephemeral value
	 * @return true on success, false otherwise
	 */
	bool loadUserNonce(const char *nonce) noexcept;
	/**
	 * Loads the given big-endian formatted positive integer as user's public
	 * ephemeral value.
	 * @param nonce public ephemeral value
	 * @param length public ephemeral value's size in bytes
	 * @return true on success, false otherwise
	 */
	bool loadUserNonce(const unsigned char *nonce, unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates host's public ephemeral value from host's secret and verifier.
	 * @return true on success, false otherwise
	 */
	bool loadHostNonce() noexcept;
	/**
	 * Loads the given hexadecimal number as host's public ephemeral value.
	 * @param nonce public ephemeral value
	 * @return true on success, false otherwise
	 */
	bool loadHostNonce(const char *nonce) noexcept;
	/**
	 * Loads the given big-endian formatted positive integer as host's public
	 * ephemeral value.
	 * @param nonce public ephemeral value
	 * @param length public ephemeral value's size in bytes
	 * @return true on success, false otherwise
	 */
	bool loadHostNonce(const unsigned char *nonce, unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates the scrambling parameter from user and host nonces (public
	 * ephemeral values).
	 * @return true on success, false otherwise
	 */
	bool loadRandomScramblingParameter() noexcept;
	/**
	 * Computes the session key.
	 * @param isHost true for host, false for user
	 * @return true on success, false otherwise
	 */
	bool loadSessionKey(bool isHost = true) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Mutual authentication: generates user's proof.
	 * @param I Nul-terminated user name
	 * @return true on success, false otherwise
	 */
	bool generateUserProof(const char *I) noexcept;
	/**
	 * Mutual authentication: generates host's proof.
	 * @return true on success, false otherwise
	 */
	bool generateHostProof() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Password-only proof: generates user's proof.
	 * @note T. Wu, SRP-6: Improvements and Refinements to the Secure Remote
	 * Password Protocol, Submission to the IEEE P1363 Working Group, Oct 2002.
	 * @return true on success, false otherwise
	 */
	bool generateUserEvidence() noexcept;
	/**
	 * Password-only proof: generates host's proof.
	 * @return true on success, false otherwise
	 */
	bool generateHostEvidence() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Verifies user's proof.
	 * @param proof user's proof
	 * @param length proof's size in bytes
	 * @return true on successful verification, false on error
	 */
	bool verifyUserProof(const unsigned char *proof,
			unsigned int length) noexcept;
	/**
	 * Verifies host's proof.
	 * @param proof host's proof
	 * @param length proof's size in bytes
	 * @return true on successful verification, false on error
	 */
	bool verifyHostProof(const unsigned char *proof,
			unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates fake nonce (to fake successful identification).
	 * @return true on success, false otherwise
	 */
	bool generateFakeNonce() noexcept;
	/**
	 * Generates fake salt (to fake successful identification).
	 * @param I NUL-terminated user name
	 * @param s seed data
	 * @param sLen seed data's size in bytes
	 * @return true on success, false otherwise
	 */
	bool generateFakeSalt(const char *I, const unsigned char *s,
			unsigned int sLen) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the computed salt.
	 * @param binary stores salt's value
	 * @param bytes stores salt's size in bytes
	 */
	void getSalt(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the computed verifier.
	 * @param binary stores verifier's value
	 * @param bytes stores verifier's size in bytes
	 */
	void getPasswordVerifier(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the computed user's nonce (public ephemeral value).
	 * @param binary stores nonce's value
	 * @param bytes stores nonce's size in bytes
	 */
	void getUserNonce(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the computed host's nonce (public ephemeral value).
	 * @param binary stores nonce's value
	 * @param bytes stores nonce's size in bytes
	 */
	void getHostNonce(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the computed session key.
	 * @param binary stores session key's value
	 * @param bytes stores session key's size in bytes
	 */
	void getSessionKey(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the digest value of session key.
	 * @return digest's value
	 */
	const unsigned char* getKey() const noexcept;
	/**
	 * Returns the user's proof.
	 * @param binary stores user's proof
	 * @param bytes stores user's proof size in byte
	 */
	void getUserProof(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the host's proof.
	 * @param binary stores host's proof
	 * @param bytes stores host's proof size in byte
	 */
	void getHostProof(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the fake nonce.
	 * @param binary stores nonce's value
	 * @param bytes stores nonce's size in bytes
	 */
	void getFakeNonce(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the fake salt.
	 * @param binary stores salt's value
	 * @param bytes stores salt's size in bytes
	 */
	void getFakeSalt(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	/**
	 * Returns the key(K) size.
	 * @return key size in bytes
	 */
	unsigned int keySize() const noexcept;
	/**
	 * Returns the group size.
	 * @return group size in bytes
	 */
	unsigned int groupSize() const noexcept;
	/**
	 * Returns the status code.
	 * @return status code
	 */
	int getStatus() const noexcept;
	/**
	 * Sets a new status code.
	 * @param status new status code
	 */
	void setStatus(int status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * For testing: prints the internal state.
	 * @param msg optional message string
	 */
	void print(const char *msg = nullptr) const noexcept;
	/**
	 * For testing: RFC 5054 vector test.
	 */
	static void test() noexcept;
	/**
	 * For testing: stress test.
	 * @param I NUL-terminated user name
	 * @param p user's clear text password
	 * @param pwdLen password length in bytes
	 * @param iterations iterations count
	 * @param type group type
	 * @param hf hash function type
	 */
	static void test(const char *I, const unsigned char *p, unsigned int pwdLen,
			unsigned int iterations, SrpGroup type = SRP_1024, HashType hf =
					WH_SHA1) noexcept;
private:
	//For initialization
	bool loadPrime() noexcept;
	//For initialization
	bool loadGenerator() noexcept;
	//For initialization
	bool loadMultiplierParameter() noexcept;
	//Checks if n % N != 0
	bool checkNotZero(BIGNUM *n) noexcept;
	//Checks whether n is in [0, N-1]
	bool checkRange(BIGNUM *n) noexcept;
	//Allocate temporary variables
	bool newContext() noexcept;
	//Free temporary variables
	void freeContext() noexcept;
private:
	class BigNumber {
	public:
		BigNumber() noexcept;
		~BigNumber();
		bool put(BIGNUM *n) noexcept;
		bool put(const unsigned char *binary, unsigned int length) noexcept;
		bool put(const char *hex) noexcept;
		bool random(int bits, int top = BN_RAND_TOP_ANY, int bottom =
		BN_RAND_BOTTOM_ANY) noexcept;
		bool pseudoRandom(const BIGNUM *range) noexcept;
		BIGNUM* get() const noexcept;
		const unsigned char* getBinary() const noexcept;
		unsigned int size() const noexcept;
		void print() const noexcept;
		void clear() noexcept;
	private:
		BIGNUM *n { nullptr };
		unsigned int bytes { 0 };
		//Sufficient for 8192 bits (largest N)
		unsigned char binary[1024] { };
	};
private:
	//Max buffer size for storing the message digest (64 bytes)
	static constexpr unsigned int MDSIZE = Sha::length(WH_SHA512);
	static constexpr unsigned int SALTLENGTH = 16;	//128 bits
	static constexpr unsigned int SECRETLENGTH = 32;	//256 bits
	SrpGroup type;
	BN_CTX *ctx;
	Sha H;
	int status;

	struct {
		//These values should remain fixed for the lifetime of the object
		BigNumber N; //A large safe prime, all arithmetic is done modulo N.
		BigNumber g; //A generator modulo N
		BigNumber k; //Multiplier parameter k = H(N, PAD(g))
	} group;

	struct {
		//Host stores (I, s, v) into it's database
		BigNumber s; //User's salt
		BigNumber x; //Private key x = H(s | H ( I | ":" | p) )
		BigNumber v; //Password verifier v = g^x
	} user;

	struct {
		//Secret ephemeral values
		BigNumber a; //User
		BigNumber b; //Host
	} secret;

	struct {
		//Public ephemeral values
		BigNumber A; //User A = g^a
		BigNumber B; //Host B = kv + g^b
		//Random scrambling parameter
		BigNumber u; //u = H(PAD(A), PAD(B))
	} shared;

	struct {
		/*
		 * Premaster secret
		 * S = (B - kg^x) ^ (a + ux)   at User's end
		 * S = (Av^u) ^ b at host's end
		 */
		BigNumber S;
		//The session key,  K = H(PAD(S))
		unsigned char K[MDSIZE];
	} key;

	struct {
		/*
		 * User -> Host: M = H(H(N) xor H(g), H(I), s, A, B, K)
		 * Alternatively M = H(A, B, S)
		 */
		unsigned char M[MDSIZE];
		/*
		 * Host -> User: H(A, M, K)
		 * Alternatively H(A, M, S)
		 */
		unsigned char AMK[MDSIZE];
	} proof;

	struct {
		/*
		 * Fake nonce and salt
		 */
		BigNumber nonce; //Value in range [0, N)
		unsigned char salt[MDSIZE]; //Fake salt
	} fake;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SRP_H_ */
