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
#include "../base/common/Atomic.h"
#include "../base/unix/Fcntl.h"
#include "../base/unix/SystemException.h"
#include <cerrno>

namespace wanhive {

Descriptor::Descriptor() noexcept {

}

Descriptor::Descriptor(int fd) noexcept :
		File(fd) {

}

Descriptor::~Descriptor() {

}

unsigned long long Descriptor::getUid() const noexcept {
	return uid.get();
}

void Descriptor::setUid(unsigned long long uid) noexcept {
	this->uid.set(uid);
}

bool Descriptor::hasTimedOut(unsigned int timeOut) const noexcept {
	return timer.hasTimedOut(timeOut);
}

int Descriptor::getHandle() const noexcept {
	return File::get();
}

void Descriptor::setHandle(int fd) noexcept {
	File::set(fd);
}

int Descriptor::releaseHandle() noexcept {
	return File::release();
}

bool Descriptor::closeHandle() noexcept {
	return File::close();
}

bool Descriptor::isReady(bool outgoing) const noexcept {
	return (getEvents() && ((getEvents() != IO_WRITE) || outgoing));
}

bool Descriptor::isBlocking() {
	auto ret = Fcntl::getStatusFlag(getHandle());
	return !(ret & O_NONBLOCK);
}

void Descriptor::setBlocking(bool block) {
	auto flags = Fcntl::getStatusFlag(getHandle());
	if (block) {
		flags &= ~O_NONBLOCK;
	} else {
		flags |= O_NONBLOCK;
	}

	Fcntl::setStatusFlag(getHandle(), flags);
}

ssize_t Descriptor::readv(const iovec *vectors, unsigned int count) {
	auto nRead = ::readv(getHandle(), vectors, count);
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
	auto nRead = ::read(getHandle(), buf, count);
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

ssize_t Descriptor::writev(const iovec *vectors, unsigned int count) {
	auto nWrite = ::writev(getHandle(), vectors, count);
	if (nWrite != -1) {
		return nWrite;
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//Would Block, clear the WRITE flag and return immediately
		clearEvents(IO_WRITE);
		return 0;
	} else {
		throw SystemException();
	}
}

ssize_t Descriptor::write(const void *buf, size_t count) {
	auto nWrite = ::write(getHandle(), buf, count);
	if (nWrite != -1) {
		return nWrite;
	} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//Would Block, clear the WRITE flag and return immediately
		clearEvents(IO_WRITE);
		return 0;
	} else {
		throw SystemException();
	}
}

} /* namespace wanhive */
