/*
 * PKI.h
 *
 * RSA-2048 based asymmetric cryptography for Wanhive
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_PKI_H_
#define WH_UTIL_PKI_H_
#include "../base/security/Rsa.h"

namespace wanhive {
//-----------------------------------------------------------------
//Length of RSA key in bits
#undef WH_PKI_KEY_LENGTH
#define WH_PKI_KEY_LENGTH 2048
//Length of RSA encrypted data in bytes (256 bytes)
#undef WH_PKI_ENCODING_LEN
#define WH_PKI_ENCODING_LEN ((WH_PKI_KEY_LENGTH) / 8)
//RSA signature
using Signature=unsigned char[WH_PKI_ENCODING_LEN];
//RSA encrypted data
using PKIEncryptedData=unsigned char[WH_PKI_ENCODING_LEN];
//-----------------------------------------------------------------
/**
 * The public key infrastructure based on RSA
 * Objects of this class can be re-initialized
 */
class PKI {
public:
	//Doesn't initialize the object
	PKI() noexcept;
	~PKI();
	//=================================================================
	/**
	 * Keys can be nullptr, associated facility will remain unavailable.
	 * If <fromFile> is true then keys will loaded from files otherwise
	 * <hostKey> and <publicKey> will be treated as Base16 encoded keys.
	 */
	//Reinitialize the object
	bool initialize(const char *hostKey, const char *publicKey, bool fromFile =
			true) noexcept;

	bool loadPublicKey(const char *publicKey, bool fromFile = true) noexcept;
	bool loadHostKey(const char *hostKey, bool fromFile = true) noexcept;

	bool hasPublicKey() const noexcept;
	bool hasHostKey() const noexcept;
	//=================================================================
	//Cannot encrypt blocks bigger than MAX_PT_LEN
	bool encrypt(const void *block, unsigned int size,
			PKIEncryptedData *target) const noexcept;
	bool decrypt(const PKIEncryptedData *block, void *result) const noexcept;

	bool sign(const void *block, unsigned int size,
			Signature *sig) const noexcept;
	bool verify(const void *block, unsigned int len,
			const Signature *sig) const noexcept;
	//=================================================================
	/*
	 * Generate a pair of 2048 bit RSA keys and store them into
	 * <hostKey> and <publicKey> files.
	 */
	static void generateKeyPair(const char *hostKey, const char *publicKey);
public:
	static constexpr unsigned int KEY_LENGTH = WH_PKI_KEY_LENGTH; //2048 bits
	static constexpr unsigned int ENCODING_LENGTH = WH_PKI_ENCODING_LEN; //256 bytes
	static constexpr unsigned int SIGNATURE_LENGTH = WH_PKI_ENCODING_LEN; //256 bytes
	static constexpr unsigned int ENCRYPTED_LENGTH = WH_PKI_ENCODING_LEN; //256 bytes
	//Maximum size in bytes of the plain text block which can be encrypted
	static constexpr unsigned int MAX_PT_LEN = (ENCODING_LENGTH)
			- ((2 * 160 / 8) + 2);
private:
	Rsa rsa;
};

#undef WH_PKI_KEY_LENGTH
#undef WH_PKI_ENCODING_LEN

} /* namespace wanhive */

#endif /* WH_UTIL_PKI_H_ */
