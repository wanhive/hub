/**
 * @file Environment.h
 *
 * Environment variables
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_ENVIRONMENT_H_
#define WH_BASE_UNIX_ENVIRONMENT_H_

/*! @namespace wanhive */
namespace wanhive {
/**
 * Environment variables
 */
class Environment {
public:
	/**
	 * Returns the global "environ" variable.
	 * @return environ variable's value
	 */
	static char** get() noexcept;
	/**
	 * Wrapper for getenv(3): reads an environment variable's value.
	 * @param name environment variable's name
	 * @return environment variable's value
	 */
	static const char* get(const char *name) noexcept;
	/**
	 * Wrapper for putenv(3): changes or adds an environment variable.
	 * @param string environment variable string
	 */
	static void put(char *string);
	/**
	 * Wrapper for setenv(3): changes or adds an environment variable.
	 * @param name environment variable's name
	 * @param value environment variable's value
	 * @param replace true for replace on conflict, false otherwise
	 */
	static void set(const char *name, const char *value, bool replace);
	/**
	 * Wrapper for unsetenv(3): deletes an environment variable.
	 * @param name environment variable's name
	 */
	static void unset(const char *name);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_ENVIRONMENT_H_ */
