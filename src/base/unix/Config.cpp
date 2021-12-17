/*
 * Config.cpp
 *
 * Runtime configuration
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Config.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <cerrno>

namespace wanhive {

long Config::system(int name) {
	errno = 0;
	auto ret = ::sysconf(name);
	if (ret == -1 && errno != 0) {
		throw SystemException();
	} else {
		return ret;
	}
}

long Config::path(int fd, int name) {
	errno = 0;
	auto ret = ::fpathconf(fd, name);
	if (ret == -1 && errno != 0) {
		throw SystemException();
	} else {
		return ret;
	}
}

long Config::path(const char *path, int name) {
	if (path == nullptr) {
		throw Exception(EX_INVALIDPARAM);
	} else {
		errno = 0;
		auto ret = ::pathconf(path, name);
		if (ret == -1 && errno != 0) {
			throw SystemException();
		} else {
			return ret;
		}
	}
}

} /* namespace wanhive */
