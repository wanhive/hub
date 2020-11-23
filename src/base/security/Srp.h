/*
 * Srp.h
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
#include <openssl/bn.h>

namespace wanhive {
//-----------------------------------------------------------------
//Supported groups
enum SrpGroup {
	SRP_1024, SRP_1536, SRP_2048, SRP_3072, SRP_4096, SRP_6144, SRP_8192
};
//-----------------------------------------------------------------
/**
 * RFC 5054 compliant SRP-6a implementation
 * Thread safe at class level
 * REF: http://srp.stanford.edu/design.html
 * REF: https://tools.ietf.org/html/rfc5054
 */
class Srp {
public:
	Srp(SrpGroup type = SRP_1024, DigestType dType = WH_SHA1) noexcept;
	~Srp();

	bool initialize() noexcept;
	//=================================================================
	//Generate a random salt, if <bytes> is zero then it defaults to 16 bytes (128 bits)
	bool loadSalt(unsigned int bytes = 0) noexcept;
	//Load the hexadecimal salt
	bool loadSalt(const char *salt) noexcept;
	//Load the positive integer in big-endian format as salt
	bool loadSalt(const unsigned char *salt, unsigned int length) noexcept;
	//=================================================================
	/*
	 * I: NUL-terminated username, p: password, pLength: password length in bytes
	 * If <I> is nullptr then the output degenerates to H(s, H(p))
	 * <rounds> is the number of hashing rounds, defaults to 1
	 */
	bool loadPrivateKey(const char *I, const unsigned char *p,
			unsigned int pLength, unsigned int rounds = 1) noexcept;

	//Generate the password verifier from the private key
	bool loadPasswordVerifier() noexcept;
	//Load the hexadecimal verifier
	bool loadPasswordVerifier(const char *verifier) noexcept;
	//Load the positive integer in big-endian format as verifier
	bool loadPasswordVerifier(const unsigned char *verifier,
			unsigned int length) noexcept;
	//=================================================================
	//Generate a random user secret, if <bytes> is zero then it defaults to 32 bytes (256 bits)
	bool loadUserSecret(unsigned int bytes = 0) noexcept;
	//Load the hexadecimal user secret
	bool loadUserSecret(const char *nonce) noexcept;
	//Load the positive integer in big-endian format as user secret
	bool loadUserSecret(const unsigned char *nonce,
			unsigned int length) noexcept;

	//Generate a random host secret, if <bytes> is zero then it defaults to 32 bytes (256 bits)
	bool loadHostSecret(unsigned int bytes = 0) noexcept;
	//Load the hexadecimal host secret
	bool loadHostSecret(const char *nonce) noexcept;
	//Load the positive integer in big-endian format as host secret
	bool loadHostSecret(const unsigned char *nonce,
			unsigned int length) noexcept;

	//Generate the user nonce from user secret
	bool loadUserNonce() noexcept;
	//Load the hexadecimal nonce
	bool loadUserNonce(const char *nonce) noexcept;
	//Load the positive integer in big-endian format as user nonce
	bool loadUserNonce(const unsigned char *nonce, unsigned int length) noexcept;

	//Generate the host nonce from host secret and verifier
	bool loadHostNonce() noexcept;
	//Load the hexadecimal nonce
	bool loadHostNonce(const char *nonce) noexcept;
	//Load the positive integer in big-endian format as host nonce
	bool loadHostNonce(const unsigned char *nonce, unsigned int length) noexcept;

	//Generate the scrambling parameter from user and host nonces
	bool loadRandomScramblingParameter() noexcept;
	//Load S and K
	bool loadSessionKey(bool isHost = true) noexcept;
	//=================================================================
	/**
	 * For full mutual authentication
	 */
	bool generateUserProof(const char *I) noexcept;
	bool generateHostProof() noexcept;
	//-----------------------------------------------------------------
	/**
	 * For password-only proof
	 * REF: T. Wu, SRP-6: Improvements and Refinements to the Secure Remote Password Protocol,
	 * Submission to the IEEE P1363 Working Group, Oct 2002.
	 */
	bool generateUserEvidence() noexcept;
	bool generateHostEvidence() noexcept;
	//-----------------------------------------------------------------
	/**
	 * For verification of the proofs
	 */
	bool verifyUserProof(const unsigned char *proof,
			unsigned int length) noexcept;
	bool verifyHostProof(const unsigned char *proof,
			unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * For generating fake nonce and salt during handshaking
	 * Allows the host to fake successful identification
	 * Not suitable for cryptographic purposes
	 */
	bool generateFakeNonce() noexcept;
	bool generateFakeSalt(const char *I, const unsigned char *s,
			unsigned int sLen) noexcept;
	//=================================================================
	/**
	 * For retrieving the outputs
	 */
	void getSalt(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	void getPasswordVerifier(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	void getUserNonce(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	void getHostNonce(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	void getSessionKey(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	const unsigned char* getKey() const noexcept;

	void getUserProof(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	void getHostProof(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;

	void getFakeNonce(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;
	void getFakeSalt(const unsigned char *&binary,
			unsigned int &bytes) const noexcept;

	//Size of Key(K) in bytes
	unsigned int keySize() const noexcept;
	//Size of Group (N) in bytes
	unsigned int groupSize() const noexcept;

	int getStatus() const noexcept;
	void setStatus(int status) noexcept;
	//=================================================================
	/**
	 * For testing and debugging
	 */
	//For debugging purposes
	void print(const char *msg = nullptr) const noexcept;
	/* RFC 5054 vector test */
	static void test() noexcept;
	/* Stress test */
	static void test(const char *I, const unsigned char *p, unsigned int pwdLen,
			unsigned int iterations, SrpGroup type = SRP_1024,
			DigestType dType = WH_SHA1) noexcept;
private:
	/**
	 * Following three functions are called by <initialize>
	 */
	bool loadPrime() noexcept;
	bool loadGenerator() noexcept;
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
		//Converts length bytes in binary to BIGNUM
		bool put(const unsigned char *binary, unsigned int length) noexcept;
		bool put(const char *hex) noexcept;
		bool random(int bits, int top = BN_RAND_TOP_ANY, int bottom =
		BN_RAND_BOTTOM_ANY) noexcept;
		bool pseudoRandom(const BIGNUM *range) noexcept;
		BIGNUM* get() const noexcept;
		const unsigned char* getBinary() const noexcept;
		unsigned int size() const noexcept;
		void print() const noexcept;
	private:
		void clear() noexcept;
	private:
		BIGNUM *n;
		unsigned int bytes;
		//Sufficient for storing 8192 bits (largest N)
		unsigned char binary[1024];
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

	//For left padding to N
	unsigned char zeros[1024];
	//Populated from Appendix A of RFC 5054
	static const struct constants {
		const char *Nhex;
		const char *ghex;
	} Nghex[7];
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SRP_H_ */
