/*
 * SignalWatcher.cpp
 *
 * Signal watcher
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Interrupt.h"
#include "Hub.h"
#include "../base/unix/SystemException.h"

namespace wanhive {

Interrupt::Interrupt(bool blocking) {
	create(blocking);
	memset(&info, 0, sizeof(info));
}

Interrupt::~Interrupt() {

}

void Interrupt::start() {

}

void Interrupt::stop() noexcept {

}

bool Interrupt::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Interrupt> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Interrupt::publish(void *arg) noexcept {
	return false;
}

ssize_t Interrupt::read() {
	memset(&info, 0, sizeof(info)); //Clear out the data
	auto nRead = Descriptor::read(&info, sizeof(info));
	if (nRead == sizeof(info)) {
		return nRead;
	} else if (nRead == 0 || nRead == -1) {
		return nRead;
	} else {
		//Something broke
		throw Exception(EX_INVALIDSTATE);
	}
}

const SignalInfo* Interrupt::getInfo() const noexcept {
	return &info;
}

void Interrupt::create(bool blocking) {
	int fd;
	sigset_t mask;
	if (sigfillset(&mask) == -1) {
		throw SystemException();
	} else if ((fd = signalfd(-1, &mask, blocking ? 0 : SFD_NONBLOCK)) == -1) {
		throw SystemException();
	} else {
		setHandle(fd);
	}
}

} /* namespace wanhive */
