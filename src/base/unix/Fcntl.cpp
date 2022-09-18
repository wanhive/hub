/*
 * Fcntl.cpp
 *
 * File descriptor operations
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Fcntl.h"
#include "SystemException.h"
#include <cerrno>

namespace wanhive {

int Fcntl::duplicate(int fd, bool closeOnExec) {
	int cmd = closeOnExec ? F_DUPFD_CLOEXEC : F_DUPFD;
	auto ret = ::fcntl(fd, cmd);
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

int Fcntl::getDescriptorFlag(int fd) {
	auto ret = ::fcntl(fd, F_GETFD);

	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

void Fcntl::setDescriptorFlag(int fd, int flag) {
	auto ret = ::fcntl(fd, F_SETFD, flag);

	if (ret == -1) {
		throw SystemException();
	} else {
		return;
	}
}

int Fcntl::getStatusFlag(int fd) {
	auto ret = ::fcntl(fd, F_GETFL);

	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

void Fcntl::setStatusFlag(int fd, int flag) {
	auto ret = ::fcntl(fd, F_SETFL, flag);

	if (ret == -1) {
		throw SystemException();
	} else {
		return;
	}
}

bool Fcntl::setLock(int fd, struct flock &type, bool block) {
	int cmd = block ? F_SETLKW : F_SETLK;

	auto ret = ::fcntl(fd, cmd, &type);

	if (ret == 0) {
		return true;
	} else if (errno == EACCES || errno == EAGAIN || errno == EINTR) {
		return false;
	} else {
		throw SystemException();
	}
}

void Fcntl::testLock(int fd, struct flock &type) {
	auto ret = ::fcntl(fd, F_GETLK, &type);

	if (ret == 0) {
		return;
	} else {
		throw SystemException();
	}
}

} /* namespace wanhive */
