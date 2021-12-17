/*
 * PSession.h
 *
 * Process session management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_PSESSION_H_
#define WH_BASE_UNIX_PSESSION_H_
#include <sys/types.h>

namespace wanhive {
/**
 * Process session management
 */
class PSession {
public:
	//Wrapper for setsid(2): creates a session
	static pid_t create();
	//Returns the session ID of the calling process
	static pid_t get() noexcept;
	//Wrapper for getsid(2): returns the session ID of a process
	static pid_t get(pid_t pid);

	//Wrapper for tcgetpgrp(3): returns the foreground process group ID
	static pid_t getForegroundGroup(int fd);
	//Wrapper for tcsetpgrp(3): sets the foreground process group ID
	static void setForegroundGroup(int fd, pid_t groupId);
public:
	static const char *TERMINAL;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PSESSION_H_ */
