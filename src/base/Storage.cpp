/*
 * Storage.cpp
 *
 * Common file system routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Storage.h"
#include "common/Exception.h"
#include "common/Memory.h"
#include "ds/Twiddler.h"
#include "unix/SystemException.h"
#include <cerrno>
#include <climits>
#include <cstring>
#include <ftw.h>
#include <unistd.h>
#include <wordexp.h>

namespace {
//Helper function for removeDirectory
int rmHelper(const char *path, const struct stat *s, int flag,
		struct FTW *f) noexcept {
	return remove(path);
}

}  // namespace

namespace wanhive {

FILE* Storage::open(const char *path, const char *modes) noexcept {
	auto createPath = false;
	for (auto x = modes; x && *x; ++x) {
		auto c = *x;
		if (c == 'w' || c == 'a') {
			createPath = true;
			break;
		}
	}

	FILE *f = nullptr;
	while ((f = fopen(path, modes)) == nullptr) {
		if (errno == ENOENT && createPath && createDirectoryForFile(path)) {
			continue;
		} else {
			break;
		}
	}
	return f;
}

int Storage::close(FILE *fp) noexcept {
	return fclose(fp);
}

int Storage::open(const char *path, int flags, mode_t mode) {
	if (path && path[0]) {
		int fd = -1;
		auto createPath = static_cast<bool>(flags & O_CREAT);
		while ((fd = ::open(path, flags, mode)) == -1) {
			if (errno == ENOENT && createPath && createDirectoryForFile(path)) {
				continue;
			} else {
				throw SystemException();
			}
		}
		return fd;
	} else {
		throw Exception(EX_ARGUMENT);
	}
}

int Storage::close(int fd) noexcept {
	return ::close(fd);
}

size_t Storage::read(int fd, void *buffer, size_t count, bool strict) {
	auto toRead = count;
	size_t index = 0;
	ssize_t n = 0;
	while (toRead != 0) {
		if ((n = ::read(fd, (unsigned char*) buffer + index, count)) == -1) {
			throw SystemException();
		} else if (n == 0) {
			//We encountered EOF in the middle of the operation
			if (strict && count) {
				throw Exception(EX_RESOURCE);
			} else {
				break;
			}
		} else {
			toRead -= n;
			index += n;
		}
	}
	return (count - toRead);
}

size_t Storage::write(int fd, const void *buffer, size_t count) {
	auto toWrite = count;
	size_t index = 0;
	ssize_t n = 0;
	while (toWrite != 0) {
		if ((n = ::write(fd, (unsigned char*) buffer + index, count)) == -1) {
			throw SystemException();
		} else {
			toWrite -= n;
			index += n;
		}
	}
	return (count - toWrite);
}

void Storage::sync(int fd) {
	if (::fsync(fd) != 0) {
		throw SystemException();
	}
}

off_t Storage::seek(int fd, off_t offset, int whence) {
	auto ret = ::lseek(fd, offset, whence);
	if (ret != (off_t) -1) {
		return ret;
	} else {
		throw SystemException();
	}
}

void Storage::truncate(int fd, off_t length) {
	if (::ftruncate(fd, length) == -1) {
		throw SystemException();
	}
}

void Storage::fill(int fd, size_t size, unsigned char c) {
	unsigned char buffer[4096];
	auto bytesleft = size;

	memset(buffer, c, sizeof(buffer));
	seek(fd, 0, SEEK_SET);

	while (bytesleft) {
		auto toWrite =
				(bytesleft > sizeof(buffer)) ? sizeof(buffer) : bytesleft;
		bytesleft -= write(fd, buffer, toWrite);
	}
}

void Storage::createDirectory(const char *path) {
	if (path && path[0]) {
		char tmp[PATH_MAX];
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, path, (sizeof(tmp) - 1));
		if (!_createDirectory(tmp)) {
			throw SystemException();
		}
	} else {
		throw Exception(EX_NULL);
	}
}

void Storage::removeDirectory(const char *path) {
	// Traverse depth-first, do not follow symbolic links
	if (path && nftw(path, rmHelper, 64, FTW_DEPTH | FTW_PHYS) == -1) {
		throw SystemException();
	}
}

int Storage::testDirectory(const char *path) noexcept {
	struct stat data;
	if (!path) {
		return -1;
	} else if (stat(path, &data) == 0) {
		return S_ISDIR(data.st_mode) ? 1 : 0;
	} else {
		return -1;
	}
}

int Storage::testFile(const char *path) noexcept {
	struct stat data;
	if (!path) {
		return -1;
	} else if (stat(path, &data) == 0) {
		return S_ISREG(data.st_mode) ? 1 : 0;
	} else {
		return -1;
	}
}

int Storage::testLink(const char *path) noexcept {
	struct stat linkstat;
	if (!path) {
		return -1;
	} else if (lstat(path, &linkstat) == 0) {
		return S_ISLNK(linkstat.st_mode) ? 1 : 0;
	} else {
		return -1;
	}
}

char* Storage::expand(const char *path) noexcept {
	if (path) {
		char *buffer = nullptr;
		wordexp_t p;
		memset(&p, 0, sizeof(p));
		if (wordexp(path, &p, WRDE_NOCMD) == 0) {
			char **w = p.we_wordv;
			//Take the first one
			if (p.we_wordc) {
				buffer = strdup(w[0]);
			}
		}
		wordfree(&p);
		return buffer;
	} else {
		return nullptr;
	}
}

bool Storage::createDirectoryForFile(const char *path) noexcept {
	if (path && path[0]) {
		char tmp[PATH_MAX];
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, path, (sizeof(tmp) - 1));
		Twiddler::stripLast(tmp, PATH_SEPARATOR);
		return _createDirectory(tmp);
	} else {
		return false;
	}
}

bool Storage::_createDirectory(char *path) noexcept {
	if (!path || !path[0]) {
		return false;
	}

	auto mark = path;
	for (; *mark == PATH_SEPARATOR; ++mark) {
		//skip heading '/'s
	}

	for (; *mark; ++mark) {
		mark = strchr(mark, PATH_SEPARATOR);

		if (mark) {		//Move into the tree one level at a time
			*mark = 0;
		}
		//-----------------------------------------------------------------
		if (access(path, F_OK) == -1) {
			//Directory doesn't exist, try to create one
			constexpr mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH
					| S_IXOTH;
			if (mkdir(path, mode) == -1) {
				return false;
			}
		} else if (testDirectory(path) != 1) {
			return false; //Not a directory
		}
		//-----------------------------------------------------------------
		if (mark) {
			*mark = PATH_SEPARATOR;
		} else {		//No more levels left
			break;
		}
	}
	return true;
}

} /* namespace wanhive */
