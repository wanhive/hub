/*
 * Environment.h
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

namespace wanhive {
/**
 * Environment variables
 */
class Environment {
public:
	//Returns a reference to the environ variable
	static char** get() noexcept;
	//Wrapper for getenv(3): gets an environment variable
	static const char* get(const char *name) noexcept;
	//Wrapper for putenv(3): changes or adds an environment variable
	static void put(char *string);
	//Wrapper for setenv(3): changes or adds an environment variable
	static void set(const char *name, const char *value, bool replace);
	//Wrapper for unsetenv(3): deletes an environment variable
	static void unset(const char *name);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_ENVIRONMENT_H_ */
