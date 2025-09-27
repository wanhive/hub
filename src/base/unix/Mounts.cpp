/*
 * Mounts.cpp
 *
 * Mounted file systems scanner
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Mounts.h"
#include "SystemException.h"

namespace wanhive {

Mounts::Mounts() {
	open();
}

Mounts::~Mounts() {
	close();
}

mntent* Mounts::next() {
	return getmntent(fp);
}

void Mounts::open() {
	close();
	fp = setmntent("/proc/mounts", "r");
	if (fp == nullptr) {
		throw SystemException();
	}
}

void Mounts::close() noexcept {
	if (fp) {
		endmntent(fp);
		fp = nullptr;
	}
}

} /* namespace wanhive */
