/**
 * @file PKI.h
 *
 * Asymmetric cryptography
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
#include "../base/security/Trust.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Asymmetric cryptography
 */
class PKI final: public Trust {
public:
	/**
	 * Default constructor: doesn't set up a key pair.
	 */
	PKI() noexcept;
	/**
	 * Constructor: loads key pair.
	 * @param privateKey private key (can be nullptr)
	 * @param publicKey public key (can be nullptr)
	 * @param secret private key's pass phrase
	 * @param text true to treat the key data as in-memory PEM-encoded texts,
	 * false to treat them as PEM-encoded file paths.
	 */
	PKI(const char *privateKey, const char *publicKey, char *secret = nullptr,
			bool text = false);
	/**
	 * Destructor
	 */
	~PKI();
	//-----------------------------------------------------------------
	/**
	 * Loads key pair (discards existing keys).
	 * @param privateKey private key (can be nullptr)
	 * @param publicKey public key (can be nullptr)
	 * @param secret private key's pass phrase
	 * @param text true to treat the key data as in-memory PEM-encoded texts,
	 * false to treat them as PEM-encoded file paths.
	 * @return true on success, false otherwise
	 */
	bool setup(const char *privateKey, const char *publicKey, char *secret =
			nullptr, bool text = false) noexcept;
	/**
	 * Loads private key (discards existing key).
	 * @param key private key (can be nullptr)
	 * @param secret private key's pass phrase
	 * @param text true to treat the key data as in-memory PEM-encoded text,
	 * false to treat it as a PEM-encoded file's path.
	 * @return true on success, false otherwise
	 */
	bool loadPrivateKey(const char *key, char *secret = nullptr, bool text =
			false) noexcept;
	/**
	 * Loads public key (discards existing key).
	 * @param key public key (can be nullptr)
	 * @param text true to treat the key data as in-memory PEM-encoded text,
	 * false to treat it as a PEM-encoded file's path.
	 * @return true on success, false otherwise
	 */
	bool loadPublicKey(const char *key, bool text = false) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Trust interface implementation
	 */
	bool hasPrivateKey() const noexcept override;
	bool hasPublicKey() const noexcept override;
	bool encrypt(const Data &plaintext, Cache &ciphertext) noexcept override;
	bool decrypt(const Data &ciphertext, Cache &plaintext) noexcept override;
	bool sign(const Data &message, Cache &signature) noexcept override;
	bool verify(const Data &message, const Data &signature) noexcept override;
	size_t payload() const noexcept override;
	size_t fingerprint(bool &fixed) const noexcept override;
	int algorithm() const noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Generates and stores key pair as PEM-encoded text files.
	 * @param privateKey private key file's path
	 * @param publicKey public key file's path
	 * @param secret private key's pass phrase
	 */
	static void generate(const char *privateKey, const char *publicKey,
			char *secret = nullptr);
public:
	/*! Key size in bits */
	static constexpr unsigned int KEY_LENGTH = 3072;
	/*! Size of encrypted data in bytes **/
	static constexpr unsigned int ENCODING_LENGTH = (KEY_LENGTH / 8);
	/*! Signature size in bytes */
	static constexpr unsigned int SIGNATURE_LENGTH = ENCODING_LENGTH;
	/*! Size of encrypted data in bytes **/
	static constexpr unsigned int ENCRYPTED_LENGTH = ENCODING_LENGTH;
	/*! Maximum data size (bytes) which can be encrypted */
	static constexpr unsigned int PAYLOAD_LENGTH = (ENCODING_LENGTH)
			- ((2 * 160 / 8) + 2);
private:
	Rsa rsa;
};

} /* namespace wanhive */

#endif /* WH_UTIL_PKI_H_ */
