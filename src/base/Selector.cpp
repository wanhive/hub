/*
 * Selector.cpp
 *
 * IO multiplexer
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Selector.h"
#include "unix/SystemException.h"
#include <cerrno>
#include <unistd.h>

namespace wanhive {

Selector::Selector() noexcept {

}

Selector::Selector(unsigned int maxEvents, bool signal) {
	initialize(maxEvents, signal);
}

Selector::~Selector() {
	close();
}

void Selector::initialize(unsigned int maxEvents, bool signal) {
	//Strictly maintain the sequence to prevent resource leak
	close();
	selected.initialize(maxEvents);
	selected.rewind(); //Nothing to read yet
	mask = (signal ? &signals : nullptr);
	create();
}

void Selector::add(int fd, uint32_t events, void *handle) {
	epoll_event event;
	event.events = events;
	event.data.ptr = handle;

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event)) {
		throw SystemException();
	}
}

void Selector::modify(int fd, uint32_t events, void *handle) {
	epoll_event event;
	event.events = events;
	event.data.ptr = handle;

	if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event)) {
		throw SystemException();
	}
}

void Selector::remove(int fd) {
	epoll_event event;
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event)) {
		throw SystemException();
	}
}

unsigned int Selector::select(int timeout) {
	_interrupted = false;
	_timedOut = false;
	selected.clear();
	auto n = epoll_pwait(epfd, selected.offset(), selected.space(), timeout,
			mask);
	if (n > 0) {
		selected.setIndex(selected.getIndex() + n);
	}
	//Rewind the buffer, for reading
	selected.rewind();

	if (n > 0) {
		return n;
	} else if (n == 0) {
		//Timed out
		_timedOut = true;
		return n;
	} else if (errno == EINTR) {
		//Received a signal
		_interrupted = true;
		return 0;
	} else {
		//Error
		throw SystemException();
	}
}

bool Selector::interrupted() const noexcept {
	return _interrupted;
}

bool Selector::timedOut() const noexcept {
	return _timedOut;
}

const SelectionEvent* Selector::next() noexcept {
	return selected.get();
}

void* Selector::attachment(const SelectionEvent *se) noexcept {
	return se->data.ptr;
}

uint32_t Selector::events(const SelectionEvent *se) noexcept {
	return se->events;
}

void Selector::create() {
	//Maintain the order to prevent resource (descriptor) leak
	if (sigemptyset(&signals) == -1) {
		throw SystemException();
	} else if ((epfd = epoll_create1(0)) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

int Selector::close() noexcept {
	int ret = 0;
	if (epfd != -1) {
		ret = ::close(epfd);
	}
	mask = nullptr;
	epfd = -1;
	_interrupted = false;
	_timedOut = false;
	return ret;
}

} /* namespace wanhive */
