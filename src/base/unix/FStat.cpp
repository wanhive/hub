/*
 * FStat.cpp
 *
 * File status information
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "FStat.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <cstring>
#include <sys/sysmacros.h>

namespace wanhive {

FStat::FStat() noexcept {
	clear();
}

FStat::FStat(int fd) {
	read(fd);
}

FStat::FStat(const char *path, bool symLink) {
	read(path, symLink);
}

FStat::FStat(int dirfd, const char *path, int flags) {
	read(dirfd, path, flags);
}

FStat::~FStat() {
	clear();
}

bool FStat::isRegularFile() const noexcept {
	return S_ISREG(_stat.st_mode);
}

bool FStat::isDirectory() const noexcept {
	return S_ISDIR(_stat.st_mode);
}

bool FStat::isCharSpecialFile() const noexcept {
	return S_ISCHR(_stat.st_mode);
}

bool FStat::isBlockSpecialFile() const noexcept {
	return S_ISBLK(_stat.st_mode);
}

bool FStat::isFifo() const noexcept {
	return S_ISFIFO(_stat.st_mode);
}

bool FStat::isSymbolicLink() const noexcept {
	return S_ISLNK(_stat.st_mode);
}

bool FStat::isSocket() const noexcept {
	return S_ISSOCK(_stat.st_mode);
}

bool FStat::isMessageQueue() {
	return S_TYPEISMQ(&_stat);
}

bool FStat::isSemaphore() {
	return S_TYPEISSEM(&_stat);
}

bool FStat::isSharedMemory() {
	return S_TYPEISSHM(&_stat);
}

bool FStat::isSetUID() const noexcept {
	return (S_ISUID & _stat.st_mode);
}

bool FStat::isSetGID() const noexcept {
	return (S_ISGID & _stat.st_mode);
}

bool FStat::access(mode_t mode) const noexcept {
	return (mode & access());
}

mode_t FStat::access() const noexcept {
	return (_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
}

off_t FStat::size() const noexcept {
	return _stat.st_size;
}

uid_t FStat::ownerId() const noexcept {
	return _stat.st_uid;
}

gid_t FStat::groupId() const noexcept {
	return _stat.st_gid;
}

unsigned int FStat::majorId() const noexcept {
	return ::major(_stat.st_dev);
}

unsigned int FStat::majorDeviceId() const noexcept {
	return ::major(_stat.st_rdev);
}

unsigned int FStat::minorId() const noexcept {
	return ::minor(_stat.st_dev);
}

unsigned int FStat::minorDeviceId() const noexcept {
	return ::minor(_stat.st_rdev);
}

const timespec& FStat::accessedOn() const noexcept {
	return _stat.st_atim;
}

const timespec& FStat::modifiedOn() const noexcept {
	return _stat.st_mtim;
}

const timespec& FStat::statusChangedOn() const noexcept {
	return _stat.st_ctim;
}

struct stat* FStat::get() noexcept {
	return &_stat;
}

const struct stat* FStat::get() const noexcept {
	return &_stat;
}

void FStat::read(int fd) {
	if (::fstat(fd, &_stat) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FStat::read(const char *path, bool symLink) {
	if (!path) {
		throw Exception(EX_ARGUMENT);
	} else {
		auto ret = !symLink ? ::stat(path, &_stat) : ::lstat(path, &_stat);
		if (ret == -1) {
			throw SystemException();
		}
	}
}

void FStat::read(int dirfd, const char *path, int flags) {
	if (!path) {
		throw Exception(EX_ARGUMENT);
	} else if (::fstatat(dirfd, path, &_stat, flags) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void FStat::clear() noexcept {
	memset(&_stat, 0, sizeof(_stat));
}

} /* namespace wanhive */
