/*
 * PGroup.cpp
 *
 * Process group management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "PGroup.h"
#include "SystemException.h"

namespace wanhive {

pid_t PGroup::get() noexcept {
	return ::getpgid(0);
}

pid_t PGroup::get(pid_t processId) {
	auto ret = ::getpgid(processId);
	if (ret == -1) {
		throw SystemException();
	} else {
		return ret;
	}
}

void PGroup::set() {
	set(0, 0);
}

void PGroup::set(pid_t groupId) {
	set(0, groupId);
}

void PGroup::set(pid_t processId, pid_t groupId) {
	auto ret = ::setpgid(processId, groupId);
	if (ret == -1) {
		throw SystemException();
	} else {
		return;
	}
}

} /* namespace wanhive */
