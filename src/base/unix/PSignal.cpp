/*
 * PSignal.cpp
 *
 * Signal handling for processes
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "PSignal.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <cerrno>
#include <unistd.h>

namespace wanhive {

void PSignal::raise(int signum) {
	raise(::getpid(), signum);
}

void PSignal::raise(pid_t pid, int signum) {
	if (::kill(pid, signum) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

unsigned int PSignal::alarm(unsigned int seconds) noexcept {
	return ::alarm(seconds);
}

void PSignal::pause() noexcept {
	::pause();
}

void PSignal::setMask(int how, const SignalSet &ss) {
	mask(how, ss.mask(), nullptr);
}

void PSignal::setMask(int how, const SignalSet &ss, SignalSet &old) {
	mask(how, ss.mask(), old.mask());
}

void PSignal::getMask(SignalSet &ss) {
	mask(0, nullptr, ss.mask());
}

void PSignal::mask(int how, const sigset_t *set, sigset_t *old) {
	if (::sigprocmask(how, set, old) == -1) {
		throw SystemException();
	}
}

void PSignal::pending(SignalSet &ss) {
	pending(ss.mask());
}

void PSignal::pending(sigset_t *set) {
	if (!set) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::sigpending(set) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void PSignal::wait(const SignalSet &ss) {
	wait(ss.mask());
}

void PSignal::wait(const sigset_t *set) {
	if (!set) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::sigsuspend(set) == -1 && errno != EINTR) {
		throw SystemException();
	} else {
		return;
	}
}

void PSignal::queue(pid_t pid, int signum, const sigval value) {
	if (::sigqueue(pid, signum, value) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

} /* namespace wanhive */
