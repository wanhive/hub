/*
 * Mounts.h
 *
 * Mounted file systems information
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_MOUNTS_H_
#define WH_BASE_UNIX_MOUNTS_H_
#include <cstdio>
#include <mntent.h>

namespace wanhive {
/**
 * Provides information about currently mounted file systems.
 */
class Mounts {
public:
	/**
	 * Constructor: opens the /proc/mounts special file stream for reading.
	 */
	Mounts();
	/**
	 * Destructor: closes the file stream.
	 */
	~Mounts();
	/**
	 * Reads the next file system description (wrapper for getmntent(3)). This
	 * function overwrites a static memory area on subsequent calls.
	 * @return pointer to description structure on success, nullptr on failure
	 */
	mntent* next();
private:
	void open();
	void close() noexcept;
private:
	FILE *fp { nullptr };
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_MOUNTS_H_ */
