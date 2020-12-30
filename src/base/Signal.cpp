/*
 * Signal.cpp
 *
 * Signal handling on Linux
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Signal.h"
#include "SystemException.h"
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace wanhive {

void Signal::block(int signum) {
	sigset_t ss;
	empty(&ss);
	add(&ss, signum);
	threadMask(SIG_BLOCK, &ss, nullptr);
}

void Signal::unblock(int signum) {
	sigset_t ss;
	empty(&ss);
	add(&ss, signum);
	threadMask(SIG_UNBLOCK, &ss, nullptr);
}

void Signal::blockAll() {
	sigset_t ss;
	fill(&ss);
	setMask(&ss);
}

void Signal::unblockAll() {
	sigset_t ss;
	empty(&ss);
	setMask(&ss);
}

void Signal::ignore(int signum) {
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = SA_RESTART;
	setAction(signum, &sa, nullptr);
}

void Signal::handle(int signum, void (*handler)(int), bool restart) {
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler ? handler : dummyHandler;
	sa.sa_flags |= (restart ? SA_RESTART : 0);
	setAction(signum, &sa, nullptr);
}

void Signal::raise(int signum) {
	int error = kill(getpid(), signum);
	if (error) {
		throw SystemException();
	}
}

void Signal::pause() noexcept {
	::pause();
}

bool Signal::timedWait(unsigned int milliseconds) {
	struct timespec tv;
	tv.tv_sec = milliseconds / 1000;
	tv.tv_nsec = (milliseconds % 1000) * 1000000L;

	sigset_t ss;
	fill(&ss);
	if (sigtimedwait(&ss, nullptr, &tv) != -1) {
		return true;
	} else if (errno == EAGAIN) {
		return false;
	} else if (errno == EINTR) {
		return true;
	} else {
		throw SystemException();
	}
}

bool Signal::timedwait(unsigned int milliseconds, int signum) {
	struct timespec tv;
	tv.tv_sec = milliseconds / 1000;
	tv.tv_nsec = (milliseconds % 1000) * 1000000L;

	sigset_t ss;
	empty(&ss);
	add(&ss, signum);
	if (sigtimedwait(&ss, nullptr, &tv) != -1) {
		return true;
	} else if (errno == EAGAIN) {
		return false;
	} else {
		throw SystemException();
	}
}

void Signal::wait() {
	sigset_t ss;
	fill(&ss);
	if (sigwaitinfo(&ss, nullptr) != -1) {
		return;
	} else if (errno == EINTR) {
		return;
	} else {
		throw SystemException();
	}
}

void Signal::wait(int signum) {
	sigset_t ss;
	empty(&ss);
	add(&ss, signum);
	if (sigwaitinfo(&ss, nullptr) == -1) {
		throw SystemException();
	}
}

int Signal::openSignalfd(int fd, const sigset_t &mask, bool blocking) {
	fd = signalfd(fd, &mask, blocking ? 0 : SFD_NONBLOCK);
	if (fd != -1) {
		return fd;
	} else {
		throw SystemException();
	}
}

int Signal::closeSignalfd(int fd) noexcept {
	return close(fd);
}

void Signal::dummyHandler(int signum) noexcept {
}

void Signal::sigchildHandler(int signum) noexcept {
	while (waitpid(-1, nullptr, WNOHANG) > 0) {
		//Clean up all ther zombies
	}
}

void Signal::empty(sigset_t *set) {
	if (sigemptyset(set) == -1) {
		throw SystemException();
	}
}

void Signal::fill(sigset_t *set) {
	if (sigfillset(set) == -1) {
		throw SystemException();
	}
}

void Signal::add(sigset_t *set, int signum) {
	if (sigaddset(set, signum) == -1) {
		throw SystemException();
	}
}

void Signal::remove(sigset_t *set, int signum) {
	if (sigdelset(set, signum) == -1) {
		throw SystemException();
	}
}

bool Signal::isMember(const sigset_t *set, int signum) {
	int ret = sigismember(set, signum);
	if (ret != -1) {
		return (bool) ret;
	} else {
		throw SystemException();
	}
}

void Signal::setMask(sigset_t *set) {
	threadMask(SIG_SETMASK, set, nullptr);
}

void Signal::threadMask(int how, const sigset_t *set, sigset_t *oldset) {
	int error = pthread_sigmask(how, set, oldset);
	if (error) {
		throw SystemException(error);
	}
}

void Signal::setAction(int signum, const struct sigaction *act,
		struct sigaction *oldact) {
	if (::sigaction(signum, act, oldact)) {
		throw SystemException();
	}
}

} /* namespace wanhive */
