/*
 * Rsa.cpp
 *
 * RSA cipher
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Rsa.h"
#include "../Storage.h"
#include <cstring>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

namespace wanhive {

Rsa::Rsa() noexcept :
		_public(nullptr), _private(nullptr) {

}

Rsa::~Rsa() {
	reset();
}

bool Rsa::init(const char *privateKey, const char *publicKey, bool fromFile,
		char *password) noexcept {
	loadPublicKey(publicKey, fromFile);
	loadPrivateKey(privateKey, fromFile, password);

	return (!privateKey || _private) && (!publicKey || _public);
}

void Rsa::reset() noexcept {
	freePublicKey();
	freePrivateKey();
}

bool Rsa::loadPrivateKey(const char *privateKey, bool fromFile,
		char *password) noexcept {
	freePrivateKey();
	if (fromFile) {
		_private = createFromFile(privateKey, false, password);
	} else {
		_private = create(privateKey, false, password);
	}
	return _private != nullptr;
}

bool Rsa::loadPublicKey(const char *publicKey, bool fromFile) noexcept {
	freePublicKey();
	if (fromFile) {
		_public = createFromFile(publicKey, true, nullptr);
	} else {
		_public = create(publicKey, true, nullptr);
	}
	return _public != nullptr;
}

void Rsa::freePrivateKey() noexcept {
	destroyKey(_private);
	_private = nullptr;
}

void Rsa::freePublicKey() noexcept {
	destroyKey(_public);
	_public = nullptr;
}

bool Rsa::hasPrivateKey() const noexcept {
	return _private != nullptr;
}

bool Rsa::hasPublicKey() const noexcept {
	return _public != nullptr;
}

int Rsa::encrypt(const unsigned char *data, int dataLength,
		unsigned char *encrypted) const noexcept {
	if (_public) {
		return publicEncrypt(_public, data, dataLength, encrypted,
		RSA_PKCS1_OAEP_PADDING);
	} else {
		return -1;
	}

}

int Rsa::decrypt(const unsigned char *encryptedData, int dataLength,
		unsigned char *decrypted) const noexcept {
	if (_private) {
		return privateDecrypt(_private, encryptedData, dataLength, decrypted,
		RSA_PKCS1_OAEP_PADDING);
	} else {
		return -1;
	}
}

bool Rsa::sign(const unsigned char *data, unsigned int dataLength,
		unsigned char *signature, unsigned int *signatureLength) const noexcept {
	if (_private) {
		unsigned char md[SHA_DIGEST_LENGTH];
		unsigned int len;
		return SHA1(data, dataLength, (unsigned char*) md)
				&& signDigest(_private, md, SHA_DIGEST_LENGTH, signature,
						signatureLength ? signatureLength : &len, NID_sha1);
	} else {
		return false;
	}

}

bool Rsa::verify(const unsigned char *data, unsigned int dataLength,
		unsigned char *signature, unsigned int signatureLength) const noexcept {
	if (_public) {
		unsigned char md[SHA_DIGEST_LENGTH];
		memset(&md, 0, sizeof(md));
		return SHA1(data, dataLength, (unsigned char*) md)
				&& verifyDigest(_public, md, SHA_DIGEST_LENGTH, signature,
						signatureLength, NID_sha1);
	} else {
		return false;
	}
}

bool Rsa::generateKeyPair(const char *privateKeyFile, const char *publicKeyFile,
		int bits, char *password) noexcept {
	if (!privateKeyFile || !publicKeyFile) {
		return false;
	}

	auto pRSA = generateRSAKey(bits);
	if (!pRSA) {
		return false;
	}

	auto pPrivKey = generatePrivateKey(pRSA);
	auto status = false;
	if (pPrivKey) {
		status = generatePem(privateKeyFile, pPrivKey, false, password, nullptr)
				&& generatePem(publicKeyFile, pPrivKey, true, nullptr, nullptr);
	} else {
		RSA_free(pRSA);
	}

	//Will clean up pRSA as well
	EVP_PKEY_free(pPrivKey);
	return status;
}

RSA* Rsa::create(const char *key, bool isPublicKey, char *password) noexcept {
	if (!key) {
		return nullptr;
	}

	auto keybio = BIO_new_mem_buf(key, -1);
	if (keybio == nullptr) {
		return 0;
	}

	RSA *rsa = nullptr;
	if (isPublicKey) {
		rsa = PEM_read_bio_RSA_PUBKEY(keybio, nullptr, nullptr, nullptr);
	} else {
		rsa = PEM_read_bio_RSAPrivateKey(keybio, nullptr, 0, password);
	}

	BIO_free_all(keybio);
	return rsa;
}

RSA* Rsa::createFromFile(const char *filename, bool isPublicKey,
		char *password) noexcept {
	if (Storage::testFile(filename) != 1) {
		return nullptr;
	}

	//Open file for reading in text mode
	auto fp = Storage::openStream(filename, "r", false);
	if (fp == nullptr) {
		return nullptr;
	}

	RSA *rsa = nullptr;
	if (isPublicKey) {
		rsa = PEM_read_RSA_PUBKEY(fp, nullptr, nullptr, nullptr);
	} else {
		rsa = PEM_read_RSAPrivateKey(fp, nullptr, 0, password);
	}

	Storage::closeStream(fp);
	return rsa;
}

void Rsa::destroyKey(RSA *rsa) noexcept {
	RSA_free(rsa);
}

int Rsa::publicEncrypt(RSA *rsa, const unsigned char *data, int dataLength,
		unsigned char *encrypted, int padding) noexcept {
	return RSA_public_encrypt(dataLength, data, encrypted, rsa, padding);
}

int Rsa::privateDecrypt(RSA *rsa, const unsigned char *encryptedData,
		int dataLength, unsigned char *decrypted, int padding) noexcept {
	return RSA_private_decrypt(dataLength, encryptedData, decrypted, rsa,
			padding);
}

int Rsa::privateEncrypt(RSA *rsa, const unsigned char *data, int dataLength,
		unsigned char *encrypted, int padding) noexcept {
	return RSA_private_encrypt(dataLength, data, encrypted, rsa, padding);
}

int Rsa::publicDecrypt(RSA *rsa, const unsigned char *encryptedData,
		int dataLength, unsigned char *decrypted, int padding) noexcept {
	return RSA_public_decrypt(dataLength, encryptedData, decrypted, rsa,
			padding);
}

int Rsa::signDigest(RSA *rsa, const unsigned char *digest,
		unsigned int digestLength, unsigned char *signature,
		unsigned int *signatureLength, int type) noexcept {
	return RSA_sign(type, digest, digestLength, signature, signatureLength, rsa);
}

int Rsa::verifyDigest(RSA *rsa, const unsigned char *digest,
		unsigned int digestLength, unsigned char *signature,
		unsigned int signatureLength, int type) noexcept {
	return RSA_verify(type, digest, digestLength, signature, signatureLength,
			rsa);
}

RSA* Rsa::generateRSAKey(int bits) noexcept {
	auto bigNumber = BN_new();
	if (!bigNumber || !BN_set_word(bigNumber, RSA_F4)) {
		BN_clear_free(bigNumber);
		return nullptr;
	}

	auto rsa = RSA_new();
	if (!rsa || !RSA_generate_key_ex(rsa, bits, bigNumber, nullptr)
			|| !RSA_check_key(rsa)) {
		RSA_free(rsa);
		BN_clear_free(bigNumber);
		return nullptr;
	}

	//Clean-up and return the RSA structure
	BN_clear_free(bigNumber);
	return rsa;
}

EVP_PKEY* Rsa::generatePrivateKey(RSA *rsa) noexcept {
	if (!rsa) {
		return nullptr;
	}

	auto pkey = EVP_PKEY_new();
	if (!pkey || !EVP_PKEY_assign_RSA(pkey, rsa)) {
		EVP_PKEY_free(pkey);
		return nullptr;
	}

	//Return the private key structure
	return pkey;
}

bool Rsa::generatePem(const char *filename, EVP_PKEY *pKey, bool isPublicKey,
		char *password, const EVP_CIPHER *cipher) noexcept {
	if (!filename || !pKey) {
		return false;
	}

	auto pCipher = password ? ((cipher ? cipher : EVP_aes_256_cbc())) : nullptr;
	auto passPhraseLength = password ? strlen(password) : 0;

	//Open the file for writing in text mode
	auto pFile = Storage::openStream(filename, "w", true);
	if (!pFile) {
		return false;
	}

	int ret = 0;
	if (isPublicKey) {
		ret = PEM_write_PUBKEY(pFile, pKey);
	} else {
		ret = PEM_write_PrivateKey(pFile, pKey, pCipher,
				(unsigned char*) password, passPhraseLength, nullptr, nullptr);
	}

	Storage::closeStream(pFile);
	return (bool) ret;
}

} /* namespace wanhive */
