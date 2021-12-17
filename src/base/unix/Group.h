/*
 * Group.h
 *
 * Group ID management of the calling process
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_GROUP_H_
#define WH_BASE_UNIX_GROUP_H_
#include <sys/types.h>

namespace wanhive {
/**
 * Group ID management of the calling process
 */
class Group {
public:
	//Wrapper for getgid(2): returns the real group ID
	static gid_t getReal() noexcept;
	//Wrapper for getegid(2): returns the real group ID
	static gid_t getEffective() noexcept;

	//Wrapper for setgid(2): sets the group ID
	static void set(gid_t gid);
	//Wrapper for setregid(2): sets real and effective group IDs
	static void set(gid_t real, gid_t effective);
	//Wrapper for setegid(2): sets effective group ID
	static void setEffective(gid_t gid);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_GROUP_H_ */
