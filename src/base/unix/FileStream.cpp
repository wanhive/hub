/*
 * FileStream.cpp
 *
 * File stream management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "FileStream.h"
#include "SystemException.h"

namespace wanhive {

FileStream::FileStream() noexcept :
		fp(nullptr) {

}

FileStream::FileStream(FILE *fp) noexcept :
		fp(fp) {
}

FileStream::FileStream(const char *path, const char *mode) :
		fp(nullptr) {
	open(path, mode);
}

FileStream::FileStream(int fd, const char *mode) :
		fp(nullptr) {
	open(fd, mode);
}

FileStream::~FileStream() {
	close();
}

FILE* FileStream::get() const noexcept {
	return this->fp;
}

void FileStream::set(FILE *fp) noexcept {
	if (fp != this->fp) {
		close();
		this->fp = fp;
	}
}

FILE* FileStream::release() noexcept {
	auto ret = fp;
	fp = nullptr;
	return ret;
}

bool FileStream::close() noexcept {
	if (fp) {
		auto status = ::fclose(fp);
		fp = nullptr;
		return (status == 0);
	} else {
		return true;
	}
}

void FileStream::open(const char *path, const char *mode) {
	auto fp = ::fopen(path, mode);
	if (!fp) {
		throw SystemException();
	} else {
		set(fp);
	}
}

void FileStream::open(int fd, const char *mode) {
	auto fp = ::fdopen(fd, mode);
	if (!fp) {
		throw SystemException();
	} else {
		set(fp);
	}
}

void FileStream::reopen(const char *path, const char *mode) {
	auto ret = ::freopen(path, mode, fp);
	if (!ret) {
		throw SystemException();
	}
}

} /* namespace wanhive */
