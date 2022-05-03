/*
 * Endpoint.cpp
 *
 * Request-response pattern implementation
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
		sockfd(-1), ssl(nullptr), sslContext(nullptr), pki(nullptr) {

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
	this->pki = pki;
}

const PKI* Endpoint::getKeyPair() const noexcept {
	return pki;
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
	auto pki = sign ? getKeyPair() : nullptr;
	if (!ssl) {
		send(sockfd, *this, pki);
	} else {
		send(ssl, *this, pki);
	}
}

void Endpoint::receive(unsigned int sequenceNumber, bool verify) {
	auto pki = verify ? getKeyPair() : nullptr;
	if (!ssl) {
		receive(sockfd, *this, sequenceNumber, pki);
	} else {
		receive(ssl, *this, sequenceNumber, pki);
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
	MessageHeader::writeSource(buffer(), d);
	MessageHeader::writeDestination(buffer(), s);
	MessageHeader::writeStatus(buffer(), WH_AQLF_ACCEPTED);
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

void Endpoint::send(int sfd, Packet &packet, const PKI *pki) {
	if (!packet.validate()) {
		throw Exception(EX_INVALIDRANGE);
	} else if (!packet.sign(pki)) {
		throw Exception(EX_SECURITY);
	} else {
		Network::sendStream(sfd, packet.buffer(), packet.header().getLength());
	}
}

void Endpoint::send(SSL *ssl, Packet &packet, const PKI *pki) {
	if (!packet.validate()) {
		throw Exception(EX_INVALIDRANGE);
	} else if (!packet.sign(pki)) {
		throw Exception(EX_SECURITY);
	} else {
		SSLContext::sendStream(ssl, packet.buffer(),
				packet.header().getLength());
	}
}

void Endpoint::receive(int sfd, Packet &packet, unsigned int sequenceNumber,
		const PKI *pki) {
	packet.clear();
	do {
		//Receive the header
		Network::receiveStream(sfd, packet.buffer(), HEADER_SIZE);

		//Prepare the header and the frame buffer
		if (!packet.unpackHeader()) {
			throw Exception(EX_INVALIDRANGE);
		}

		//Receive the payload
		Network::receiveStream(sfd, packet.payload(),
				packet.getPayloadLength());
	} while (sequenceNumber
			&& (packet.header().getSequenceNumber() != sequenceNumber));

	if (!packet.verify(pki)) {
		throw Exception(EX_SECURITY);
	}
}

void Endpoint::receive(SSL *ssl, Packet &packet, unsigned int sequenceNumber,
		const PKI *pki) {
	packet.clear();
	do {
		//Receive the header
		SSLContext::receiveStream(ssl, packet.buffer(), HEADER_SIZE);

		//Prepare the header and the frame buffer
		if (!packet.unpackHeader()) {
			throw Exception(EX_INVALIDRANGE);
		}

		//Receive the payload
		SSLContext::receiveStream(ssl, packet.payload(),
				packet.getPayloadLength());
	} while (sequenceNumber
			&& (packet.header().getSequenceNumber() != sequenceNumber));

	if (!packet.verify(pki)) {
		throw Exception(EX_SECURITY);
	}
}

} /* namespace wanhive */
