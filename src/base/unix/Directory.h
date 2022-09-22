/*
 * Directory.h
 *
 * Directory management
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_DIRECTORY_H_
#define WH_BASE_UNIX_DIRECTORY_H_
#include <dirent.h>
#include <fcntl.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace wanhive {
/**
 * Directory management
 */
class Directory {
public:
	/**
	 * Default constructor: creates an empty object.
	 */
	Directory() noexcept;
	/**
	 * Constructor: opens a directory.
	 * @param name directory's pathname
	 */
	Directory(const char *name);
	/**
	 * Constructor: Opens directory associated with the given file descriptor.
	 * @param fd director's file descriptor
	 */
	Directory(int fd);
	/**
	 * Destructor: automatically closes the associated directory stream.
	 */
	~Directory();
	//-----------------------------------------------------------------
	/**
	 * Wrapper for readdir(3): reads a directory entry at current position.
	 * @return the next directory entry
	 */
	dirent* read();
	/**
	 * Wrapper for rewinddir(3): resets the directory stream.
	 */
	void rewind();
	/**
	 * Wrapper for telldir(3): returns current position in directory stream.
	 * @return current position's value
	 */
	long tell();
	/**
	 * Wrapper for seekdir(3): sets the current position in directory stream.
	 * @param loc new position's value
	 */
	void seek(long loc);
	/**
	 * Wrapper for dirfd(3): returns the directory identifier.
	 * @return directory's file descriptor
	 */
	int getDescriptor();
	//-----------------------------------------------------------------
	/**
	 * Wrapper for opendir(3): opens a directory (replaces the existing one).
	 * @param name directory's pathname
	 */
	void open(const char *name);
	/**
	 * Wrapper for fopendir(3): opens a directory (replaces the existing one).
	 * @param fd directory's file descriptor
	 */
	void open(int fd);
	/**
	 * Wrapper for closedir(3): closes a directory.
	 * @return true on success, false on system error
	 */
	bool close() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for scandir(3): scans a directory for matching entries.
	 * @param name directory's pathname
	 * @param filter filter function (nullptr to select all)
	 * @param cmp comparator function for sorting
	 * @param count stores the selected directory entries count
	 * @return directory entries' list (can be nullptr)
	 */
	static dirent** scan(const char *name, int (*filter)(const dirent*),
			int (*cmp)(const dirent**, const dirent**), int &count);
	/**
	 * Wrapper for scandirat(3): scans a directory for matching entries.
	 * @param fd base file descriptor
	 * @param name relative or absolute pathname
	 * @param filter filter function (nullptr to select all)
	 * @param cmp comparator function for sorting
	 * @param count stores the selected directory entries count
	 * @return directory entries' list (can be nullptr)
	 */
	static dirent** scan(int fd, const char *name, int (*filter)(const dirent*),
			int (*cmp)(const dirent**, const dirent**), int &count);
	/**
	 * Wrapper for nftw(3): walks through the directory tree.
	 * @param path the base pathname
	 * @param fn callback function
	 * @param descriptors the maximum number of directories to hold open
	 * @param flag bitmap of flags
	 * @return callback function's return value
	 */
	static int walk(const char *path,
			int (*fn)(const char*, const struct stat*, int, FTW*),
			int descriptors, int flag);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for mkdir(2): creates a new directory.
	 * @param path pathname of the new directory
	 * @param mode bitmap of mode flags
	 */
	static void create(const char *path, mode_t mode);
	/**
	 * Wrapper for mkdirat(2): creates a new directory.
	 * @param dirfd base file descriptor
	 * @param path relative or absolute pathname
	 * @param mode bitmap of mode flags
	 */
	static void create(int dirfd, const char *path, mode_t mode);
	/**
	 * Wrapper for rmdir(2): deletes an empty directory.
	 * @param path directory's pathname
	 */
	static void remove(const char *path);
private:
	DIR *dir;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_DIRECTORY_H_ */
