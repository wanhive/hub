/*
 * SignalSet.cpp
 *
 * Signal set operations
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "SignalSet.h"
#include "SystemException.h"
#include <cstring>

namespace wanhive {

SignalSet::SignalSet() noexcept {
	memset(&_set, 0, sizeof(_set));
}

SignalSet::SignalSet(bool fill) {
	if (fill) {
		this->fill();
	} else {
		this->empty();
	}
}

SignalSet::~SignalSet() {

}

void SignalSet::empty() {
	empty(_set);
}

void SignalSet::fill() {
	fill(_set);
}

void SignalSet::add(int signum) {
	add(_set, signum);
}

void SignalSet::remove(int signum) {
	remove(_set, signum);
}

bool SignalSet::test(int signum) const {
	return test(_set, signum);
}

sigset_t* SignalSet::mask() noexcept {
	return &_set;
}

const sigset_t* SignalSet::mask() const noexcept {
	return &_set;
}

void SignalSet::empty(sigset_t &set) {
	if (::sigemptyset(&set) == 0) {
		return;
	} else {
		throw SystemException();
	}
}

void SignalSet::fill(sigset_t &set) {
	if (::sigfillset(&set) == 0) {
		return;
	} else {
		throw SystemException();
	}
}

void SignalSet::add(sigset_t &set, int signum) {
	if (::sigaddset(&set, signum) == 0) {
		return;
	} else {
		throw SystemException();
	}
}

void SignalSet::remove(sigset_t &set, int signum) {
	if (::sigdelset(&set, signum) == 0) {
		return;
	} else {
		throw SystemException();
	}
}

bool SignalSet::test(const sigset_t &set, int signum) {
	auto status = ::sigismember(&set, signum);
	if (status == 1) {
		return true;
	} else if (status == 0) {
		return false;
	} else {
		throw SystemException();
	}
}

} /* namespace wanhive */
