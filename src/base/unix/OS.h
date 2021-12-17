/*
 * OS.h
 *
 * Operating system's information
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
 * Operating system's information
 */
class OS {
public:
	OS();
	~OS();

	//Name of the operating system
	const char* name() const noexcept;
	//Name of this node
	const char* node() const noexcept;
	//Current release of the operating system
	const char* release() const noexcept;
	//Current version of this release
	const char* version() const noexcept;
	//Hardware's name
	const char* machine() const noexcept;
	//Hostname of this system
	const char* host() const noexcept;
private:
	//Wrapper for uname(2) and gethostname(2)
	void init();
private:
	utsname _name;
	char _host[HOST_NAME_MAX + 1];
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_OS_H_ */
