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
#include "../base/SystemException.h"
#include "../base/ds/Twiddler.h"
#include "../base/security/CryptoUtils.h"

namespace wanhive {

MemoryPool Socket::pool;
SSLContext *Socket::sslCtx = nullptr;

Socket::Socket(int fd) noexcept :
		Watcher(fd) {
	clear();
}

Socket::Socket(int fd, const SocketAddress &sa) noexcept :
		Watcher(fd) {
	clear();
	address = sa;
}

Socket::Socket(const NameInfo &ni, bool blocking, int timeoutMils) {
	try {
		clear();
		if (!strcasecmp(ni.service, "unix")) {
			setHandle(Network::unixConnectedSocket(ni.host, address, blocking));
		} else {
			setHandle(Network::connectedSocket(ni, address, blocking));
		}
		if (blocking) {
			Network::setSocketTimeout(getHandle(), timeoutMils, timeoutMils);
		}
		setType(SOCKET_PROXY);
	} catch (BaseException &e) {
		closeHandle();
		throw;
	}
}

Socket::Socket(const char *service, int backlog, bool isUnix, bool blocking) {
	try {
		clear();
		if (!isUnix) {
			setHandle(Network::serverSocket(service, address, blocking));
		} else {
			setHandle(Network::unixServerSocket(service, address, blocking));
		}
		Network::listen(getHandle(), backlog);
		setType(SOCKET_LISTENER);
	} catch (BaseException &e) {
		closeHandle();
		throw;
	}
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

void Socket::start() {

}

void Socket::stop() noexcept {
	shutdown();
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
	SocketAddress sa;
	int flag = blocking ? 0 : SOCK_NONBLOCK;
	auto sfd = Network::accept(this->getHandle(), sa, flag);
	if (sfd == -1) {
		//Would block
		clearEvents(IO_READ);
		return nullptr;
	}

	try {
		return new Socket(sfd, sa);
	} catch (BaseException &e) {
		Network::close(sfd);
		throw;
	}
}

int Socket::shutdown(int how) noexcept {
	return Network::shutdown(this->getHandle(), how);
}

ssize_t Socket::read() {
	if (!sslCtx || isType(SOCKET_LOCAL)) {
		return socketRead();
	} else {
		return secureRead();
	}
}

ssize_t Socket::write() {
	if (!sslCtx || isType(SOCKET_LOCAL)) {
		return socketWrite();
	} else {
		return secureWrite();
	}
}

Message* Socket::getMessage() {
	if (incomingMessage == nullptr) {
		if (in.isEmpty()) {
			return nullptr;
		}
		incomingMessage = Message::create(getUid());
		if (incomingMessage == nullptr) {
			return nullptr;
		}
		incomingMessage->setType(getType());
		incomingMessage->putFlags(MSG_WAIT_HEADER);
		incomingMessage->setGroup(getGroup());
		incomingMessage->setMarked();
	}

	Message *msg = nullptr;
	switch (incomingMessage->getFlags()) {
	case MSG_WAIT_HEADER:
		if (in.readSpace() >= Message::HEADER_SIZE) {
			in.read(incomingMessage->getStorage(), Message::HEADER_SIZE);
			incomingMessage->prepareHeader();
			incomingMessage->putFlags(MSG_WAIT_DATA);
		} else {
			return nullptr;
		}
		/* no break */
	case MSG_WAIT_DATA:
		if (incomingMessage->testLength()) {
			if (in.readSpace() < incomingMessage->getPayloadLength()) {
				return nullptr;
			} else {
				in.read(incomingMessage->getStorage(),
						incomingMessage->getPayloadLength());
				incomingMessage->prepareData();
				incomingMessage->putFlags(MSG_WAIT_PROCESSING);
			}
		} else {
			Message::recycle(incomingMessage);
			incomingMessage = nullptr;
			throw Exception(EX_INVALIDRANGE);
		}
		/* no break */
	case MSG_WAIT_PROCESSING:
		totalIncomingMessages += 1;
		msg = incomingMessage;
		incomingMessage = nullptr;
		return msg;
	default:
		return nullptr;
	}
	return nullptr;
}

SocketAddress const& Socket::getAddress() const noexcept {
	return address;
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
		conn->setType(SOCKET_LOCAL);
		return conn;
	} catch (BaseException &e) {
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
		int nVectors = 2;
		if (vector.part[1].length == 0) {
			nVectors = 1;
		}

		if ((nRecv = Watcher::read((const iovec*) &vector, nVectors)) > 0) {
			in.skipWrite(nRecv);
		}
	}
	return nRecv;
}

ssize_t Socket::socketWrite() {
	auto iovCount = Twiddler::min(fillOutgoingQueue(), IOV_MAX);
	if (iovCount) {
		auto vec = outgoingMessages.offset();
		auto nSent = Watcher::write(vec, iovCount);
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
			auto count = vector.part[i].length;
			auto data = vector.part[i].base;
			ssize_t received = 0;
			if (count && (received = sslRead(data, count)) > 0) {
				in.skipWrite(received);
				nRecv += received;
				if ((size_t) received < count) { //Partial read
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

	auto iovCount = fillOutgoingQueue();
	if (iovCount) {
		auto vec = outgoingMessages.offset();
		ssize_t nSent = 0;
		CryptoUtils::clearErrors();
		for (unsigned int i = 0; i < iovCount; i++) {
			auto sent = sslWrite(vec[i].iov_base, vec[i].iov_len);
			if (sent == 0) {
				break;
			} else if ((size_t) sent == vec[i].iov_len) {
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
		//Number of outgoing messages queued up, vector has two parts
		auto space = out.getReadable(vector);
		if (space) {
			//Reset for next write cycle
			outgoingMessages.clear();
			//How many outgoing messages can be queued up
			space = Twiddler::min(space, outgoingMessages.capacity());
			//vec points to the storage of the IOVEC buffer
			auto vec = outgoingMessages.offset();
			unsigned int iovCount = 0;
			for (unsigned int i = 0; i < 2; ++i) {
				for (unsigned int j = 0;
						((j < vector.part[i].length) && (iovCount < space));
						++j) {
					vec[iovCount].iov_base =
							vector.part[i].base[j]->getStorage();
					vec[iovCount].iov_len = vector.part[i].base[j]->remaining();
					iovCount++;
				}
			}
			totalOutgoingMessages += iovCount;
			outgoingMessages.setIndex(iovCount); //Move the index forward
			outgoingMessages.rewind();			//Prepare for next read cycle
		}
	}

	return outgoingMessages.space();
}

void Socket::adjustOutgoingQueue(size_t count) noexcept {
	auto vec = outgoingMessages.offset();
	auto iovCount = outgoingMessages.space();
	if (count) {
		size_t total = 0;
		unsigned int dispatchedMessageCount = 0;
		for (unsigned int index = 0; index < iovCount; ++index) {
			total += vec[index].iov_len;
			if (total > count) {
				//This IOVEC has been consumed only partially
				auto originalLength = vec[index].iov_len;
				vec[index].iov_len = (total - count);
				vec[index].iov_base = ((unsigned char*) (vec[index].iov_base))
						+ (originalLength - (total - count));
				break;
			}

			//We have dispatched this message, recycle it
			Message *msg = nullptr;
			out.get(msg);
			Message::recycle(msg);
			++dispatchedMessageCount;
		}
		outgoingMessages.setIndex(
				outgoingMessages.getIndex() + dispatchedMessageCount);
	}
}

void Socket::clear() noexcept {
	memset(&secure, 0, sizeof(secure));
	incomingMessage = nullptr;
	totalIncomingMessages = 0;
	totalOutgoingMessages = 0;
	outQueueLimit = 0;
	outgoingMessages.rewind();
	memset(&address, 0, sizeof(address));
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
