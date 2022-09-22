/*
 * Fcntl.h
 *
 * File descriptor operations
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_FCNTL_H_
#define WH_BASE_UNIX_FCNTL_H_
#include <fcntl.h>
#include <unistd.h>

namespace wanhive {
/**
 * File descriptor operations
 * @ref fcntl(2)
 */
class Fcntl {
public:
	/**
	 * Duplicates a file descriptor.
	 * @param fd file descriptor to duplicate
	 * @param closeOnExec true to set the close-on-exec descriptor flag for the
	 * new descriptor, false otherwise.
	 * @return new file descriptor's value
	 */
	static int duplicate(int fd, bool closeOnExec = false);
	/**
	 * Duplicates a file descriptor.
	 * @param fd file descriptor to duplicate
	 * @param newFd new file descriptor's recommended value
	 * @param closeOnExec  true to set the close-on-exec descriptor flag for the
	 * new descriptor, false otherwise.
	 * @return new file descriptor's value
	 */
	static int duplicate(int fd, int newFd, bool closeOnExec = false);
	//-----------------------------------------------------------------
	/**
	 * Returns the file descriptor flags.
	 * @param fd file descriptor
	 * @return descriptor flags
	 */
	static int getDescriptorFlag(int fd);
	/**
	 * Sets (overwrites) the file descriptor flags.
	 * @param fd file descriptor
	 * @param flag new descriptor flags
	 */
	static void setDescriptorFlag(int fd, int flag);
	/**
	 * Returns the file status flag.
	 * @param fd file descriptor
	 * @return status flags
	 */
	static int getStatusFlag(int fd);
	/**
	 * Sets (overwrites) the file status flags.
	 * @param fd file descriptor
	 * @param flag new status flags
	 */
	static void setStatusFlag(int fd, int flag);
	//-----------------------------------------------------------------
	/**
	 * Advisory record locking: acquires or releases record locks.
	 * @param fd file descriptor
	 * @param type specifies the parameters
	 * @param block true for blocking mode, false for non-blocking mode
	 * @return true on success, false on temporary error (the operation was
	 * prohibited or interrupted).
	 */
	static bool setLock(int fd, struct flock &type, bool block);
	/**
	 * Advisory record locking: tests existence of record locks.
	 * @param fd file descriptor
	 * @param type value-result argument for passing on the parameters and
	 * storing the results.
	 */
	static void testLock(int fd, struct flock &type);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FCNTL_H_ */
