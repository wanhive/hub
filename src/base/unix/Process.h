/**
 * @file Process.h
 *
 * Process management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_PROCESS_H_
#define WH_BASE_UNIX_PROCESS_H_
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>

/*! @namespace wanhive */
namespace wanhive {
/**
 * Process management
 */
class Process {
public:
	/**
	 * Wrapper for atexit(3): registers a function to be called at normal
	 * process termination.
	 * @param function callback function
	 * @return true on success, false otherwise
	 */
	static bool atExit(void (*function)(void)) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for getpid(2): returns the ID of the calling process.
	 * @return calling process's identifier
	 */
	static pid_t self() noexcept;
	/**
	 * Wrapper for getppid(2): returns the parent ID of the calling process.
	 * @return calling process' parent identifier
	 */
	static pid_t parent() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for fork(2): creates a child process.
	 * @return child's process identifier in the parent, 0 in the child
	 */
	static pid_t fork();
	/**
	 * Wrapper for waitpid(2): waits for state changes in a child process
	 * @param pid process or group identifier
	 * @param options operation flags
	 * @param status stores the child process' status
	 * @return returned value of the system call
	 */
	static pid_t wait(pid_t pid, int options, int *status = nullptr) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for execv(2) and execvp(2): executes a file.
	 * @param name executable file's pathname
	 * @param argv program's arguments
	 * @param search true for PATH search, false otherwise
	 */
	static void execute(const char *name, char *const argv[], bool search =
			false);
	/**
	 * Wrapper for execve(2) and execvpe(2): executes a file.
	 * @param name executable file's pathname
	 * @param argv program's arguments
	 * @param envp program's environment
	 * @param search true for PATH search, false otherwise
	 */
	static void execute(const char *name, char *const argv[],
			char *const envp[], bool search = false);
	/**
	 * Wrapper for fexecve(2): executes a file.
	 * @param fd executable file's descriptor
	 * @param argv program's arguments
	 * @param envp program's environment
	 */
	static void execute(int fd, char *const argv[], char *const envp[]);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for times(2): returns process times.
	 * @param buf stores the the process times (can be nullptr)
	 * @return calling process' CPU time
	 */
	static clock_t time(tms *buf);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for getrlimit(2): returns resource limits.
	 * @param resource resource identifier
	 * @param limit stores the limits
	 */
	static void getLimit(int resource, rlimit *limit);
	/**
	 * Wrapper for setrlimit(2): sets resource limits.
	 * @param resource resource identifier
	 * @param limit new limits
	 */
	static void setLimit(int resource, const rlimit *limit);
	/**
	 * Wrapper for getpriority(2): returns the scheduling priority.
	 * @param which target's type
	 * @param id target's identifier
	 * @return current nice value
	 */
	static int getPriority(int which, id_t id);
	/**
	 * Wrapper for setpriority(2): sets the scheduling priority.
	 * @param which target's type
	 * @param id target's identifier
	 * @param value new nice value
	 */
	static void setPriority(int which, id_t id, int value);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PROCESS_H_ */
