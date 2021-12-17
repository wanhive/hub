/*
 * User.h
 *
 * User ID management of the calling process
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_USER_H_
#define WH_BASE_UNIX_USER_H_
#include <sys/types.h>

namespace wanhive {
/**
 * User ID management of the calling process
 */
class User {
public:
	//Wrapper for getuid(2): returns the real user ID
	static uid_t getReal() noexcept;
	//Wrapper for geteuid(2): returns the real user ID
	static uid_t getEffective() noexcept;

	//Wrapper for setuid(2): sets the user ID
	static void set(uid_t uid);
	//Wrapper for setreuid(2): sets real and effective user IDs
	static void set(uid_t real, uid_t effective);
	//Wrapper for seteuid(2): sets effective user ID
	static void setEffective(uid_t uid);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_USER_H_ */
