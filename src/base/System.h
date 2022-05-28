/*
 * System.h
 *
 * Basic system information
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SYSTEM_H_
#define WH_BASE_SYSTEM_H_
#include <cstddef>

namespace wanhive {
/**
 * Basic system information
 */
class System {
public:
	/**
	 * Returns the directory that contains the executable file running in the
	 * current process.
	 * @param buffer buffer for storing the pathname
	 * @param length buffer's size n bytes
	 * @return the pathname (do not free this pointer)
	 */
	static const char* executableDirectory(char *buffer, size_t length);
	/**
	 * Returns NUL-terminated absolute pathname that is the current working
	 * directory of the application.
	 * @param buffer buffer for storing the pathname
	 * @param length buffer's size in bytes
	 * @return the pathname
	 */
	static char* currentWorkingDirectory(char *buffer, size_t length);
	/**
	 * Returns the value of an environment variable.
	 * @param name environment variable's name
	 * @return environment variable's value, possibly nullptr.
	 */
	static const char* getEnvironment(const char *name) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_SYSTEM_H_ */
