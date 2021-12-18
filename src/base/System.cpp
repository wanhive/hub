/*
 * System.cpp
 *
 * Basic system information
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "System.h"
#include "unix/Environment.h"
#include "unix/FileSystem.h"
#include "unix/WorkingDirectory.h"

namespace wanhive {

const char* System::executableDirectory(char *buffer, size_t length) {
	FileSystem::readLink("/proc/self/exe", buffer, length);
	return FileSystem::directoryName(buffer);
}

char* System::currentWorkingDirectory(char *buffer, size_t length) {
	return WorkingDirectory::get(buffer, length);
}

const char* System::getEnvironment(const char *name) noexcept {
	return Environment::get(name);
}

} /* namespace wanhive */
