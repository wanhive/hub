/*
 * User.cpp
 *
 * User ID management of the calling process
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "User.h"
#include "SystemException.h"
#include <unistd.h>

namespace wanhive {

uid_t User::getReal() noexcept {
	return ::getuid();
}

uid_t User::getEffective() noexcept {
	return ::geteuid();
}

void User::set(uid_t uid) {
	if (::setuid(uid) == -1) {
		throw SystemException();
	}
}

void User::set(uid_t real, uid_t effective) {
	if (::setreuid(real, effective) == -1) {
		throw SystemException();
	}
}

void User::setEffective(uid_t uid) {
	if (::seteuid(uid) == -1) {
		throw SystemException();
	}
}

} /* namespace wanhive */
