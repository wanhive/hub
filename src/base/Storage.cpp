/*
 * Storage.cpp
 *
 * File and directory management on Linux
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
#include <cstring>
#include <ftw.h>
#include <libgen.h>
#include <unistd.h>
#include <wordexp.h>
#include <sys/file.h>

namespace {
//Helper function for removeDirectory
int rmHelper(const char *path, const struct stat *s, int flag,
		struct FTW *f) noexcept {
	return remove(path);
}

}  // namespace

namespace wanhive {
/*
 * Linux-specific settings
 */
const char Storage::DIR_SEPARATOR = '/';
const char *Storage::DIR_SEPARATOR_STR = "/";
const char Storage::PATH_SEPARATOR = ':';
const char *Storage::PATH_SEPARATOR_STR = ":";
const char *Storage::NEWLINE = "\n";

FILE* Storage::openStream(const char *path, const char *modes,
		bool createPath) noexcept {
	auto cf = false;
	for (auto x = modes; x && *x; ++x) {
		auto c = *x;
		if (c == 'w' || c == 'a') {
			cf = true;
			break;
		}
	}

	createPath &= cf; //Adjust
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

int Storage::closeStream(FILE *fp) noexcept {
	return fclose(fp);
}

int Storage::getDescriptor(FILE *stream) {
	auto result = fileno(stream);
	if (result != -1) {
		return result;
	} else {
		throw SystemException();
	}
}

int Storage::open(const char *path, int flags, mode_t mode, bool createPath) {
	if (path && path[0]) {
		int fd = -1;
		createPath &= (flags & O_CREAT); //Adjust
		while ((fd = ::open(path, flags, mode)) == -1) {
			if (errno == ENOENT && createPath && createDirectoryForFile(path)) {
				continue;
			} else {
				throw SystemException();
			}
		}
		return fd;
	} else {
		throw Exception(EX_INVALIDPARAM);
	}
}

int Storage::close(int fd) noexcept {
	return ::close(fd);
}

FILE* Storage::getStream(int fd, const char *modes) {
	auto fp = fdopen(fd, modes);
	if (fp) {
		return fp;
	} else {
		throw SystemException();
	}
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
	if (!fsync(fd)) {
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

ssize_t Storage::readLink(const char *pathname, char *buf, size_t len) {
	if (!pathname || !buf) {
		throw Exception(EX_INVALIDPARAM);
	}

	//Read at-most len-1 bytes (readlink doesn't append nul terminator)
	auto ret = readlink(pathname, buf, len - 1);
	if (ret != -1) {
		buf[ret] = '\0';
		return ret;
	} else {
		throw SystemException();
	}
}

bool Storage::lock(int fd, bool shared, bool block) {
	auto flag = block ? 0 : LOCK_NB;
	flag |= shared ? LOCK_SH : LOCK_EX;
	if (flock(fd, flag) == 0) {
		return true;
	} else if (errno == EWOULDBLOCK) {
		return false;
	} else {
		throw SystemException();
	}
}

bool Storage::unlock(int fd) {
	if (flock(fd, LOCK_UN) == 0) {
		return true;
	} else {
		throw SystemException();
	}
}

void Storage::createDirectory(const char *pathname) {
	if (pathname && pathname[0]) {
		auto tmp = WH_strdup(pathname);
		auto ret = _createDirectory(tmp);
		WH_free(tmp);
		if (!ret) {
			throw SystemException();
		}
	} else {
		throw Exception(EX_NULL);
	}
}

void Storage::removeDirectory(const char *pathname) {
	// Traverse depth-first, do not follow symbolic links
	if (pathname && nftw(pathname, rmHelper, 64, FTW_DEPTH | FTW_PHYS) == -1) {
		throw SystemException();
	}
}

int Storage::testDirectory(const char *pathname) noexcept {
	struct stat data;
	if (stat(pathname, &data) == 0) {
		return S_ISDIR(data.st_mode) ? 1 : 0;
	} else {
		return -1;
	}
}

int Storage::testFile(const char *pathname) noexcept {
	struct stat data;
	if (stat(pathname, &data) == 0) {
		return S_ISREG(data.st_mode) ? 1 : 0;
	} else {
		return -1;
	}
}

int Storage::testLink(const char *pathname) noexcept {
	struct stat linkstat;
	if (lstat(pathname, &linkstat) == 0) {
		return S_ISLNK(linkstat.st_mode) ? 1 : 0;
	} else {
		return -1;
	}
}

char* Storage::expandPathName(const char *pathname) noexcept {
	if (pathname) {
		char *buffer = nullptr;
		wordexp_t p;
		memset(&p, 0, sizeof(p));
		if (wordexp(pathname, &p, WRDE_NOCMD) == 0) {
			char **w = p.we_wordv;
			//Take the first one
			if (p.we_wordc) {
				buffer = WH_strdup(w[0]);
			}
		}
		wordfree(&p);
		return buffer;
	} else {
		return nullptr;
	}
}

char* Storage::directoryName(char *path) noexcept {
	return dirname(path);
}

char* Storage::baseName(char *path) noexcept {
	return basename(path);
}

bool Storage::createDirectoryForFile(const char *pathname) noexcept {
	if (pathname && pathname[0]) {
		auto tmp = WH_strdup(pathname);
		Twiddler::stripLast(tmp, DIR_SEPARATOR);
		auto ret = _createDirectory(tmp);
		WH_free(tmp);
		return ret;
	} else {
		return false;
	}
}

bool Storage::_createDirectory(char *pathname) noexcept {
	if (!pathname || !pathname[0]) {
		return false;
	}

	auto mark = pathname;
	for (; *mark == DIR_SEPARATOR; ++mark) {
		//skip heading '/'s
	}

	for (; *mark; ++mark) {
		mark = strchr(mark, DIR_SEPARATOR);

		if (mark) {		//Move into the tree one level at a time
			*mark = 0;
		}
		//-----------------------------------------------------------------
		if (access(pathname, F_OK) == -1) {
			//Directory doesn't exist, try to create one
			mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; // 755
			if (mkdir(pathname, mode) == -1) {
				return false;
			}
		} else if (testDirectory(pathname) != 1) {
			return false; //Not a directory
		}
		//-----------------------------------------------------------------
		if (mark) {
			*mark = DIR_SEPARATOR;
		} else {		//No more levels left
			break;
		}
	}
	return true;
}

} /* namespace wanhive */
