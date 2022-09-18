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
 * REF: fcntl(2)
 */
class Fcntl {
public:
	//Duplicates a file descriptor
	static int duplicate(int fd, bool closeOnExec = false);
	//Duplicates a file descriptor
	static int duplicate(int fd, int newFd, bool closeOnExec = false);

	//Returns the file descriptor flag
	static int getDescriptorFlag(int fd);
	//Sets the file descriptor flag
	static void setDescriptorFlag(int fd, int flag);

	//Returns the file status flag
	static int getStatusFlag(int fd);
	//Sets the file status flag
	static void setStatusFlag(int fd, int flag);

	//Advisory record locking: sets record locks
	static bool setLock(int fd, struct flock &type, bool block);
	//Advisory record locking: tests existence of record locks
	static void testLock(int fd, struct flock &type);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FCNTL_H_ */
