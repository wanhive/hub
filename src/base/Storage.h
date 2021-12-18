/*
 * Storage.h
 *
 * Common file/directory handling routines
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
 * Common file/directory handling routines
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
	//Wrapper for fdopen(3), opens a file as stream
	static FILE* getStream(int fd, const char *modes);
	//If <strict> flag is set, then fails if <count> bytes could not be read
	static size_t read(int fd, void *buffer, size_t count, bool strict);
	//Returns the number of bytes written, throws SystemException
	static size_t write(int fd, const void *buffer, size_t count);
	//Wrapper for fsync(2): commits the data to the disk
	static void sync(int fd);
	//wrapper for lseek(2) system call
	static off_t seek(int fd, off_t offset, int whence);
	//Fills the file <fd> with the character <c>
	static void fill(int fd, size_t size, unsigned char c);
	//=================================================================
	//Implementation of "mkdir -p"
	static void createDirectory(const char *pathname);
	//Recursively remove all entries in a directory (rm -rf)
	static void removeDirectory(const char *pathname);
	//=================================================================
	//Returns 1 if a directory, 0 otherwise, -1 on error
	static int testDirectory(const char *pathname) noexcept;
	// Returns 1 if a regular file, 0 otherwise, -1 on error
	static int testFile(const char *pathname) noexcept;
	//Returns 1 if a symbolic link, 0 otherwise, -1 on error
	static int testLink(const char *pathname) noexcept;
	//=================================================================
	/*
	 * Performs posix shell like expansion of pathname.
	 * Returned buffer should be freed using WH_free
	 */
	static char* expandPathName(const char *pathname) noexcept;
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
