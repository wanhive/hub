/*
 * WorkingDirectory.h
 *
 * Working directory management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_WORKINGDIRECTORY_H_
#define WH_BASE_UNIX_WORKINGDIRECTORY_H_
#include <cstddef>

namespace wanhive {
/**
 * Working directory management
 */
class WorkingDirectory {
public:
	/**
	 * Wrapper for chdir(2): changes working directory.
	 * @param path new working directory's pathname
	 */
	static void set(const char *path);
	/**
	 * Wrapper for fchdir(2): changes working directory.
	 * @param fd new working directory's descriptor
	 */
	static void set(int fd);
	/**
	 * Wrapper for getcwd(3): returns the current working directory.
	 * @param buffer stores the current working directory's pathname
	 * @param size buffer's size in bytes
	 * @return pointer to the output string
	 */
	static char* get(char *buffer, size_t size);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_WORKINGDIRECTORY_H_ */
