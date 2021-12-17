/*
 * FileSystem.h
 *
 * File-system operations
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_FILESYSTEM_H_
#define WH_BASE_UNIX_FILESYSTEM_H_
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace wanhive {
/**
 * File-system operations
 */
class FileSystem {
public:
	//Wrapper for access(2): checks user's permissions for a file
	static bool access(const char *path, int mode) noexcept;
	//Wrapper for faccessat(2): checks user's permissions for a file
	static bool access(int dirfd, const char *path, int mode, int flag) noexcept;

	//Wrapper for umask(2): sets file mode creation mask
	static mode_t umask(mode_t mask) noexcept;

	//Wrapper for chmod(2): changes file permissions
	static void chmod(const char *path, mode_t mode);
	//Wrapper for fchmod(2): changes file permissions
	static void chmod(int fd, mode_t mode);
	//Wrapper for fchmodat(2): changes file permissions
	static void chmod(int dirfd, const char *path, mode_t mode, int flag);

	//Wrapper for chown(2) and lchown(2): changes ownership of a file
	static void chown(const char *path, uid_t owner, gid_t group, bool symLink =
			false);
	//Wrapper for fchown(2): changes ownership of a file
	static void chown(int fd, uid_t owner, gid_t group);
	//Wrapper for fchownat(2): changes ownership of a file
	static void chown(int dirfd, const char *path, uid_t owner, gid_t group,
			int flag);

	//Wrapper for truncate(2): truncates a file to a specified length
	static void truncate(const char *path, off_t length);
	//Wrapper for ftruncate(2): truncates a file to a specified length
	static void truncate(int fd, off_t length);

	//Wrapper for link(2): creates a hard link
	static void link(const char *oldPath, const char *newPath);
	//Wrapper for linkat(2): creates a hard link
	static void link(int oldDirfd, const char *oldPath, int newDirfd,
			const char *newPath, int flag);

	//Wrapper for unlink(2): deletes a name from the filesystem
	static void unlink(const char *path);
	//Wrapper for unlinkat(2): deletes a name from the filesystem
	static void unlink(int dirfd, const char *path, int flag);

	//Wrapper for remove(3); removes a file or directory
	static void remove(const char *path);

	//Wrapper for rename(2): changes the name or location of a file
	static void rename(const char *oldPath, const char *newPath);
	//Wrapper for renameat2(3) [fallback: renameat(2)]: renames a file
	static void rename(int oldDirfd, const char *oldPath, int newDirfd,
			const char *newPath, unsigned int flag = 0);

	//Wrapper for symlink(2): creates  a  symbolic  link
	static void symLink(const char *target, const char *linkPath);
	//Wrapper for symlinkat(2): creates  a  symbolic  link
	static void symLink(int dirfd, const char *target, const char *linkPath);

	//Wrapper for readlink(2): reads the value of a symbolic link
	static size_t readLink(const char *path, char *buf, size_t size);
	//Wrapper for readlinkat(2): reads the value of a symbolic link
	static size_t readLink(int dirfd, const char *path, char *buf, size_t size);

	//Wrapper for futimens(2): changes file timestamps
	static void setTimestamp(int fd, const timespec times[2]);
	//Wrapper for utimensat(2): changes file timestamps
	static void setTimestamp(int dirfd, const char *path,
			const timespec times[2], int flag);

	//Wrapper for dirname(3)
	static const char* directoryName(char *path) noexcept;
	//Wrapper for basename(3)
	static const char* baseName(char *path) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FILESYSTEM_H_ */
