/*
 * Directory.cpp
 *
 * Directory management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Directory.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <cerrno>
#include <unistd.h>

namespace wanhive {

Directory::Directory() noexcept :
		dir(nullptr) {

}

Directory::Directory(const char *name) :
		dir(nullptr) {
	open(name);
}

Directory::Directory(int fd) :
		dir(nullptr) {
	open(fd);
}

Directory::~Directory() {
	close();
}

dirent* Directory::read() {
	if (dir) {
		errno = 0;
		auto info = ::readdir(dir);
		if (info) {
			return info;
		} else {
			throw SystemException();
		}
	} else {
		throw Exception(EX_INVALIDOPERATION);
	}
}

void Directory::rewind() {
	if (dir) {
		::rewinddir(dir);
	} else {
		throw Exception(EX_INVALIDOPERATION);
	}
}

long Directory::tell() {
	long loc = 0;
	if (!dir) {
		throw Exception(EX_INVALIDOPERATION);
	} else if ((loc = ::telldir(dir)) == -1) {
		throw SystemException();
	} else {
		return loc;
	}
}

void Directory::seek(long loc) {
	if (dir) {
		::seekdir(dir, loc);
	} else {
		throw Exception(EX_INVALIDOPERATION);
	}
}

int Directory::getDescriptor() {
	int fd = 0;
	if (!dir) {
		throw Exception(EX_INVALIDOPERATION);
	} else if ((fd = ::dirfd(dir)) == -1) {
		throw SystemException();
	} else {
		return fd;
	}
}

void Directory::open(const char *name) {
	DIR *newDir = nullptr;
	if (!name) {
		throw Exception(EX_INVALIDPARAM);
	} else if ((newDir = ::opendir(name)) == nullptr) {
		throw SystemException();
	} else {
		close();
		dir = newDir;
	}
}

void Directory::open(int fd) {
	DIR *newDir = nullptr;
	if ((newDir = ::fdopendir(fd)) == nullptr) {
		throw SystemException();
	} else {
		close();
		dir = newDir;
	}
}

bool Directory::close() noexcept {
	int status = 0;
	if (dir) {
		status = ::closedir(dir);
	}

	dir = nullptr;
	return (status == 0);
}

dirent** Directory::scan(const char *name, int (*filter)(const dirent*),
		int (*cmp)(const dirent**, const dirent**), int &count) {
	dirent **list = nullptr;
	if (!name) {
		throw Exception(EX_INVALIDPARAM);
	} else if ((count = ::scandir(name, &list, filter, cmp)) == -1) {
		throw SystemException();
	} else {
		return list;
	}
}

dirent** Directory::scan(int fd, const char *name, int (*filter)(const dirent*),
		int (*cmp)(const dirent**, const dirent**), int &count) {
	dirent **list = nullptr;
	if (!name) {
		throw Exception(EX_INVALIDPARAM);
	} else if ((count = ::scandirat(fd, name, &list, filter, cmp)) == -1) {
		throw SystemException();
	} else {
		return list;
	}
}

int Directory::walk(const char *dirpath,
		int (*fn)(const char*, const struct stat*, int, FTW*), int descriptors,
		int flag) {
	return ::nftw(dirpath, fn, descriptors, flag);
}

void Directory::create(const char *path, mode_t mode) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::mkdir(path, mode) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void Directory::create(int dirfd, const char *path, mode_t mode) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::mkdirat(dirfd, path, mode) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void Directory::remove(const char *path) {
	if (!path) {
		throw Exception(EX_INVALIDPARAM);
	} else if (::rmdir(path) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

} /* namespace wanhive */
