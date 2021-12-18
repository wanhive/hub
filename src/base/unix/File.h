/*
 * File.h
 *
 * File descriptor management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_FILE_H_
#define WH_BASE_UNIX_FILE_H_
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace wanhive {
/**
 * File descriptor management
 */
class File {
public:
	//Creates an empty object (call open later)
	File() noexcept;
	//Associates the file descriptor <fd> with this object
	File(int fd) noexcept;
	//Opens a file
	File(const char *path, int flags);
	//Opens or creates a file
	File(const char *path, int flags, mode_t mode);
	//Opens a file
	File(int dirfd, const char *path, int flags);
	//Opens or creates a file
	File(int dirfd, const char *path, int flags, mode_t mode);
	//Automatically closes the associated file descriptor
	~File();

	//Returns the file descriptor associated with this object
	int get() const noexcept;
	//Sets a file descriptor after closing the existing one
	void set(int fd) noexcept;
	//Releases the file descriptor associated with this object
	int release() noexcept;
	//Wrapper for close(2): Closes the file descriptor
	bool close() noexcept;

	//Wrapper for open(2): opens a file
	void open(const char *path, int flags);
	//Wrapper for open(2): opens or creates a file
	void open(const char *path, int flags, mode_t mode);
	//Wrapper for openat(2): opens a file
	void open(int dirfd, const char *path, int flags);
	//Wrapper for openat(2): opens or creates a file
	void open(int dirfd, const char *path, int flags, mode_t mode);

	//Wrapper for flock(2): applies an advisory lock
	bool lock(bool shared, bool block);
	//Wrapper for flock(2): removes an advisory lock
	void unlock();

	//Wrapper for dup(2): duplicates a file descriptor
	int duplicate();
	//Wrapper for dup3(2) [fallback: dup2(2)]: duplicates a file descriptor
	int duplicate(int newfd, int flag = 0);
private:
	int fd;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FILE_H_ */
