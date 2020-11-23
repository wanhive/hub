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
#include "SystemException.h"
#include "common/Exception.h"
#include "common/Memory.h"
#include "ds/Twiddler.h"
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <ftw.h>
#include <libgen.h>
#include <unistd.h>
#include <wordexp.h>
#include <sys/file.h>

namespace {
//Helper function for removeDirectory
int rmHelper(const char *path, const struct stat *s, int flag,
		struct FTW *f) noexcept {
	int rv = remove(path);
	if (rv) {
		perror(path);
	}
	return rv;
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
	int result = fileno(stream);
	if (result != -1) {
		return result;
	} else {
		throw SystemException();
	}
}

int Storage::open(const char *path, int flags, mode_t mode, bool createPath) {
	if (path && path[0]) {
		int fd = -1;
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
	FILE *fp = fdopen(fd, modes);
	if (fp) {
		return fp;
	} else {
		throw SystemException();
	}
}

size_t Storage::read(int fd, void *buffer, size_t count, bool strict) {
	size_t toRead = count;
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
	size_t toWrite = count;
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
	off_t ret = ::lseek(fd, offset, whence);
	if (ret != (off_t) -1) {
		return ret;
	} else {
		throw SystemException();
	}
}

void Storage::fill(int fd, size_t size, unsigned char c) {
	unsigned char buffer[4096];
	size_t bytesleft = size;

	memset(buffer, c, sizeof(buffer));
	seek(fd, 0, SEEK_SET);

	while (bytesleft) {
		size_t toWrite =
				(bytesleft > sizeof(buffer)) ? sizeof(buffer) : bytesleft;
		bytesleft -= write(fd, buffer, toWrite);
	}
}

int Storage::readLink(const char *pathname, char *buf, size_t len) {
	//Read at-most len-1 bytes (readlink doesn't append nul terminator)
	int ret = readlink(pathname, buf, len - 1);
	if (ret != -1) {
		buf[ret] = '\0';
		return ret;
	} else {
		throw SystemException();
	}
}

bool Storage::lock(int fd, bool shared, bool block) noexcept {
	int flag = 0;
	if (!block) {
		flag = LOCK_NB;
	}
	if (shared) {
		flag |= LOCK_SH;
	} else {
		flag |= LOCK_EX;
	}

	if (flock(fd, flag) == 0) {
		return true;
	} else {
		return false;
	}
}

bool Storage::unlock(int fd, bool block) noexcept {
	int flag = LOCK_UN;
	if (!block) {
		flag |= LOCK_NB;
	}

	if (flock(fd, flag) == 0) {
		return true;
	} else {
		return false;
	}
}

void Storage::createDirectory(const char *pathname) {
	if (pathname && pathname[0]) {
		char *tmp = WH_strdup(pathname);
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
	struct stat filestat;
	if (!pathname) {
		return -1;
	} else if (stat(pathname, &filestat) != 0) {
		if (errno != ENOENT) {
			return -1;
		} else {
			return 0;
		}
	} else if (!S_ISDIR(filestat.st_mode)) {
		return 0;
	} else if (access(pathname, F_OK)) {
		return -1;
	} else { //passed all tests
		return 1;
	}
}

int Storage::testFile(const char *pathname) noexcept {
	struct stat filestat;
	if (!pathname) {
		return -1;
	} else if (stat(pathname, &filestat) != 0) {
		if (errno != ENOENT) {
			return -1;
		} else {
			return 0;
		}
	} else if (!S_ISREG(filestat.st_mode)) {
		return 0;
	} else if (access(pathname, F_OK) < 0) {
		return -1;
	} else { //passed all tests
		return 1;
	}
}

int Storage::testLink(const char *pathname) noexcept {
	struct stat linkstat;
	if (!pathname) {
		return -1;
	} else if (lstat(pathname, &linkstat) != 0) {
		if (errno != ENOENT) {
			return -1;
		} else {
			return 0;
		}
	} else if (!S_ISLNK(linkstat.st_mode)) {
		return 0;
	} else if (access(pathname, F_OK) < 0) {
		return -1;
	} else { //passed all tests
		return 1;
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

void Storage::canonicalize(char *name) noexcept {
	char *idx;
	char c;

	idx = name;
	while (*idx) {
		c = *idx;

		if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?'
				|| c == '"' || c == '<' || c == '>' || c == '|') {
			*idx = '_';
		}

		idx++;
	}
}

bool Storage::createDirectoryForFile(const char *pathname) noexcept {
	if (pathname && pathname[0]) {
		char *tmp = WH_strdup(pathname);
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

	char *mark = pathname;
	for (; *mark == DIR_SEPARATOR; ++mark) {
		//skip heading '/'s
	}

	for (; *mark; ++mark) {
		mark = strchr(mark, DIR_SEPARATOR);

		if (mark) {		//Move into the tree one level at a time
			*mark = 0;
		}
		//-----------------------------------------------------------------
		int ret = testDirectory(pathname);
		if (ret == -1) {
			return false;
		}
		if (ret == 0) {
			const mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP
					| S_IROTH | S_IXOTH; // 755
			ret = mkdir(pathname, mode);
			if ((ret != 0) && (errno != EEXIST)) {
				return false;
			}
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
