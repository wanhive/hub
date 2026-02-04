/**
 * @file Trust.h
 *
 * Public key cryptography
 *
 *
 * Copyright (C) 2026 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_TRUST_H_
#define WH_BASE_SECURITY_TRUST_H_
#include "../ds/BufferVector.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Public key cryptography
 */
class Trust {
public:
	/**
	 * Virtual destructor.
	 */
	virtual ~Trust() = default;
	/**
	 * Checks the availability of private key.
	 * @return true if a private key exists, false otherwise
	 */
	virtual bool hasPrivateKey() const noexcept = 0;
	/**
	 * Checks the availability of a public key.
	 * @return true if a public key exists, false otherwise
	 */
	virtual bool hasPublicKey() const noexcept = 0;
	/**
	 * Performs public key encryption of plaintext.
	 * @param plaintext original data
	 * @param ciphertext stores the encrypted data
	 * @return true on success, false on error
	 */
	virtual bool encrypt(Data &plaintext, Cache &ciphertext) noexcept = 0;
	/**
	 * Performs private key decryption of ciphertext.
	 * @param ciphertext encrypted data
	 * @param plaintext stores the original data
	 * @return true on success, false on error
	 */
	virtual bool decrypt(Data &ciphertext, Cache &plaintext) noexcept = 0;
	/**
	 * Signs data using the private key.
	 * @param message original data
	 * @param signature stores the digital signature
	 * @return true on success, false on error
	 */
	virtual bool sign(Data &message, Cache &signature) noexcept = 0;
	/**
	 * Verifies a digital signature using the public key.
	 * @param message original data
	 * @param signature digital signature
	 * @return true on successful verification, false on error
	 */
	virtual bool verify(Data &message, Data &signature) noexcept = 0;
	/**
	 * Returns the maximum plaintext length in bytes that can be encrypted.
	 * @return maximum plaintext size in bytes, 0 if not applicable
	 */
	virtual size_t payload() const noexcept = 0;
	/**
	 * Returns the maximum digital signature length in bytes.
	 * @param fixed stores true if the digital signature has a fixed size,
	 * stores false if the digital signature's size varies.
	 * @return maximum digital signature size in bytes, 0 if not applicable
	 */
	virtual size_t fingerprint(bool &fixed) const noexcept = 0;
	/**
	 * Returns base algorithm's numeric identifier.
	 * @return algorithm's identifier
	 */
	virtual int algorithm() const noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_TRUST_H_ */
