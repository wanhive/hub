/*
 * Process.h
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

namespace wanhive {
/**
 * Process management
 */
class Process {
public:
	//Wrapper for atexit(3)
	static bool atExit(void (*function)(void)) noexcept;

	//Wrapper for getpid(2): returns the ID of the calling process
	static pid_t self() noexcept;
	//Wrapper for getppid(2): returns the parent ID of the calling process
	static pid_t parent() noexcept;

	//Wrapper for fork(2): creates a child process
	static pid_t fork();
	//Wrapper for waitpid(2): waits for state changes in a child process
	static pid_t wait(pid_t pid, int options, int *status = nullptr) noexcept;

	//Wrapper for execv(2) and execvp(2): executes a file
	static void execute(const char *name, char *const argv[], bool search =
			false);
	//Wrapper for execve(2) and execvpe(2): executes a file
	static void execute(const char *name, char *const argv[],
			char *const envp[], bool search = false);
	//Wrapper for fexecve(2): executes a file
	static void execute(int fd, char *const argv[], char *const envp[]);

	//Wrapper for times(2): returns process times
	static clock_t time(tms *buf);

	//Wrapper for getrlimit(2): returns resource limits
	static void getLimit(int resource, rlimit *limit);
	//Wrapper for setrlimit(2): returns resource limit
	static void setLimit(int resource, const rlimit *limit);

	//Wrapper for getpriority(2): returns the scheduling priority
	static int getPriority(int which, id_t id);
	//Wrapper for setpriority(2): returns the scheduling priority
	static void setPriority(int which, id_t id, int value);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PROCESS_H_ */
