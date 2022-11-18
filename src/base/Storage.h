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
	/**
	 * Opens a file stream (wrapper for fopen(3) library function).
	 * @param path file's pathname
	 * @param modes string describing how to open the file
	 * @return file stream on success, nullptr on error
	 */
	static FILE* openStream(const char *path, const char *modes) noexcept;
	/**
	 * Closes a file stream (wrapper for fclose(3) library function).
	 * @param fp the file pointer
	 * @return value returned by fclose(3)
	 */
	static int closeStream(FILE *fp) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Opens a file (wrapper for open(2) system call).
	 * @param path file's pathname
	 * @param flags status flags
	 * @param mode mode flags
	 * @return file descriptor
	 */
	static int open(const char *path, int flags, mode_t mode = 0);
	/**
	 * Closes a file descriptor (wrapper for close(2) system call).
	 * @param fd file descriptor
	 * @return value returned by close(2)
	 */
	static int close(int fd) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads data from a given file.
	 * @param fd file descriptor
	 * @param buffer input buffer
	 * @param count maximum number of bytes of data to read
	 * @param strict true to read exactly the given bytes of data, false to read
	 * at most the given bytes of data.
	 * @return number of bytes read (possibly zero if EOF encountered)
	 */
	static size_t read(int fd, void *buffer, size_t count, bool strict);
	/**
	 * Writes data to a given file.
	 * @param fd file descriptor
	 * @param buffer output buffer
	 * @param count number of bytes to write
	 * @return number of bytes written
	 */
	static size_t write(int fd, const void *buffer, size_t count);
	/**
	 * Commits data to the disk: wrapper for fsync(2) system call.
	 * @param fd file descriptor
	 */
	static void sync(int fd);
	/**
	 * Repositions file offset: wrapper for lseek(2) system call.
	 * @param fd file descriptor
	 * @param offset the offset value
	 * @param whence the offset directive
	 * @return offset from beginning of the file in bytes
	 */
	static off_t seek(int fd, off_t offset, int whence);
	/**
	 * Fills up a file with the given character.
	 * @param fd file descriptor
	 * @param size number of characters to write
	 * @param c the character to write
	 */
	static void fill(int fd, size_t size, unsigned char c);
	//-----------------------------------------------------------------
	/**
	 * Makes the directory and it's parent directories as needed (equivalent to
	 * "mkdir -p").
	 * @param pathname directory's pathname
	 */
	static void createDirectory(const char *pathname);
	/**
	 * Removes a directory and it's contents (equivalent to rm -rf).
	 * @param pathname directory's pathname
	 */
	static void removeDirectory(const char *pathname);
	//-----------------------------------------------------------------
	/**
	 * Checks if the given pathname belongs to a directory.
	 * @param pathname the pathname to check
	 * @return 1 if directory, 0 if not a directory, -1 on error
	 */
	static int testDirectory(const char *pathname) noexcept;
	/**
	 * Checks if the given pathname belongs to a regular file.
	 * @param pathname the pathname to check
	 * @return 1 if regular file, 0 if not a regular file, -1 on error
	 */
	static int testFile(const char *pathname) noexcept;
	/**
	 * Checks if the given pathname belongs to a symbolic link.
	 * @param pathname the pathname to check
	 * @return 1 if symbolic link, 0 if not a symbolic link, -1 on error
	 */
	static int testLink(const char *pathname) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs posix shell like expansion of pathname.
	 * @param pathname pathname for expansion
	 * @return expanded pathname (caller should free this string).
	 */
	static char* expandPathName(const char *pathname) noexcept;
private:
	static bool createDirectoryForFile(const char *pathname) noexcept;
	static bool _createDirectory(char *pathname) noexcept;
public:
	/** Pathname separator character */
	static constexpr char PATH_SEPARATOR = '/';
	/** Pathname separator string */
	static inline const char PATH_SEPARATOR_STR[] = "/";
};

} /* namespace wanhive */

#endif /* WH_BASE_STORAGE_H_ */
