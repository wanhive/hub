/*
 * Storage.h
 *
 * File and directory management on Linux
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_STORAGE_H_
#define WH_BASE_STORAGE_H_
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>

namespace wanhive {
/**
 * Basic routines for file/directory handling
 */
class Storage {
public:
	//Creates the directory structure if <createPath> is true, returns nullptr on error
	static FILE* openStream(const char *path, const char *modes,
			bool createPath) noexcept;
	//Returns 0 on success, EOF on error
	static int closeStream(FILE *fp) noexcept;
	//Wrapper for fileno(3), returns the file descriptor
	static int getDescriptor(FILE *stream);
	//=================================================================
	//Wrapper for open(2) system call, if <createPath> then tries to create directories
	static int open(const char *path, int flags, mode_t mode, bool createPath);
	//Wrapper for close(2) system call, returns 0 on success, -1 on error
	static int close(int fd) noexcept;
	//Wrapper for fdopen, opens a file as stream
	static FILE* getStream(int fd, const char *modes);
	//If <strict> flag is set, then fails if <count> bytes could not be read
	static size_t read(int fd, void *buffer, size_t count, bool strict);
	//Returns the number of bytes written, throws SystemException
	static size_t write(int fd, const void *buffer, size_t count);
	//Wrapper for fsync(2): commits the data to the disk
	static void sync(int fd);
	//wrapper for lseek(2) system call
	static off_t seek(int fd, off_t offset, int whence);
	//Fills the file <fd> with character <c>
	static void fill(int fd, size_t size, unsigned char c);
	/*
	 * Returns the resolved symbolic link
	 * Wrapper for readlink(2), places nul-terminated linked path in <buf>.
	 * Returns the number of bytes copied, and if it equals the <len>
	 * then we cannot tell whether the full name was copied or not.
	 */
	static int readLink(const char *pathname, char *buf, size_t len);
	//=================================================================
	/**
	 * Wrappers for the flock(2) system call.
	 */

	/*
	 * Places an advisory lock on the specified file, if <shared> is false then
	 * an exclusive lock is placed. If <block> is true then the method call
	 * blocks in case of contention/conflict. Returns true on success, false
	 * if the call would block and nonblocking operation was selected.
	 */
	static bool lock(int fd, bool shared, bool block = true);
	//Removes an existing lock held by the process, returns true on success
	static bool unlock(int fd);
	//=================================================================
	//Implementation of "mkdir -p"
	static void createDirectory(const char *pathname);
	//Recursively remove all entries in a directory (rm -rf)
	static void removeDirectory(const char *pathname);
	//=================================================================
	//Returns 1 if directory, 0 otherwise, -1 on error
	static int testDirectory(const char *pathname) noexcept;
	// Returns 1 if file, 0 otherwise, -1 on error
	static int testFile(const char *pathname) noexcept;
	//Returns 1 if path is a symbolic link, 0 otherwise, -1 on error
	static int testLink(const char *pathname) noexcept;
	//=================================================================
	/*
	 * Performs posix shell like expansion of pathname.
	 * Returned buffer should be freed using WH_free
	 */
	static char* expandPathName(const char *pathname) noexcept;
	/*
	 * Wrapper for dirname(3)
	 * Breaks the path into directory name component.
	 * The function modifies the <path> argument.
	 * Do not free the returned pointer.
	 */
	static char* directoryName(char *path) noexcept;
	/*
	 * Wrapper for basename(3)
	 * Breaks the path into filename component.
	 * The function modifies the <path> argument.
	 * Do not free the returned pointer.
	 */
	static char* baseName(char *path) noexcept;
	//Replaces special characters in the filename with '_'
	static void canonicalize(char *name) noexcept;
private:
	/*
	 * Creates the complete directory structure for storing a file
	 * Returns true on success, false on error
	 */
	static bool createDirectoryForFile(const char *pathname) noexcept;
	//Returns true on success, returns false on error (pathname may be modified)
	static bool _createDirectory(char *pathname) noexcept;
public:
	static const char DIR_SEPARATOR;
	static const char *DIR_SEPARATOR_STR;
	static const char PATH_SEPARATOR;
	static const char *PATH_SEPARATOR_STR;
	static const char *NEWLINE;
};

} /* namespace wanhive */

#endif /* WH_BASE_STORAGE_H_ */
