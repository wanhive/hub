/*
 * OS.cpp
 *
 * System information
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "OS.h"
#include "SystemException.h"
#include <unistd.h>

namespace wanhive {

OS::OS() {
	init();
}

OS::~OS() {

}

const char* OS::name() const noexcept {
	return _name.sysname;
}

const char* OS::node() const noexcept {
	return _name.nodename;
}

const char* OS::release() const noexcept {
	return _name.release;
}

const char* OS::version() const noexcept {
	return _name.version;
}

const char* OS::machine() const noexcept {
	return _name.machine;
}

const char* OS::host() const noexcept {
	return _host;
}

void OS::init() {
	if (::uname(&_name) == -1 || ::gethostname(_host, sizeof(_host)) == -1) {
		throw SystemException();
	}
}

} /* namespace wanhive */
