/*
 * System.cpp
 *
 * System and execution environment information
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "System.h"
#include "Storage.h"
#include "unix/SystemException.h"
#include <cstdlib>
#include <unistd.h>

namespace wanhive {

char* System::executableDirectory(char *buffer, size_t length) {
	Storage::readLink("/proc/self/exe", buffer, length);
	return Storage::directoryName(buffer);
}

char* System::currentWorkingDirectory(char *buffer, size_t length) {
	auto cwd = getcwd(buffer, length);
	if (cwd) {
		return cwd;
	} else {
		throw SystemException();
	}
}

const char* System::getEnvironment(const char *name) noexcept {
	if (!name) {
		return nullptr;
	} else {
		return ::getenv(name);
	}
}

} /* namespace wanhive */
