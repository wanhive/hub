/*
 * OS.h
 *
 * System information
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_OS_H_
#define WH_BASE_UNIX_OS_H_
#include <climits>
#include <sys/utsname.h>

namespace wanhive {
/**
 * System information
 */
class OS {
public:
	/**
	 * Default constructor: reads the system information.
	 */
	OS();
	/**
	 * Destructor
	 */
	~OS();

	/**
	 * Returns operating system's name.
	 * @return name of the operating system
	 */
	const char* name() const noexcept;
	/**
	 * Returns node's network name.
	 * @return name of this node
	 */
	const char* node() const noexcept;
	/**
	 * Returns the release information.
	 * @return current release level
	 */
	const char* release() const noexcept;
	/**
	 * Returns the version information.
	 * @return current version of this release
	 */
	const char* version() const noexcept;
	/**
	 * Returns hardware's information.
	 * @return hardware's name
	 */
	const char* machine() const noexcept;
	/**
	 * Returns system's host name (same as OS::node()).
	 * @return host name of this system
	 */
	const char* host() const noexcept;
private:
	void init();
private:
	utsname _name;
	char _host[HOST_NAME_MAX + 1];
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_OS_H_ */
