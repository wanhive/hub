/*
 * PSession.h
 *
 * Job control: session management
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
 * Job control: session management
 */
class PSession {
public:
	/**
	 * Wrapper for setsid(2): creates a new session.
	 * @return new session identifier
	 */
	static pid_t create();
	/**
	 * Returns session identifier of the calling process.
	 * @return session identifier
	 */
	static pid_t get() noexcept;
	/**
	 * Wrapper for getsid(2): returns the session ID of a process.
	 * @param pid process' identifier
	 * @return session identifier
	 */
	static pid_t get(pid_t pid);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for tcgetpgrp(3): returns the foreground process group ID.
	 * @param fd controlling terminal's descriptor
	 * @return foreground process group's identifier
	 */
	static pid_t getForegroundGroup(int fd);
	/**
	 * Wrapper for tcsetpgrp(3): sets the foreground process group ID.
	 * @param fd controlling terminal's descriptor
	 * @param groupId foreground process group's identifier
	 */
	static void setForegroundGroup(int fd, pid_t groupId);
	/**
	 * Wrapper for tcgetsid(3): returns the controlling terminal's session ID.
	 * @param fd controlling terminal's descriptor
	 * @return session leader's process group identifier
	 */
	static pid_t getLeader(int fd);
public:
	/** Controlling terminal's pathname */
	static inline const char TERMINAL[] = "/dev/tty";
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PSESSION_H_ */
