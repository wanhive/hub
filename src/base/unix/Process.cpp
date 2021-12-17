/*
 * Process.cpp
 *
 * Process management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Process.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

namespace wanhive {

bool Process::atExit(void (*function)(void)) noexcept {
	if (function) {
		return ::atexit(function) == 0;
	} else {
		return false;
	}
}

pid_t Process::self() noexcept {
	return ::getpid();
}

pid_t Process::parent() noexcept {
	return ::getppid();
}

pid_t Process::fork() {
	auto pid = ::fork();
	if (pid == -1) {
		throw SystemException();
	} else {
		return pid;
	}
}

pid_t Process::wait(pid_t pid, int options, int *status) noexcept {
	return ::waitpid(pid, status, options);
}

void Process::execute(const char *name, char *const argv[], bool search) {
	if (!name || !argv) {
		throw Exception(EX_INVALIDPARAM);
	} else if (search) {
		::execvp(name, argv);
		throw SystemException();
	} else {
		::execv(name, argv);
		throw SystemException();
	}
}

void Process::execute(const char *name, char *const argv[], char *const envp[],
		bool search) {
	if (!name || !argv) {
		throw Exception(EX_INVALIDPARAM);
	} else if (search) {
		::execvpe(name, argv, envp);
		throw SystemException();
	} else {
		::execve(name, argv, envp);
		throw SystemException();
	}
}

void Process::execute(int fd, char *const argv[], char *const envp[]) {
	if (!argv) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		::fexecve(fd, argv, envp);
		throw SystemException();
	}
}

clock_t Process::time(tms *buf) {
	auto ret = ::times(buf);
	if (ret == (clock_t) -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

void Process::getLimit(int resource, rlimit *limit) {
	if (::getrlimit(resource, limit) != 0) {
		throw SystemException();
	} else {
		return;
	}
}

void Process::setLimit(int resource, const rlimit *limit) {
	if (::setrlimit(resource, limit) != 0) {
		throw SystemException();
	} else {
		return;
	}
}

int Process::getPriority(int which, id_t id) {
	errno = 0;
	auto ret = ::getpriority(which, id);
	if (ret == -1 && errno != 0) {
		throw SystemException();
	} else {
		return ret;
	}
}

void Process::setPriority(int which, id_t id, int value) {
	if (::setpriority(which, id, value) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

} /* namespace wanhive */
