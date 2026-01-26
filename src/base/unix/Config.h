/**
 * @file Config.h
 *
 * Runtime configuration
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_CONFIG_H_
#define WH_BASE_UNIX_CONFIG_H_
#include <climits>
#include <unistd.h>

/*! @namespace wanhive */
namespace wanhive {
/**
 * Runtime configuration
 */
class Config {
public:
	/**
	 * Wrapper for sysconf(3): returns system configuration at runtime.
	 * @param name option's value
	 * @return configuration value
	 */
	static long system(int name);
	/**
	 * Wrapper for fpathconf(3): returns configuration values for files.
	 * @param fd open file descriptor
	 * @param name option's value
	 * @return configuration value
	 */
	static long path(int fd, int name);
	/**
	 * Wrapper for pathconf(3):  returns configuration values for files.
	 * @param path file's pathname
	 * @param name option's value
	 * @return configuration value
	 */
	static long path(const char *path, int name);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_CONFIG_H_ */
