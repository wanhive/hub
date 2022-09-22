/*
 * File.cpp
 *
 * File descriptor management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "File.h"
#include "SystemException.h"
#include "../common/defines.h"
#include "../common/Exception.h"
#include <cerrno>
#include <unistd.h>
#include <sys/file.h>

#undef WH_DUP2
#if defined(WH_LINUX)
#define WH_DUP2(oldfd, newfd, flags) ::dup3(oldfd, newfd, flags)
#else
#define WH_DUP2(oldfd, newfd, flags) ::dup2(oldfd, newfd)
#endif

namespace wanhive {

File::File() noexcept :
		fd(-1) {

}

File::File(int fd) noexcept :
		fd(fd) {

}

File::File(const char *path, int flags) :
		fd(-1) {
	open(path, flags);
}

File::File(const char *path, int flags, mode_t mode) :
		fd(-1) {
	open(path, flags, mode);
}

File::File(int dirfd, const char *path, int flags) :
		fd(-1) {
	open(dirfd, path, flags);
}

File::File(int dirfd, const char *path, int flags, mode_t mode) :
		fd(-1) {
	open(dirfd, path, flags, mode);
}

File::~File() {
	close();
}

int File::get() const noexcept {
	return fd;
}

void File::set(int fd) noexcept {
	if (this->fd != fd) {
		close();
		this->fd = fd;
	}
}

int File::release() noexcept {
	auto ret = fd;
	fd = -1;
	return ret;
}

bool File::close() noexcept {
	if (fd >= 0) {
		auto status = ::close(fd);
		fd = -1;
		return (status == 0);
	} else {
		return true;
	}
}

void File::open(const char *path, int flags) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		auto newFd = ::open(path, flags);
		if (newFd == -1) {
			throw SystemException();
		} else {
			set(newFd);
		}
	}
}

void File::open(const char *path, int flags, mode_t mode) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		auto newFd = ::open(path, flags, mode);
		if (newFd == -1) {
			throw SystemException();
		} else {
			set(newFd);
		}
	}
}

void File::open(int dirfd, const char *path, int flags) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		auto newFd = ::openat(dirfd, path, flags);
		if (newFd == -1) {
			throw SystemException();
		} else {
			set(newFd);
		}
	}
}

void File::open(int dirfd, const char *path, int flags, mode_t mode) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		auto newFd = ::openat(dirfd, path, flags, mode);
		if (newFd == -1) {
			throw SystemException();
		} else {
			set(newFd);
		}
	}
}

bool File::lock(bool shared, bool block) {
	int op = shared ? LOCK_SH : LOCK_EX;
	op |= block ? 0 : LOCK_NB;
	if (::flock(get(), op) == 0) {
		return true;
	} else if (errno == EWOULDBLOCK) {
		return false;
	} else {
		throw SystemException();
	}
}

void File::unlock() {
	if (::flock(get(), LOCK_UN) == 0) {
		return;
	} else {
		throw SystemException();
	}
}

int File::duplicate() {
	auto ret = ::dup(get());
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

int File::duplicate(int newfd, int flags) {
	auto ret = WH_DUP2(get(), newfd, flags);
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

} /* namespace wanhive */
