/*
 * PGroup.h
 *
 * Job control: process group management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_PGROUP_H_
#define WH_BASE_UNIX_PGROUP_H_
#include <sys/types.h>
#include <unistd.h>

namespace wanhive {
/**
 * Job control: process group management
 */
class PGroup {
public:
	/**
	 * Returns calling process' group ID.
	 * @return group identifier
	 */
	static pid_t get() noexcept;
	/**
	 * Wrapper for getpgid(2): Returns a process' group ID.
	 * @param processId process' identifier
	 * @return group identifier
	 */
	static pid_t get(pid_t processId);
	/**
	 * Sets calling process' group ID to its process ID.
	 */
	static void set();
	/**
	 * Sets calling process' group ID.
	 * @param groupId new group identifier
	 */
	static void set(pid_t groupId);
	/**
	 * Wrapper for setpgid(2): sets a process' group ID.
	 * @param processId process' identifier
	 * @param groupId new group identifier
	 */
	static void set(pid_t processId, pid_t groupId);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PGROUP_H_ */
