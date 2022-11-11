/*
 * WorkingDirectory.cpp
 *
 * Working directory management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "WorkingDirectory.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <unistd.h>

namespace wanhive {

void WorkingDirectory::set(const char *path) {
	if (!path) {
		throw Exception(EX_ARGUMENT);
	} else if (::chdir(path) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void WorkingDirectory::set(int fd) {
	if (::fchdir(fd) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

char* WorkingDirectory::get(char *buffer, size_t size) {
	auto cwd = ::getcwd(buffer, size);
	if (cwd) {
		return cwd;
	} else {
		throw SystemException();
	}
}

} /* namespace wanhive */
