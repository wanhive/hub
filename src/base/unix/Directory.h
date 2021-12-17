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
	//Creates an empty object (call open later)
	Directory() noexcept;
	//Opens the directory <name>
	Directory(const char *name);
	//Opens the directory associated with the file descriptor <fd>
	Directory(int fd);
	virtual ~Directory();

	//Wrapper for readdir(3): reads a directory entry at current position
	dirent* read();
	//Wrapper for rewinddir(3): resets the directory stream
	void rewind();
	//Wrapper for telldir(3): returns current position in directory stream
	long tell();
	//Wrapper for seekdir(3): sets the current position in directory stream
	void seek(long loc);
	//Wrapper for dirfd(3): returns the directory identifier
	int getDescriptor();

	//Wrapper for opendir(3): opens a directory
	void open(const char *name);
	//Wrapper for fopendir(3): opens a directory
	void open(int fd);
	//Wrapper for closedir(3): closes a directory
	bool close() noexcept;

	//Wrapper for scandir(3): scans a directory for matching entries
	static dirent** scan(const char *name, int (*filter)(const dirent*),
			int (*cmp)(const dirent**, const dirent**), int &count);
	//Wrapper for scandirat(3): scans a directory for matching entries
	static dirent** scan(int fd, const char *name, int (*filter)(const dirent*),
			int (*cmp)(const dirent**, const dirent**), int &count);

	//Wrapper for nftw(3): walks through the directory tree
	static int walk(const char *path,
			int (*fn)(const char*, const struct stat*, int, FTW*),
			int descriptors, int flag);

	//Wrapper for mkdir(2): creates a directory
	static void create(const char *path, mode_t mode);
	//Wrapper for mkdirat(2): creates a directory
	static void create(int dirfd, const char *path, mode_t mode);
	//Wrapper for rmdir(2): deletes a directory
	static void remove(const char *path);
private:
	DIR *dir;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_DIRECTORY_H_ */
