/*
 * Stream.cpp
 *
 * Byte stream watcher
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Stream.h"
#include "Hub.h"

namespace wanhive {

Stream::Stream(int fd) noexcept :
		Watcher { fd } {

}

Stream::~Stream() {

}

size_t Stream::ingest(const unsigned char *src, size_t count) noexcept {
	auto ret = out.write(src, count);
	if (ret) {
		setFlags(WATCHER_OUT);
	}

	return ret;
}

bool Stream::ingest(const unsigned char &src) noexcept {
	if (out.put(src)) {
		setFlags(WATCHER_OUT);
		return true;
	} else {
		return false;
	}
}

size_t Stream::space() const noexcept {
	return out.writeSpace();
}

size_t Stream::emit(unsigned char *dest, size_t count) noexcept {
	return in.read(dest, count);
}

bool Stream::emit(unsigned char &dest) noexcept {
	return in.get(dest);
}

size_t Stream::available() const noexcept {
	return in.readSpace();
}

void Stream::start() {

}

void Stream::stop() noexcept {

}

bool Stream::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Stream> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Stream::publish(void *arg) noexcept {
	return false;
}

ssize_t Stream::read() {
	ssize_t nRecv = 0;
	CircularBufferVector<unsigned char> vector;
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

ssize_t Stream::write() {
	CircularBufferVector<unsigned char> vector;
	if (out.getReadable(vector)) {
		auto count = (vector.part[1].length) ? 2 : 1;
		iovec iovs[2] = { { vector.part[0].base, vector.part[0].length }, {
				vector.part[1].base, vector.part[1].length } };
		auto nSent = Descriptor::writev(iovs, count);
		out.skipRead(nSent);
		return nSent;
	} else {
		clearFlags(WATCHER_OUT);
		return 0;
	}
}

} /* namespace wanhive */
