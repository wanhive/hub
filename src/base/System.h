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
	 * Returns the executable file's (running in the current process) directory.
	 * @param buffer stores directory's pathname
	 * @param length buffer's size in bytes
	 * @return buffer's pointer
	 */
	static const char* executableDirectory(char *buffer, size_t length);
	/**
	 * Returns the current working directory.
	 * @param buffer stores directory's pathname
	 * @param length buffer's size in bytes
	 * @return buffer's pointer
	 */
	static char* currentWorkingDirectory(char *buffer, size_t length);
	/**
	 * Returns an environment variable's value.
	 * @param name environment variable's name
	 * @return pointer to the value string (possibly nullptr).
	 */
	static const char* getEnvironment(const char *name) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_SYSTEM_H_ */
