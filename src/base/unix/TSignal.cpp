/*
 * TSignal.cpp
 *
 * Signal handling for posix threads
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "TSignal.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <cerrno>

namespace wanhive {

void TSignal::setMask(int how, const SignalSet &ss) {
	mask(how, ss.mask(), nullptr);
}
void TSignal::setMask(int how, const SignalSet &ss, SignalSet &old) {
	mask(how, ss.mask(), old.mask());
}

void TSignal::getMask(SignalSet &ss) {
	mask(0, nullptr, ss.mask());
}

void TSignal::mask(int how, const sigset_t *set, sigset_t *old) {
	auto rc = ::pthread_sigmask(how, set, old);
	if (rc == 0) {
		return;
	} else {
		throw SystemException(rc);
	}
}

void TSignal::raise(int signum) {
	raise(::pthread_self(), signum);
}

void TSignal::raise(pthread_t threadId, int signum) {
	auto rc = ::pthread_kill(threadId, signum);
	if (rc == 0) {
		return;
	} else {
		throw SystemException(rc);
	}
}

int TSignal::suspend(const SignalSet &ss) {
	return suspend(ss.mask());
}

int TSignal::suspend(const sigset_t *set) {
	if (!set) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		int signum = 0;
		auto rc = ::sigwait(set, &signum);
		if (rc == 0) {
			return signum;
		} else {
			throw SystemException(rc);
		}
	}
}

void TSignal::queue(pthread_t threadId, int signum, const sigval value) {
	auto rc = ::pthread_sigqueue(threadId, signum, value);
	if (rc == 0) {
		return;
	} else {
		throw SystemException(rc);
	}
}

} /* namespace wanhive */
