/*
 * Interrupt.cpp
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
#include <sys/signalfd.h>
#include <signal.h>

namespace wanhive {

Interrupt::Interrupt(bool blocking) {
	create(blocking);
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

ssize_t Interrupt::read(int &signum) {
	signum = 0;
	signalfd_siginfo info;
	auto nRead = Descriptor::read(&info, sizeof(info));
	if (nRead == sizeof(info)) {
		signum = info.ssi_signo;
		return nRead;
	} else if (nRead == 0 || nRead == -1) {
		return nRead;
	} else {
		//Something broke
		throw Exception(EX_STATE);
	}
}

void Interrupt::create(bool blocking) {
	int fd;
	sigset_t mask;
	if (sigfillset(&mask) == -1) {
		throw SystemException();
	} else if ((fd = signalfd(-1, &mask, blocking ? 0 : SFD_NONBLOCK)) == -1) {
		throw SystemException();
	} else {
		Descriptor::set(fd);
	}
}

} /* namespace wanhive */
