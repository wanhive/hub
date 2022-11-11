/*
 * Environment.cpp
 *
 * Environment variables
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Environment.h"
#include "SystemException.h"
#include "../common/Exception.h"
#include <cstdlib>

extern char **environ;

namespace wanhive {

char** Environment::get() noexcept {
	return environ;
}

const char* Environment::get(const char *name) noexcept {
	if (name) {
		return ::getenv(name);
	} else {
		return nullptr;
	}
}

void Environment::put(char *string) {
	if (!string) {
		throw Exception(EX_ARGUMENT);
	} else if (::putenv(string) != 0) {
		throw SystemException();
	} else {
		return;
	}
}

void Environment::set(const char *name, const char *value, bool replace) {
	if (!name || !value) {
		throw Exception(EX_ARGUMENT);
	} else if (::setenv(name, value, (replace ? 1 : 0)) != 0) {
		throw SystemException();
	} else {
		return;
	}
}

void Environment::unset(const char *name) {
	if (!name) {
		throw Exception(EX_ARGUMENT);
	} else if (::unsetenv(name) != 0) {
		throw SystemException();
	} else {
		return;
	}
}

} /* namespace wanhive */
