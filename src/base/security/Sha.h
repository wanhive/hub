/*
 * Sha.h
 *
 * SHA secure hash
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SECURITY_SHA_H_
#define WH_BASE_SECURITY_SHA_H_
#include <openssl/sha.h>

namespace wanhive {
//-----------------------------------------------------------------
//Supported message digest types
enum DigestType {
	WH_SHA1, WH_SHA256, WH_SHA512
};
//-----------------------------------------------------------------
/**
 * SHA message digest implementation for Wanhive.
 * Supported message digest types have been enumerated above.
 */
class Sha {
public:
	Sha(DigestType type) noexcept;
	virtual ~Sha();

	bool init() noexcept;
	bool update(const void *data, size_t dataLength) noexcept;
	bool final(unsigned char *messageDigest) noexcept;

	unsigned char* create(const unsigned char *data, size_t dataLength,
			unsigned char *messageDigest) noexcept;
	bool verify(const unsigned char *data, size_t dataLength,
			const unsigned char *messageDigest) noexcept;
	//Digest length in bytes
	unsigned int length() const noexcept;

	//Digest length for the given <type> in bytes
	static constexpr unsigned int length(DigestType type) noexcept {
		switch (type) {
		case WH_SHA1:
			return SHA_DIGEST_LENGTH;
		case WH_SHA256:
			return SHA256_DIGEST_LENGTH;
		default:
			return SHA512_DIGEST_LENGTH;
		}
	}
private:
	const DigestType type;
	const unsigned int _length; //Digest length in bytes
	union {
		SHA_CTX sha;
		SHA256_CTX sha256;
		SHA512_CTX sha512;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_BASE_SECURITY_SHA_H_ */
