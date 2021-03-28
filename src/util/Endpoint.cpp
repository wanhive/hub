/*
 * Endpoint.cpp
 *
 * Basic routines for Wanhive protocol implementation
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
		sockfd(-1), ssl(nullptr), sslContext(nullptr), pki(nullptr), sourceId(
				0), sequenceNumber(0), session(0) {

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

void Endpoint::setSocket(int socket) {
	try {
		SSL *ssl = nullptr;
		if (sslContext) {
			ssl = sslContext->connect(socket);
		}
		//No error
		disconnect();
		this->sockfd = socket;
		this->ssl = ssl;
	} catch (const BaseException &e) {
		throw;
	}
}

void Endpoint::setSecureSocket(SSL *ssl) {
	if (ssl && sslContext && sslContext->inContext(ssl)) {
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

int Endpoint::swapSocket(int socket) {
	if (!ssl || SSLContext::setSocket(ssl, socket)) {
		auto tmp = sockfd;
		sockfd = socket;
		return tmp;
	} else {
		throw Exception(EX_SECURITY);
	}
}

SSL* Endpoint::swapSecureSocket(SSL *ssl) {
	if (ssl && this->ssl && sslContext && sslContext->inContext(ssl)
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

void Endpoint::setSource(uint64_t source) noexcept {
	this->sourceId = source;
}

uint64_t Endpoint::getSource() const noexcept {
	return sourceId;
}

void Endpoint::setSequenceNumber(uint16_t sequenceNumber) noexcept {
	this->sequenceNumber = sequenceNumber;
}

uint16_t Endpoint::getSequenceNumber() const noexcept {
	return sequenceNumber;
}

uint16_t Endpoint::nextSequenceNumber() noexcept {
	if (!sequenceNumber) {
		sequenceNumber = 1;
	}
	return sequenceNumber++;
}

void Endpoint::setSession(uint8_t session) noexcept {
	this->session = session;
}

uint8_t Endpoint::getSession() const noexcept {
	return session;
}

const MessageHeader& Endpoint::getHeader() const noexcept {
	return _header;
}

const unsigned char* Endpoint::getBuffer(unsigned int offset) const noexcept {
	if (offset < Message::MTU) {
		return (_buffer + offset);
	} else {
		return nullptr;
	}
}

const unsigned char* Endpoint::getPayload(unsigned int offset) const noexcept {
	if (offset < Message::PAYLOAD_SIZE) {
		return (_buffer + Message::HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
}

bool Endpoint::pack(const MessageHeader &header,
		const unsigned char *payload) noexcept {
	if (!Message::testLength(header.getLength())) {
		return false;
	} else if (header.getLength() > Message::HEADER_SIZE && !payload) {
		return false;
	} else {
		_header = header;
		_header.serialize(_buffer);
		if (payload) {
			memcpy(_buffer + Message::HEADER_SIZE, payload,
					_header.getLength() - Message::HEADER_SIZE);
		}
		return true;
	}
}

bool Endpoint::pack(const unsigned char *message) noexcept {
	if (message) {
		_header.deserialize(message);
		if (Message::testLength(_header.getLength())) {
			memcpy(_buffer, message, _header.getLength());
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Endpoint::pack(const MessageHeader &header, const char *format,
		...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = pack(header, format, ap);
	va_end(ap);
	return ret;
}

bool Endpoint::pack(const MessageHeader &header, const char *format,
		va_list ap) noexcept {
	_header = header;
	_header.setLength(0); //Ignore the length

	auto size = _header.serialize(_buffer);
	size += Serializer::vpack(_buffer + Message::HEADER_SIZE,
			Message::PAYLOAD_SIZE, format, ap);

	if (format && format[0] && size == Message::HEADER_SIZE) {
		//Payload formatting error
		return false;
	} else {
		_header.setLength(size);
		MessageHeader::setLength(_buffer, _header.getLength());
		return true;
	}
}

bool Endpoint::append(const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = append(format, ap);
	va_end(ap);
	return ret;
}

bool Endpoint::append(const char *format, va_list ap) noexcept {
	if (!Message::testLength(_header.getLength())
			|| MessageHeader::getLength(_buffer) != _header.getLength()) {
		//Incorrect message length
		return false;
	}

	auto size = Serializer::vpack(_buffer + _header.getLength(),
			Message::MTU - _header.getLength(), format, ap);
	if (size) {
		_header.setLength(_header.getLength() + size);
		MessageHeader::setLength(_buffer, _header.getLength());
		return true;
	} else {
		return false;
	}
}

void Endpoint::unpackHeader(MessageHeader &header) const noexcept {
	header.deserialize(_buffer);
}

bool Endpoint::unpack(const char *format, ...) const noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = unpack(format, ap);
	va_end(ap);
	return ret;
}

bool Endpoint::unpack(const char *format, va_list ap) const noexcept {
	auto msgLength = _header.getLength();
	if (format && Message::testLength(msgLength)
			&& MessageHeader::getLength(_buffer) == msgLength) {
		return Serializer::vunpack(_buffer + Message::HEADER_SIZE,
				(msgLength - Message::HEADER_SIZE), format, ap);
	} else {
		return false;
	}
}

bool Endpoint::checkCommand(uint8_t command, uint8_t qualifier) const noexcept {
	return _header.getCommand() == command
			&& _header.getQualifier() == qualifier;
}

bool Endpoint::checkCommand(uint8_t command, uint8_t qualifier,
		uint8_t status) const noexcept {
	return checkCommand(command, qualifier) && _header.getStatus() == status;
}

void Endpoint::send(bool sign) {
	if (!ssl) {
		Endpoint::send(sockfd, _buffer, _header.getLength(),
				sign ? pki : nullptr);
	} else {
		Endpoint::send(ssl, _buffer, _header.getLength(), sign ? pki : nullptr);
	}
}

void Endpoint::receive(unsigned int sequenceNumber, bool verify) {
	if (!ssl) {
		Endpoint::receive(sockfd, _buffer, _header, sequenceNumber,
				verify ? pki : nullptr);
	} else {
		Endpoint::receive(ssl, _buffer, _header, sequenceNumber,
				verify ? pki : nullptr);
	}
}

bool Endpoint::executeRequest(bool sign, bool verify) {
	send(sign);
	receive(_header.getSequenceNumber(), verify);
	return (_header.getStatus() == WH_AQLF_ACCEPTED);
}

void Endpoint::sendPong() {
	receive();
	auto s = _header.getSource();
	auto d = _header.getDestination();
	MessageHeader::setSource(_buffer, d);
	MessageHeader::setDestination(_buffer, s);
	MessageHeader::setStatus(_buffer, WH_AQLF_ACCEPTED);
	send();
}

MessageHeader& Endpoint::header() noexcept {
	return _header;
}

unsigned char* Endpoint::buffer(unsigned int offset) noexcept {
	if (offset < Message::MTU) {
		return (_buffer + offset);
	} else {
		return nullptr;
	}
}

unsigned char* Endpoint::payload(unsigned int offset) noexcept {
	if (offset < Message::PAYLOAD_SIZE) {
		return (_buffer + Message::HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
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

void Endpoint::send(int sockfd, unsigned char *buf, unsigned int length,
		const PKI *pki) {
	if (!buf || !Message::testLength(length)) {
		throw Exception(EX_INVALIDPARAM);
	} else if (!sign(buf, length, pki)) {
		throw Exception(EX_SECURITY);
	} else {
		Network::sendStream(sockfd, buf, length);
	}
}

void Endpoint::send(SSL *ssl, unsigned char *buf, unsigned int length,
		const PKI *pki) {
	if (!buf || !Message::testLength(length)) {
		throw Exception(EX_INVALIDPARAM);
	} else if (!sign(buf, length, pki)) {
		throw Exception(EX_SECURITY);
	} else {
		SSLContext::sendStream(ssl, buf, length);
	}
}

void Endpoint::receive(int sockfd, unsigned char *buf, MessageHeader &header,
		unsigned int sequenceNumber, const PKI *pki) {
	if (!buf) { //take the exceptional case out of the way
		throw Exception(EX_NULL);
	}

	while (true) {
		Network::receiveStream(sockfd, buf, Message::HEADER_SIZE);
		header.deserialize(buf);
		if (!Message::testLength(header.getLength())) {
			throw Exception(EX_INVALIDRANGE);
		}

		Network::receiveStream(sockfd, (buf + Message::HEADER_SIZE),
				(header.getLength() - Message::HEADER_SIZE));

		if (!sequenceNumber || (header.getSequenceNumber() == sequenceNumber)) {
			break;
		}
	}

	if (!verify(buf, header.getLength(), pki)) {
		throw Exception(EX_SECURITY);
	}
}

void Endpoint::receive(SSL *ssl, unsigned char *buf, MessageHeader &header,
		unsigned int sequenceNumber, const PKI *pki) {
	if (!buf) { //take the exceptional case out of the way
		throw Exception(EX_NULL);
	}

	while (true) {
		SSLContext::receiveStream(ssl, buf, Message::HEADER_SIZE);
		header.deserialize(buf);
		if (!Message::testLength(header.getLength())) {
			throw Exception(EX_INVALIDRANGE);
		}

		SSLContext::receiveStream(ssl, (buf + Message::HEADER_SIZE),
				(header.getLength() - Message::HEADER_SIZE));

		if (!sequenceNumber || (header.getSequenceNumber() == sequenceNumber)) {
			break;
		}
	}

	if (!verify(buf, header.getLength(), pki)) {
		throw Exception(EX_SECURITY);
	}
}

unsigned int Endpoint::pack(const MessageHeader &header, unsigned char *buffer,
		const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto size = pack(header, buffer, format, ap);
	va_end(ap);
	return size;
}

unsigned int Endpoint::pack(const MessageHeader &header, unsigned char *buffer,
		const char *format, va_list ap) noexcept {
	if (buffer) {
		auto size = header.serialize(buffer);
		size += Serializer::vpack(buffer + Message::HEADER_SIZE,
				Message::PAYLOAD_SIZE, format, ap);
		MessageHeader::setLength(buffer, size);
		return size;
	} else {
		return 0;
	}
}

unsigned int Endpoint::unpack(MessageHeader &header,
		const unsigned char *buffer, const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto size = unpack(header, buffer, format, ap);
	va_end(ap);
	return size;
}

unsigned int Endpoint::unpack(MessageHeader &header,
		const unsigned char *buffer, const char *format, va_list ap) noexcept {
	if (buffer) {
		auto size = header.deserialize(buffer);
		auto msgLength = header.getLength();
		if (Message::testLength(msgLength)) {
			size += Serializer::vunpack(buffer + Message::HEADER_SIZE,
					(msgLength - Message::HEADER_SIZE), format, ap);
			return size;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

bool Endpoint::sign(unsigned char *out, unsigned int &length,
		const PKI *pki) noexcept {
	if (pki == nullptr) {
		return true;
	}
	//Make sure that we have got enough space for appending the signature
	if (!out || length < Message::HEADER_SIZE
			|| (length + PKI::SIGNATURE_LENGTH) > Message::MTU) {
		return false;
	}
	//--------------------------------------------------------------------------
	Signature signature;
	//Finalize the buffer, otherwise verification will fail
	MessageHeader::setLength(out, length + PKI::SIGNATURE_LENGTH);
	if (pki->sign(out, length, &signature)) {
		Serializer::packib((out + length), (unsigned char*) &signature,
				PKI::SIGNATURE_LENGTH);
		length += PKI::SIGNATURE_LENGTH;
		return true;
	} else {
		//Roll Back
		MessageHeader::setLength(out, length);
		return false;
	}
}

bool Endpoint::sign(Message *msg, const PKI *pki) noexcept {
	if (msg && msg->validate()) {
		unsigned int length = msg->getLength();
		auto ret = sign(msg->getStorage(), length, pki);
		//Update the message length
		msg->putLength(length);
		return ret;
	} else {
		return false;
	}
}

bool Endpoint::verify(const unsigned char *in, unsigned int length,
		const PKI *pki) noexcept {
	if (pki == nullptr) {
		return true;
	}

	//Make sure that the message is long enough to carry a signature
	if (in && length <= Message::MTU
			&& length >= (PKI::SIGNATURE_LENGTH + Message::HEADER_SIZE)) {
		auto bufLength = length - PKI::SIGNATURE_LENGTH;
		auto block = in;
		auto sign = (const Signature*) (block + bufLength);
		return pki->verify(block, bufLength, sign);
	} else {
		return false;
	}
}

bool Endpoint::verify(const Message *msg, const PKI *pki) noexcept {
	return msg && msg->validate()
			&& verify(msg->getStorage(), msg->getLength(), pki);
}

bool Endpoint::executeRequest(int sfd, MessageHeader &header,
		unsigned char *buf, const PKI *signer, const PKI *verifier) {
	send(sfd, buf, header.getLength(), signer);
	receive(sfd, buf, header, header.getSequenceNumber(), verifier);
	return (header.getStatus() == WH_AQLF_ACCEPTED);
}

bool Endpoint::executeRequest(SSL *ssl, MessageHeader &header,
		unsigned char *buf, const PKI *signer, const PKI *verifier) {
	send(ssl, buf, header.getLength(), signer);
	receive(ssl, buf, header, header.getSequenceNumber(), verifier);
	return (header.getStatus() == WH_AQLF_ACCEPTED);
}

} /* namespace wanhive */
