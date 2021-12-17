/*
 * FileSystem.cpp
 *
 * File-system operations
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "FileSystem.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <libgen.h>

namespace wanhive {

bool FileSystem::access(const char *path, int mode) noexcept {
	if (!path) {
		return false;
	} else if (::access(path, mode) == -1) {
		return false;
	} else {
		return true;
	}
}

bool FileSystem::access(int dirfd, const char *path, int mode,
		int flag) noexcept {
	if (!path) {
		return false;
	} else if (::faccessat(dirfd, path, mode, flag) == -1) {
		return false;
	} else {
		return true;
	}
}

mode_t FileSystem::umask(mode_t mask) noexcept {
	return ::umask(mask);
}

void FileSystem::chmod(const char *path, mode_t mode) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::chmod(path, mode) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::chmod(int fd, mode_t mode) {
	if (::fchmod(fd, mode) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::chmod(int dirfd, const char *path, mode_t mode, int flag) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::fchmodat(dirfd, path, mode, flag) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::chown(const char *path, uid_t owner, gid_t group,
		bool symLink) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		auto ret =
				!symLink ?
						::chown(path, owner, group) :
						::lchown(path, owner, group);
		if (ret == -1) {
			throw SystemException();
		}
	}
}

void FileSystem::chown(int fd, uid_t owner, gid_t group) {
	if (::fchown(fd, owner, group) == -1) {
		throw SystemException();
	}
}

void FileSystem::chown(int dirfd, const char *path, uid_t owner, gid_t group,
		int flag) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (fchownat(dirfd, path, owner, group, flag) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::truncate(const char *path, off_t length) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::truncate(path, length) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::truncate(int fd, off_t length) {
	if (::ftruncate(fd, length) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::link(const char *oldPath, const char *newPath) {
	if (!oldPath || !newPath) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::link(oldPath, newPath) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::link(int oldDirfd, const char *oldPath, int newDirfd,
		const char *newPath, int flag) {
	if (!oldPath || !newPath) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::linkat(oldDirfd, oldPath, newDirfd, newPath, flag) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::unlink(const char *path) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::unlink(path) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::unlink(int dirfd, const char *path, int flag) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::unlinkat(dirfd, path, flag) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::remove(const char *path) {
	if (::remove(path) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::rename(const char *oldPath, const char *newPath) {
	if (::rename(oldPath, newPath) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::rename(int oldDirfd, const char *oldPath, int newDirfd,
		const char *newPath, unsigned int flag) {
#ifdef WH_LINUX
	auto ret = ::renameat2(oldDirfd, oldPath, newDirfd, newPath, flag);
#else
	auto ret = ::renameat(oldDirfd, oldPath, newDirfd, newPath);
#endif
	if (ret == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::symLink(const char *target, const char *linkPath) {
	if (!target || !linkPath) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::symlink(target, linkPath) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::symLink(int dirfd, const char *target, const char *linkPath) {
	if (!target || !linkPath) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::symlinkat(target, dirfd, linkPath) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

size_t FileSystem::readLink(const char *path, char *buf, size_t size) {
	struct stat sb;
	if (!path || !buf || !size) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::lstat(path, &sb) == -1) {
		throw SystemException();
	} else if ((size - 1) < (size_t) sb.st_size) {
		throw Exception(EX_OVERFLOW);
	} else {
		auto ret = ::readlink(path, buf, size);
		if (ret == -1) {
			throw SystemException();
		} else {
			buf[ret] = '\0';
			return ret;
		}
	}
}

size_t FileSystem::readLink(int dirfd, const char *path, char *buf,
		size_t size) {
	struct stat sb;
	if (!path || !buf || !size) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::fstatat(dirfd, path, &sb, AT_SYMLINK_NOFOLLOW) == -1) {
		throw SystemException();
	} else if ((size - 1) < (size_t) sb.st_size) {
		throw Exception(EX_OVERFLOW);
	} else {
		auto ret = ::readlinkat(dirfd, path, buf, size);
		if (ret == -1) {
			throw SystemException();
		} else {
			buf[ret] = '\0';
			return ret;
		}
	}
}

void FileSystem::setTimestamp(int fd, const timespec times[2]) {
	if (::futimens(fd, times) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FileSystem::setTimestamp(int dirfd, const char *path,
		const timespec times[2], int flag) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::utimensat(dirfd, path, times, flag) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

const char* FileSystem::directoryName(char *path) noexcept {
	return ::dirname(path);
}

const char* FileSystem::baseName(char *path) noexcept {
	return ::basename(path);
}

} /* namespace wanhive */
