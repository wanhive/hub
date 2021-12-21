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
	/*
	 * Returns the directory containing the executable file running in the current
	 * process. The function modifies the memory pointed to by <buffer>.
	 * Do not call free on the returned pointer.
	 */
	static const char* executableDirectory(char *buffer, size_t length);
	/*
	 * Returns NUL-terminated absolute pathname that is the current working
	 * directory of the application. The function modifies the <buffer>.
	 */
	static char* currentWorkingDirectory(char *buffer, size_t length);
	/*
	 * Finds an environment variable <name> and returns a pointer to the
	 * corresponding "value" string (possibly nullptr).
	 * Caller must not modify/free the returned value
	 */
	static const char* getEnvironment(const char *name) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_SYSTEM_H_ */
