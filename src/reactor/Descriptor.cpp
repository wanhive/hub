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
#include <cstdio>

namespace wanhive {

Descriptor::Descriptor() noexcept {

}

Descriptor::Descriptor(int fd) noexcept :
		File { fd } {

}

Descriptor::~Descriptor() {

}

unsigned long long Descriptor::getUid() const noexcept {
	return uid.get();
}

void Descriptor::setUid(unsigned long long uid) noexcept {
	this->uid.set(uid);
}

bool Descriptor::hasTimedOut(unsigned int timeout) const noexcept {
	return timer.hasTimedOut(timeout);
}

bool Descriptor::isReady(bool outgoing) const noexcept {
	return (getEvents() && ((getEvents() != IO_WRITE) || outgoing));
}

bool Descriptor::isBlocking() {
	auto ret = Fcntl::getStatusFlag(File::get());
	return !(ret & O_NONBLOCK);
}

void Descriptor::setBlocking(bool block) {
	auto flags = Fcntl::getStatusFlag(File::get());
	if (block) {
		flags &= ~O_NONBLOCK;
	} else {
		flags |= O_NONBLOCK;
	}

	Fcntl::setStatusFlag(File::get(), flags);
}

ssize_t Descriptor::readv(const iovec *vectors, unsigned int count) {
	auto nRead = ::readv(File::get(), vectors, count);
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

ssize_t Descriptor::read(void *buffer, size_t count) {
	auto nRead = ::read(File::get(), buffer, count);
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
	auto nWrite = ::writev(File::get(), vectors, count);
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

ssize_t Descriptor::write(const void *buffer, size_t count) {
	auto nWrite = ::write(File::get(), buffer, count);
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
