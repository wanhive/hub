/*
 * Socket.cpp
 *
 * Message stream watcher
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Socket.h"
#include "Hub.h"
#include "../base/Selector.h"
#include "../base/ds/Twiddler.h"
#include "../base/security/CryptoUtils.h"
#include "../base/unix/SystemException.h"

namespace wanhive {

SSLContext *Socket::sslCtx = nullptr;

Socket::Socket(int fd) noexcept :
		Pooled(0), Watcher(fd) {
	clear();
}

Socket::Socket(SSL *ssl) :
		Pooled(0) {
	clear();
	if (ssl && sslCtx && sslCtx->inContext(ssl)) {
		secure.ssl = ssl;
		Descriptor::setHandle(SSLContext::getSocket(ssl));
	} else {
		throw Exception(EX_SECURITY);
	}
}

Socket::Socket(const NameInfo &ni, bool blocking, int timeout) :
		Pooled(0) {
	try {
		clear();
		SocketAddress sa;
		if (!strcasecmp(ni.service, "unix")) {
			Descriptor::setHandle(
					Network::unixConnectedSocket(ni.host, sa, blocking));
			setFlags(SOCKET_LOCAL);
		} else {
			Descriptor::setHandle(Network::connectedSocket(ni, sa, blocking));
		}
		if (blocking) {
			Network::setSocketTimeout(Descriptor::getHandle(), timeout,
					timeout);
		}
		setType(SOCKET_PROXY);
	} catch (const BaseException &e) {
		Descriptor::closeHandle();
		throw;
	}
}

Socket::Socket(const char *service, int backlog, bool isUnix, bool blocking) :
		Pooled(0) {
	try {
		clear();
		SocketAddress sa;
		if (!isUnix) {
			Descriptor::setHandle(Network::serverSocket(service, sa, blocking));
		} else {
			Descriptor::setHandle(
					Network::unixServerSocket(service, sa, blocking));
			setFlags(SOCKET_LOCAL);
		}
		Network::listen(Descriptor::getHandle(), backlog);
		setType(SOCKET_LISTENER);
	} catch (const BaseException &e) {
		Descriptor::closeHandle();
		throw;
	}
}

Socket::~Socket() {
	cleanup();
}

void* Socket::operator new(size_t size) {
	auto p = Pooled::operator new(size);
	if (p) {
		return p;
	} else {
		throw Exception(EX_MEMORY);
	}
}

void Socket::operator delete(void *p) noexcept {
	Pooled::operator delete(p);
}

size_t Socket::emit(unsigned char *dest, size_t count) noexcept {
	return in.read(dest, count);
}

bool Socket::emit(unsigned char &dest) noexcept {
	return in.get(dest);
}

size_t Socket::available() const noexcept {
	return in.readSpace();
}

void Socket::start() {

}

void Socket::stop() noexcept {
	Network::shutdown(Descriptor::getHandle());
}

bool Socket::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Socket> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Socket::publish(void *arg) noexcept {
	auto message = static_cast<Message*>(arg);
	if (message && (!outQueueLimit || out.readSpace() < outQueueLimit)
			&& out.put(message)) {
		message->addReferenceCount();
		setTrace(message->getTrace());
		setFlags(WATCHER_OUT);
		return true;
	} else {
		return false;
	}
}

void Socket::setTopic(unsigned int index) noexcept {
	if (subscriptions.set(index)) {
		setFlags(WATCHER_MULTICAST);
	}
}

void Socket::clearTopic(unsigned int index) noexcept {
	subscriptions.clear(index);
	if (!subscriptions.count()) {
		clearFlags(WATCHER_MULTICAST);
	}
}

bool Socket::testTopic(unsigned int index) const noexcept {
	return subscriptions.test(index);
}

unsigned long long Socket::getOption(int name) const noexcept {
	switch (name) {
	case WATCHER_READ_BUFFER_MAX:
		return READ_BUFFER_SIZE;
	case WATCHER_WRITE_BUFFER_MAX:
		return outQueueLimit;
	default:
		return 0;
	}
}

void Socket::setOption(int name, unsigned long long value) noexcept {
	switch (name) {
	case WATCHER_WRITE_BUFFER_MAX:
		outQueueLimit = Twiddler::min(value, (OUT_QUEUE_SIZE - 1));
		break;
	default:
		break;
	}
}

Socket* Socket::accept(bool blocking) {
	auto sfd = -1;
	try {
		SocketAddress sa;
		sfd = Network::accept(Descriptor::getHandle(), sa, blocking);
		if (sfd == -1) {
			clearEvents(IO_READ); //Would block
			return nullptr;
		}
		auto s = new Socket(sfd);
		if (testFlags(SOCKET_LOCAL)) {
			s->setFlags(SOCKET_LOCAL);
		}
		return s;
	} catch (const BaseException &e) {
		Network::close(sfd);
		throw;
	}
}

ssize_t Socket::read() {
	if (!sslCtx || testFlags(SOCKET_LOCAL)) {
		return socketRead();
	} else {
		return secureRead();
	}
}

ssize_t Socket::write() {
	if (!sslCtx || testFlags(SOCKET_LOCAL)) {
		return socketWrite();
	} else {
		return secureWrite();
	}
}

Message* Socket::getMessage() {
	if (incomingMessage == nullptr) {
		if (in.isEmpty()) { //:-)
			return nullptr;
		}
		incomingMessage = Message::create(getUid());
		if (incomingMessage == nullptr) {
			return nullptr;
		}
		incomingMessage->setType(getType());
		incomingMessage->putTrace(getTrace());
		incomingMessage->setGroup(getGroup());
		incomingMessage->setMarked();
	}

	try {
		if (incomingMessage->build(*this)) {
			totalIncomingMessages += 1;
			auto msg = incomingMessage;
			incomingMessage = nullptr;
			return msg;
		} else {
			return nullptr;
		}
	} catch (BaseException &e) {
		Message::recycle(incomingMessage);
		incomingMessage = nullptr;
		throw;
	}
}

Socket* Socket::createSocketPair(int &sfd, bool blocking) {
	int sv[2] = { -1, -1 };
	try {
		Network::socketPair(sv, blocking);
		sfd = sv[1];
		auto conn = new Socket(sv[0]);
		conn->setFlags(SOCKET_LOCAL);
		conn->setType(SOCKET_PROXY);
		return conn;
	} catch (const BaseException &e) {
		if (sv[0] != -1) {
			Network::close(sv[0]);
		}
		if (sv[1] != -1) {
			Network::close(sv[1]);
		}
		throw;
	}
}

void Socket::setSSLContext(SSLContext *ctx) noexcept {
	sslCtx = ctx;
}

ssize_t Socket::socketRead() {
	ssize_t nRecv = 0;
	CircularBufferVector<unsigned char> vector;
	//Receive data into the read buffer
	if (in.getWritable(vector)) {
		auto count = (vector.part[1].length) ? 2 : 1;
		iovec iovs[2] = { { vector.part[0].base, vector.part[0].length }, {
				vector.part[1].base, vector.part[1].length } };
		if ((nRecv = Descriptor::readv(iovs, count)) > 0) {
			in.skipWrite(nRecv);
		}
	}
	return nRecv;
}

ssize_t Socket::socketWrite() {
	auto iovCount = Twiddler::min(fillOutgoingQueue(), IOV_MAX);
	if (iovCount) {
		auto vec = outgoingMessages.offset();
		auto nSent = Descriptor::writev(vec, iovCount);
		adjustOutgoingQueue(nSent);
		return nSent;
	} else {
		//Nothing queued up
		clearFlags(WATCHER_OUT);
		return 0;
	}
}

ssize_t Socket::secureRead() {
	initSSL();

	if (secure.callWrite) {
		return secureWrite();
	}

	ssize_t nRecv = 0;
	CircularBufferVector<unsigned char> vector;
	//Receive data into both the segments
	if (in.getWritable(vector)) {
		CryptoUtils::clearErrors();
		for (unsigned int i = 0; i < 2; i++) {
			auto data = vector.part[i].base;
			auto length = vector.part[i].length;
			auto received = length ? sslRead(data, length) : 0;
			nRecv += received;
			in.skipWrite(received);
			if ((size_t) received != length) { //Partial read
				break;
			}
		}
	}
	return nRecv;
}

ssize_t Socket::secureWrite() {
	initSSL();

	if (secure.callRead) {
		return secureRead();
	}

	auto count = fillOutgoingQueue();
	if (count) {
		CryptoUtils::clearErrors();
		ssize_t nSent = 0;
		auto iovecs = outgoingMessages.offset();
		for (unsigned int i = 0; i < count; i++) {
			auto data = iovecs[i].iov_base;
			auto length = iovecs[i].iov_len;
			auto sent = length ? sslWrite(data, length) : 0;
			nSent += sent;
			if ((size_t) sent != length) { //Partial write
				break;
			}
		}
		adjustOutgoingQueue(nSent);
		return nSent;
	} else {
		//Nothing queued up
		clearFlags(WATCHER_OUT);
		return 0;
	}
}

void Socket::initSSL() {
	if (!sslCtx || (secure.callRead && secure.callWrite)) {
		throw Exception(EX_STATE);
	} else if (!secure.ssl) {
		secure.ssl = sslCtx->create(Descriptor::getHandle(),
				!isType(SOCKET_PROXY));
	} else if (secure.verified || !isType(SOCKET_PROXY)) {
		return;
	} else if (!SSL_is_init_finished(secure.ssl)) {
		return;
	} else if (SSLContext::verify(secure.ssl)) {
		secure.verified = true;
	} else {
		throw Exception(EX_SECURITY);
	}
}

ssize_t Socket::sslRead(void *buf, size_t count) {
	if (!secure.ssl || !buf || !count) {
		throw Exception(EX_ARGUMENT);
	}

	auto bytes = SSL_read(secure.ssl, buf, count);
	if (bytes > 0) {
		if (secure.callRead) {
			setEvents(IO_READ); //Restored
			secure.callRead = false;
		}
		return bytes;
	}

	switch (SSL_get_error(secure.ssl, bytes)) {
	case SSL_ERROR_WANT_READ:
		clearEvents(IO_READ);
		return 0;
		break;
	case SSL_ERROR_WANT_WRITE:
		clearEvents(IO_WR); //Will restore the read event later
		setFlags(WATCHER_OUT);
		secure.callRead = true;
		return 0;
		break;
	case SSL_ERROR_SYSCALL:
		throw SystemException();
		break;
	case SSL_ERROR_ZERO_RETURN:
		throw Exception(EX_RESOURCE);
		break;
	default:
		throw Exception(EX_SECURITY);
		break;
	}
}

ssize_t Socket::sslWrite(const void *buf, size_t count) {
	if (!secure.ssl || !buf || !count) {
		throw Exception(EX_ARGUMENT);
	}

	auto bytes = SSL_write(secure.ssl, buf, count);
	if (bytes > 0) {
		if (secure.callWrite) {
			setEvents(IO_WRITE); //Restored
			secure.callWrite = false;
		}
		return bytes;
	}

	switch (SSL_get_error(secure.ssl, bytes)) {
	case SSL_ERROR_WANT_READ:
		clearEvents(IO_WR); //Will restore the write event later
		secure.callWrite = true;
		return 0;
		break;
	case SSL_ERROR_WANT_WRITE:
		clearEvents(IO_WRITE);
		return 0;
		break;
	case SSL_ERROR_SYSCALL:
		throw SystemException();
		break;
	case SSL_ERROR_ZERO_RETURN:
		throw Exception(EX_RESOURCE);
		break;
	default:
		throw Exception(EX_SECURITY);
		break;
	}
}

unsigned int Socket::fillOutgoingQueue() noexcept {
	if (!outgoingMessages.hasSpace()) {
		CircularBufferVector<Message*> vector;
		auto space = out.getReadable(vector);
		if (space) {
			outgoingMessages.clear(); //Reset for writing
			space = Twiddler::min(space, outgoingMessages.capacity()); //Adjust

			auto iovecs = outgoingMessages.offset();
			unsigned int count = 0;
			for (unsigned int i = 0; i < 2; ++i) { //Two parts
				auto &mvecs = vector.part[i];
				for (size_t j = 0; ((j < mvecs.length) && (count < space));
						++j) {
					auto msg = mvecs.base[j];
					iovecs[count].iov_base = msg->buffer();
					iovecs[count].iov_len =
							msg->validate() ? msg->getLength() : 0;
					++count;
				}
			}
			totalOutgoingMessages += count;
			outgoingMessages.setIndex(count); //Update the index
			outgoingMessages.rewind(); //Prepare for reading
		}
	}

	return outgoingMessages.space();
}

void Socket::adjustOutgoingQueue(size_t bytes) noexcept {
	size_t total = 0;
	unsigned int sentMessages = 0;
	auto iovecs = outgoingMessages.offset();
	auto count = outgoingMessages.space();
	for (unsigned int index = 0; index < count; ++index) {
		iovec &iov = iovecs[index];
		total += iov.iov_len;
		if (total > bytes) {
			//This IOVEC has been consumed partially
			auto originalLength = iov.iov_len;
			iov.iov_len = (total - bytes);
			iov.iov_base = ((unsigned char*) (iov.iov_base))
					+ (originalLength - (total - bytes));
			break;
		}

		//We have sent this message, recycle it
		Message *msg = nullptr;
		out.get(msg);
		Message::recycle(msg);
		++sentMessages;
	}
	outgoingMessages.setIndex(outgoingMessages.getIndex() + sentMessages);
}

void Socket::clear() noexcept {
	memset(&secure, 0, sizeof(secure));
	incomingMessage = nullptr;
	totalIncomingMessages = 0;
	totalOutgoingMessages = 0;
	outQueueLimit = 0;
	outgoingMessages.rewind();
}

void Socket::cleanup() noexcept {
	SSLContext::destroy(secure.ssl);
	Message::recycle(incomingMessage);

	Message *message;
	while ((out.get(message))) {
		Message::recycle(message);
	}
}

} /* namespace wanhive */
