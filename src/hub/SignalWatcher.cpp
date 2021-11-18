/*
 * SignalWatcher.cpp
 *
 * Signal handler for Wanhive hubs
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "SignalWatcher.h"
#include "Hub.h"
#include "../base/SystemException.h"

namespace wanhive {

SignalWatcher::SignalWatcher(bool blocking) {
	create(blocking);
	memset(&info, 0, sizeof(info));
}

SignalWatcher::~SignalWatcher() {

}

void SignalWatcher::start() {

}

void SignalWatcher::stop() noexcept {

}

bool SignalWatcher::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<SignalWatcher> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool SignalWatcher::publish(void *arg) noexcept {
	return false;
}

ssize_t SignalWatcher::read() {
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

const SignalInfo* SignalWatcher::getInfo() const noexcept {
	return &info;
}

void SignalWatcher::create(bool blocking) {
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
