/*
 * Socket.cpp
 *
 * Watcher for sending and receiving messages
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

MemoryPool Socket::pool;
SSLContext *Socket::sslCtx = nullptr;

Socket::Socket(int fd) noexcept :
		Watcher(fd) {
	clear();
}

Socket::Socket(SSL *ssl) {
	clear();
	if (ssl && sslCtx && sslCtx->inContext(ssl)) {
		secure.ssl = ssl;
		setHandle(SSLContext::getSocket(ssl));
	} else {
		throw Exception(EX_SECURITY);
	}
}

Socket::Socket(const NameInfo &ni, bool blocking, int timeoutMils) {
	try {
		clear();
		SocketAddress sa;
		if (!strcasecmp(ni.service, "unix")) {
			setHandle(Network::unixConnectedSocket(ni.host, sa, blocking));
			setFlags(SOCKET_LOCAL);
		} else {
			setHandle(Network::connectedSocket(ni, sa, blocking));
		}
		if (blocking) {
			Network::setSocketTimeout(getHandle(), timeoutMils, timeoutMils);
		}
		setType(SOCKET_PROXY);
	} catch (const BaseException &e) {
		closeHandle();
		throw;
	}
}

Socket::Socket(const char *service, int backlog, bool isUnix, bool blocking) {
	try {
		clear();
		SocketAddress sa;
		if (!isUnix) {
			setHandle(Network::serverSocket(service, sa, blocking));
		} else {
			setHandle(Network::unixServerSocket(service, sa, blocking));
			setFlags(SOCKET_LOCAL);
		}
		Network::listen(getHandle(), backlog);
		setType(SOCKET_LISTENER);
	} catch (const BaseException &e) {
		closeHandle();
		throw;
	}
}

Socket::~Socket() {
	cleanup();
}

void* Socket::operator new(size_t size) {
	if (allocated() != poolSize()) {
		return pool.allocate();
	} else {
		throw Exception(EX_ALLOCFAILED);
	}
}

void Socket::operator delete(void *p) noexcept {
	pool.deallocate(p);
}

size_t Socket::take(unsigned char *buffer, size_t count) {
	return in.read(buffer, count);
}

size_t Socket::available() const noexcept {
	return in.readSpace();
}

void Socket::start() {

}

void Socket::stop() noexcept {
	Network::shutdown(this->getHandle());
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

Socket* Socket::accept(bool blocking) {
	auto sfd = -1;
	try {
		SocketAddress sa;
		sfd = Network::accept(this->getHandle(), sa, blocking);
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

bool Socket::hasTimedOut(unsigned int timeOut) const noexcept {
	return timer.hasTimedOut(timeOut);
}

void Socket::setOutputQueueLimit(unsigned int limit) noexcept {
	outQueueLimit = Twiddler::min(limit, (OUT_QUEUE_SIZE - 1));
}

unsigned int Socket::getOutputQueueLimit() const noexcept {
	return outQueueLimit;
}

bool Socket::isEphemeralId(unsigned long long id) noexcept {
	return id > MAX_ACTIVE_ID;
}

SSL* Socket::getSecureSocket() const noexcept {
	return secure.ssl;
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

void Socket::initPool(unsigned int size) {
	pool.initialize(sizeof(Socket), size);
}

void Socket::destroyPool() {
	if (pool.destroy()) {
		throw Exception(EX_INVALIDSTATE);
	}
}

unsigned int Socket::poolSize() noexcept {
	return pool.capacity();
}

unsigned int Socket::allocated() noexcept {
	return pool.allocated();
}

unsigned int Socket::unallocated() noexcept {
	//How many more connections can we create
	return poolSize() - allocated();
}

ssize_t Socket::socketRead() {
	ssize_t nRecv = 0;
	CircularBufferVector<unsigned char> vector;
	//Receive data into the read buffer
	if (in.getWritable(vector)) {
		auto count = (vector.part[1].length) ? 2 : 1;
		if ((nRecv = Descriptor::readv((const iovec*) &vector, count)) > 0) {
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
			ssize_t received = 0;
			if (length && (received = sslRead(data, length)) > 0) {
				in.skipWrite(received);
				nRecv += received;
				if ((size_t) received < length) { //Partial read
					break;
				}
			} else {
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
			auto sent = sslWrite(data, length);
			if (sent == 0) {
				break;
			} else if ((size_t) sent == length) {
				nSent += sent;
				continue;
			} else { //Partial write
				nSent += sent;
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
		throw Exception(EX_INVALIDSTATE);
	} else if (!secure.ssl) {
		secure.ssl = sslCtx->create(getHandle(), !isType(SOCKET_PROXY));
	} else if (!secure.verified && isType(SOCKET_PROXY)
			&& SSL_is_init_finished(secure.ssl)) {
		if (SSLContext::verify(secure.ssl)) {
			secure.verified = true;
		} else {
			throw Exception(EX_SECURITY);
		}
	}
}

ssize_t Socket::sslRead(void *buf, size_t count) {
	if (!secure.ssl || !buf || !count) {
		throw Exception(EX_INVALIDPARAM);
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
		throw Exception(EX_INVALIDPARAM);
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
				BufferVector<Message*> &mvecs = vector.part[i];
				for (size_t j = 0; ((j < mvecs.length) && (count < space));
						++j) {
					Message *msg = mvecs.base[j];
					iovecs[count].iov_base = msg->buffer();
					iovecs[count].iov_len = msg->getLength();
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
	if (bytes) {
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
