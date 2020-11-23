/*
 * Descriptor.cpp
 *
 * Resource descriptor
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Descriptor.h"
#include "../base/Selector.h"
#include "../base/SystemException.h"
#include "../base/common/Atomic.h"
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

namespace wanhive {

unsigned long long Descriptor::_nextUid = MIN_TMP_ID;

Descriptor::Descriptor() noexcept :
		uid(nextUid()), fd(-1) {

}

Descriptor::Descriptor(int fd) noexcept :
		uid(nextUid()), fd(fd) {

}

Descriptor::~Descriptor() {
	closeHandle();
}

void Descriptor::setUid(unsigned long long uid) noexcept {
	this->uid = uid;
}

unsigned long long Descriptor::getUid() const noexcept {
	return uid;
}

int Descriptor::getHandle() const noexcept {
	return fd;
}

void Descriptor::setBlocking(bool block) {
	auto flags = fcntl(fd, F_GETFL);
	if (flags != -1) {
		if (block) {
			flags &= ~O_NONBLOCK;
		} else {
			flags |= O_NONBLOCK;
		}

		if (fcntl(fd, F_SETFL, flags) == 0) {
			return;
		}
	}
	//If we have reached here, then there must have been an error
	throw SystemException();
}

void Descriptor::setHandle(int fd) noexcept {
	closeHandle();
	this->fd = fd;
}

int Descriptor::releaseHandle() noexcept {
	auto ret = fd;
	fd = -1;
	return ret;
}

void Descriptor::closeHandle() noexcept {
	if (fd != -1) {
		::close(fd);
	}
	fd = -1;
}

bool Descriptor::isReady(bool outgoing) const noexcept {
	return (getEvents() && ((getEvents() != IO_WRITE) || outgoing));
}

ssize_t Descriptor::read(const struct iovec *vectors, int count) {
	auto nRead = ::readv(fd, vectors, count);
	if (nRead > 0) {
		return nRead;
	} else if (nRead == 0) {
		//May have encountered EOF
		return count ? -1 : nRead;
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//Would Block, clear the READ flag
		clearEvents(IO_READ);
		return 0;
	} else {
		throw SystemException();
	}
}

ssize_t Descriptor::read(void *buf, size_t count) {
	auto nRead = ::read(fd, buf, count);
	if (nRead > 0) {
		return nRead;
	} else if (nRead == 0) {
		//May have encountered EOF
		return count ? -1 : nRead;
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//Would Block, clear the READ flag
		clearEvents(IO_READ);
		return 0;
	} else {
		throw SystemException();
	}
}

ssize_t Descriptor::write(const struct iovec *vectors, int count) {
	auto nWrite = writev(fd, vectors, count);
	if (nWrite != -1) {
		return nWrite;
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//Would Block, clear WRITE-flag and return immediately
		clearEvents(IO_WRITE);
		return 0;
	} else {
		throw SystemException();
	}
}

ssize_t Descriptor::write(const void *buf, size_t count) {
	auto nWrite = ::write(fd, buf, count);
	if (nWrite != -1) {
		return nWrite;
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//Would Block, clear WRITE-flag and return immediately
		clearEvents(IO_WRITE);
		return 0;
	} else {
		throw SystemException();
	}
}

unsigned long long Descriptor::nextUid() noexcept {
	//For all practical purposes this is sufficient
	return Atomic<unsigned long long>::fetchAndAdd(&_nextUid, 1);
}

} /* namespace wanhive */
