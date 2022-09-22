/*
 * PSession.cpp
 *
 * Job control: session management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "PSession.h"
#include "SystemException.h"
#include <termios.h>
#include <unistd.h>

namespace wanhive {

const char *PSession::TERMINAL = "/dev/tty";

pid_t PSession::create() {
	auto ret = ::setsid();
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

pid_t PSession::get() noexcept {
	return ::getsid(0);
}

pid_t PSession::get(pid_t pid) {
	auto ret = ::getsid(pid);
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

pid_t PSession::getForegroundGroup(int fd) {
	auto ret = ::tcgetpgrp(fd);
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

void PSession::setForegroundGroup(int fd, pid_t groupId) {
	auto ret = ::tcsetpgrp(fd, groupId);
	if (ret == -1) {
		throw SystemException();
	} else {
		return;
	}
}

pid_t PSession::getLeader(int fd) {
	auto ret = tcgetsid(fd);
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

} /* namespace wanhive */
