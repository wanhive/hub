/*
 * @file MLDsa.h
 *
 * ML-DSA cipher
 *
 *
 * Copyright (C) 2026 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_MLDSA_H_
#define WH_BASE_SECURITY_MLDSA_H_
#include "KeyPair.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Standardized parameters
 */
enum MLDsaParam {
	ML_DSA_44, /**< Level 2 */
	ML_DSA_65, /**< Level 3 */
	ML_KEM_87 /**< Level 5 */
};

/**
 * ML-DSA cipher
 */
class MLDsa: public KeyPair {
public:
	/**
	 * Constructor: creates an empty object (no key pair).
	 */
	MLDsa(MLDsaParam param = ML_DSA_44) noexcept;
	/**
	 * Destructor
	 */
	~MLDsa();
	//-----------------------------------------------------------------
	/**
	 * Signs the given data.
	 * @param data data for signing
	 * @param dataLength data's size in bytes
	 * @param signature output buffer for digital signature. Set to nullptr to
	 * calculate the maximum buffer size.
	 * @param signatureLength buffer size in bytes as input and signature's
	 * size in bytes as output (value-result argument).
	 * @return true on success, false on error
	 */
	bool sign(const unsigned char *data, size_t dataLength,
			unsigned char *signature, size_t &signatureLength) noexcept;
	/**
	 * Verifies the given data.
	 * @param verifiable data
	 * @param dataLength data's size in bytes
	 * @param signature digital signature
	 * @param signatureLength signature's size in bytes
	 * @return true on successful verification, false otherwise
	 */
	bool verify(const unsigned char *data, size_t dataLength,
			const unsigned char *signature, size_t signatureLength) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates PEM encoded key pair.
	 * @param privateKey private key file's path
	 * @param publicKey public key file's path
	 * @param secret private key's pass phrase
	 * @return true on success, false on error
	 */
	bool generate(const char *privateKey, const char *publicKey, char *secret =
			nullptr) noexcept;
private:
	EVP_MD_CTX* mdContext() noexcept;
private:
	EVP_MD_CTX *mdctx { };
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_MLDSA_H_ */
