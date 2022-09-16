/*
 * SSLContext.cpp
 *
 * Secure connection's context
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "SSLContext.h"
#include "CryptoUtils.h"
#include "../common/Exception.h"

namespace wanhive {

SSLContext::SSLContext() noexcept :
		ctx(nullptr) {

}

SSLContext::SSLContext(const char *certificate, const char *privateKey) :
		ctx(nullptr) {
	try {
		initialize(certificate, privateKey);
	} catch (const BaseException &e) {
		//Clean up the partially initialized object
		clear();
		throw;
	}
}

SSLContext::~SSLContext() {
	clear();
}

SSL_CTX* SSLContext::getContext() const noexcept {
	return ctx;
}

void SSLContext::initialize(const char *certificate, const char *privateKey) {
	if (!ctx && (ctx = SSL_CTX_new(SSLv23_method()))) {
		SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
		SSL_CTX_set_options(ctx,
				SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1
						| SSL_OP_NO_TLSv1_1);
	}

	if (!installKeys(certificate, privateKey)) {
		throw Exception(EX_SECURITY);
	}
}

void SSLContext::loadTrustedPaths(const char *file, const char *path) {
	if (!ctx) {
		throw Exception(EX_RESOURCE);
	} else if ((file || path)
			&& (SSL_CTX_load_verify_locations(ctx, file, path) != 1)) {
		throw Exception(EX_SECURITY);
	} else if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
		throw Exception(EX_SECURITY);
	} else {
		//success
	}
}

SSL* SSLContext::create(int fd, bool server) {
	SSL *ssl = nullptr;
	if (!ctx) {
		throw Exception(EX_RESOURCE);
	} else if ((ssl = SSL_new(ctx)) == nullptr) {
		throw Exception(EX_SECURITY);
	} else if (SSL_set_fd(ssl, fd) != 1) {
		SSL_free(ssl);
		throw Exception(EX_SECURITY);
	} else {
		if (server) {
			SSL_set_accept_state(ssl);
		} else {
			SSL_set_connect_state(ssl);
		}
		return ssl;
	}
}

bool SSLContext::inContext(const SSL *ssl) const noexcept {
	return ssl && (SSL_get_SSL_CTX(ssl) == ctx);
}

SSL* SSLContext::connect(int fd) {
	auto ssl = create(fd, false);
	if (SSL_connect(ssl) == 1 && SSLContext::verify(ssl)) {
		return ssl;
	} else {
		destroy(ssl);
		throw Exception(EX_SECURITY);
	}
}

int SSLContext::getSocket(const SSL *ssl) noexcept {
	return SSL_get_fd(ssl);
}

bool SSLContext::setSocket(SSL *ssl, int fd) noexcept {
	return ssl && (SSL_set_fd(ssl, fd) == 1);
}

bool SSLContext::verify(const SSL *ssl) noexcept {
	return ssl && (SSL_get_verify_result(ssl) == X509_V_OK);
}

void SSLContext::destroy(SSL *ssl) noexcept {
	SSL_free(ssl);
}

bool SSLContext::installKeys(const char *certificate,
		const char *privateKey) noexcept {
	if (!ctx) {
		return false;
	} else {
		return (!certificate
				|| SSL_CTX_use_certificate_chain_file(ctx, certificate) == 1)
				&& (!privateKey || (SSL_CTX_use_PrivateKey_file(ctx, privateKey,
				SSL_FILETYPE_PEM) == 1 && SSL_CTX_check_private_key(ctx) == 1));
	}
}

void SSLContext::clear() noexcept {
	SSL_CTX_free(ctx);
	ctx = nullptr;
}

size_t SSLContext::receiveStream(SSL *ssl, unsigned char *buf, size_t length) {
	if (!ssl || (!buf && length)) {
		throw Exception(EX_INVALIDPARAM);
	} else if (!length) {
		return 0;
	} else {
		auto toRecv = length;
		size_t index = 0;
		ssize_t n = 0;
		while (toRecv != 0) {
			if ((n = SSL_read(ssl, buf + index, toRecv)) > 0) {
				toRecv -= n;
				index += n;
			} else {
				throw Exception(EX_SECURITY);
			}
		}
		return (length - toRecv);
	}
}

size_t SSLContext::sendStream(SSL *ssl, const unsigned char *buf,
		size_t length) {
	if (!ssl || (!buf && length)) {
		throw Exception(EX_INVALIDPARAM);
	} else if (!length) {
		return 0;
	} else {
		auto toSend = length;
		size_t index = 0;
		ssize_t n = 0;
		while (toSend != 0) {
			if ((n = SSL_write(ssl, buf + index, toSend)) > 0) {
				toSend -= n;
				index += n;
			} else {
				throw Exception(EX_SECURITY);
			}
		}
		return (length - toSend);
	}
}

} /* namespace wanhive */

