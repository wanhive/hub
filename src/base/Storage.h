/*
 * Storage.h
 *
 * Common file system routines
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
 * Common file system routines
 */
class Storage {
public:
	/**
	 * Opens a file stream (wrapper for fopen(3)).
	 * @param path file's path
	 * @param modes string describing how to open the file
	 * @return file stream on success, nullptr on error
	 */
	static FILE* openStream(const char *path, const char *modes) noexcept;
	/**
	 * Closes a file stream (wrapper for fclose(3)).
	 * @param fp the file pointer
	 * @return value returned by fclose(3)
	 */
	static int closeStream(FILE *fp) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Opens a file (wrapper for open(2)).
	 * @param path file's path
	 * @param flags status flags
	 * @param mode mode flags
	 * @return file descriptor
	 */
	static int open(const char *path, int flags, mode_t mode = 0);
	/**
	 * Closes a file descriptor (wrapper for close(2)).
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
	 * Commits data to the disk (wrapper for fsync(2)).
	 * @param fd file descriptor
	 */
	static void sync(int fd);
	/**
	 * Repositions file offset (wrapper for lseek(2)).
	 * @param fd file descriptor
	 * @param offset the offset value
	 * @param whence the offset directive
	 * @return offset from beginning of the file in bytes
	 */
	static off_t seek(int fd, off_t offset, int whence);
	/**
	 * Truncates a file to a given length (wrapper for ftruncate(2)).
	 * @param fd file descriptor
	 * @param length truncated size (bytes)
	 */
	static void truncate(int fd, off_t length);
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
	 * @param path directory's path
	 */
	static void createDirectory(const char *path);
	/**
	 * Removes a directory and it's contents (equivalent to rm -rf).
	 * @param path directory's path
	 */
	static void removeDirectory(const char *path);
	//-----------------------------------------------------------------
	/**
	 * Checks if a pathname belongs to a directory.
	 * @param path pathname to check
	 * @return 1 if directory, 0 if not a directory, -1 on error
	 */
	static int testDirectory(const char *path) noexcept;
	/**
	 * Checks if a pathname belongs to a regular file.
	 * @param path pathname to check
	 * @return 1 if regular file, 0 if not a regular file, -1 on error
	 */
	static int testFile(const char *path) noexcept;
	/**
	 * Checks if a pathname belongs to a symbolic link.
	 * @param path pathname to check
	 * @return 1 if symbolic link, 0 if not a symbolic link, -1 on error
	 */
	static int testLink(const char *path) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Performs posix shell like expansion of pathname.
	 * @param path pathname for expansion
	 * @return expanded pathname (caller should free this string).
	 */
	static char* expand(const char *path) noexcept;
private:
	static bool createDirectoryForFile(const char *path) noexcept;
	static bool _createDirectory(char *path) noexcept;
public:
	/*! Pathname separator character */
	static constexpr char PATH_SEPARATOR = '/';
	/*! Pathname separator string */
	static constexpr const char *PATH_SEPARATOR_STR = "/";
};

} /* namespace wanhive */

#endif /* WH_BASE_STORAGE_H_ */
