/*
 * Group.cpp
 *
 * Group ID management of the calling process
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Group.h"
#include "SystemException.h"
#include "../common/defines.h"
#include <unistd.h>

namespace wanhive {

gid_t Group::getReal() noexcept {
	return ::getgid();
}

gid_t Group::getEffective() noexcept {
	return ::getegid();
}

void Group::set(gid_t gid) {
	if (::setgid(gid) == -1) {
		throw SystemException();
	}
}

void Group::set(gid_t real, gid_t effective) {
	if (::setregid(real, effective) == -1) {
		throw SystemException();
	}
}

void Group::setEffective(gid_t gid) {
	if (::setegid(gid) == -1) {
		throw SystemException();
	}
}

} /* namespace wanhive */
