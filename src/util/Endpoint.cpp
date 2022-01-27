/*
 * Endpoint.cpp
 *
 * Basic routines for protocol implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Endpoint.h"
#include "commands.h"
#include "../base/common/Exception.h"
#include "../base/ds/Serializer.h"
#include "../base/security/CryptoUtils.h"

namespace wanhive {

Endpoint::Endpoint() noexcept :
		sockfd(-1), ssl(nullptr), sslContext(nullptr) {

}

Endpoint::~Endpoint() {
	disconnect();
}

void Endpoint::setSSLContext(SSLContext *ctx) noexcept {
	sslContext = ctx;
}

SSLContext* Endpoint::getSSLContext() const noexcept {
	return sslContext;
}

void Endpoint::useKeyPair(const PKI *pki) noexcept {
	trust.set(pki);
}

const PKI* Endpoint::getKeyPair() const noexcept {
	return trust.get();
}

void Endpoint::connect(const NameInfo &ni, int timeoutMils) {
	auto sfd = -1;
	try {
		SocketAddress sa;
		sfd = connect(ni, sa, timeoutMils);
		if (sa.address.ss_family == AF_UNIX) {
			setSSLContext(nullptr);
		}
		setSocket(sfd);
	} catch (const BaseException &e) {
		Network::close(sfd);
		throw;
	}
}

void Endpoint::disconnect() {
	Network::close(sockfd);
	sockfd = -1;
	SSLContext::destroy(ssl);
	ssl = nullptr;
}

int Endpoint::getSocket() const noexcept {
	return sockfd;
}

SSL* Endpoint::getSecureSocket() const noexcept {
	return ssl;
}

void Endpoint::setSocket(int sfd) {
	try {
		if (sfd == this->sockfd) {
			return;
		} else {
			SSL *ssl = nullptr;
			if (sslContext) {
				ssl = sslContext->connect(sfd);
			}
			//No error
			disconnect();
			this->sockfd = sfd;
			this->ssl = ssl;
		}
	} catch (const BaseException &e) {
		throw;
	}
}

void Endpoint::setSecureSocket(SSL *ssl) {
	if (ssl == this->ssl) {
		return;
	} else if (ssl && sslContext && sslContext->inContext(ssl)) {
		disconnect();
		this->sockfd = SSLContext::getSocket(ssl);
		this->ssl = ssl;
	} else {
		throw Exception(EX_SECURITY);
	}
}

int Endpoint::releaseSocket() noexcept {
	auto tmp = sockfd;
	sockfd = -1;
	SSLContext::destroy(ssl);
	ssl = nullptr;
	return tmp;
}

SSL* Endpoint::releaseSecureSocket() noexcept {
	if (ssl) {
		auto tmp = ssl;
		sockfd = -1;
		ssl = nullptr;
		return tmp;
	} else {
		return nullptr;
	}
}

int Endpoint::swapSocket(int sfd) {
	if (sfd == this->sockfd) {
		return sfd;
	} else if (!ssl || SSLContext::setSocket(ssl, sfd)) {
		auto tmp = sockfd;
		sockfd = sfd;
		return tmp;
	} else {
		throw Exception(EX_SECURITY);
	}
}

SSL* Endpoint::swapSecureSocket(SSL *ssl) {
	if (ssl == this->ssl) {
		return ssl;
	} else if (ssl && this->ssl && sslContext && sslContext->inContext(ssl)
			&& this->sockfd == SSLContext::getSocket(this->ssl)) {
		auto tmp = this->ssl;
		this->ssl = ssl;
		this->sockfd = SSLContext::getSocket(ssl);
		return tmp;
	} else {
		throw Exception(EX_SECURITY);
	}
}

void Endpoint::setSocketTimeout(int recvTimeout, int sendTimeout) const {
	Network::setSocketTimeout(sockfd, recvTimeout, sendTimeout);
}

void Endpoint::send(bool sign) {
	if (!ssl) {
		Endpoint::send(sockfd, buffer(), header().getLength(),
				sign ? getKeyPair() : nullptr);
	} else {
		Endpoint::send(ssl, buffer(), header().getLength(),
				sign ? getKeyPair() : nullptr);
	}
}

void Endpoint::receive(unsigned int sequenceNumber, bool verify) {
	if (!ssl) {
		Endpoint::receive(sockfd, buffer(), header(), sequenceNumber,
				verify ? getKeyPair() : nullptr);
	} else {
		Endpoint::receive(ssl, buffer(), header(), sequenceNumber,
				verify ? getKeyPair() : nullptr);
	}
}

bool Endpoint::executeRequest(bool sign, bool verify) {
	send(sign);
	receive(header().getSequenceNumber(), verify);
	return (header().getStatus() == WH_AQLF_ACCEPTED);
}

void Endpoint::sendPong() {
	receive();
	auto s = header().getSource();
	auto d = header().getDestination();
	MessageHeader::setSource(buffer(), d);
	MessageHeader::setDestination(buffer(), s);
	MessageHeader::setStatus(buffer(), WH_AQLF_ACCEPTED);
	send();
}

int Endpoint::connect(const NameInfo &ni, SocketAddress &sa, int timeoutMils) {
	auto sfd = -1;
	try {
		if (!strcasecmp(ni.service, "unix")) {
			sfd = Network::unixConnectedSocket(ni.host, sa, true);
		} else {
			sfd = Network::connectedSocket(ni, sa, true);
		}
		Network::setSocketTimeout(sfd, timeoutMils, timeoutMils);
	} catch (const BaseException &e) {
		if (sfd != -1) {
			Network::close(sfd);
		}
		throw;
	}
	return sfd;
}

void Endpoint::send(int sfd, unsigned char *buf, unsigned int length,
		const PKI *pki) {
	if (!buf || !testLength(length)) {
		throw Exception(EX_INVALIDPARAM);
	} else if (!Trust(pki).sign(buf, length)) {
		throw Exception(EX_SECURITY);
	} else {
		Network::sendStream(sfd, buf, length);
	}
}

void Endpoint::send(SSL *ssl, unsigned char *buf, unsigned int length,
		const PKI *pki) {
	if (!buf || !testLength(length)) {
		throw Exception(EX_INVALIDPARAM);
	} else if (!Trust(pki).sign(buf, length)) {
		throw Exception(EX_SECURITY);
	} else {
		SSLContext::sendStream(ssl, buf, length);
	}
}

void Endpoint::receive(int sfd, unsigned char *buf, MessageHeader &header,
		unsigned int sequenceNumber, const PKI *pki) {
	if (!buf) { //take the exceptional case out of the way
		throw Exception(EX_NULL);
	}

	while (true) {
		Network::receiveStream(sfd, buf, HEADER_SIZE);
		header.deserialize(buf);
		if (!testLength(header.getLength())) {
			throw Exception(EX_INVALIDRANGE);
		}

		Network::receiveStream(sfd, (buf + HEADER_SIZE),
				(header.getLength() - HEADER_SIZE));

		if (!sequenceNumber || (header.getSequenceNumber() == sequenceNumber)) {
			break;
		}
	}

	if (!Trust(pki).verify(buf, header.getLength())) {
		throw Exception(EX_SECURITY);
	}
}

void Endpoint::receive(SSL *ssl, unsigned char *buf, MessageHeader &header,
		unsigned int sequenceNumber, const PKI *pki) {
	if (!buf) { //take the exceptional case out of the way
		throw Exception(EX_NULL);
	}

	while (true) {
		SSLContext::receiveStream(ssl, buf, HEADER_SIZE);
		header.deserialize(buf);
		if (!testLength(header.getLength())) {
			throw Exception(EX_INVALIDRANGE);
		}

		SSLContext::receiveStream(ssl, (buf + HEADER_SIZE),
				(header.getLength() - HEADER_SIZE));

		if (!sequenceNumber || (header.getSequenceNumber() == sequenceNumber)) {
			break;
		}
	}

	if (!Trust(pki).verify(buf, header.getLength())) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
