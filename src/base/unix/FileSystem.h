/**
 * @file FileSystem.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * File-system operations
 */
class FileSystem {
public:
	/**
	 * Wrapper for access(2): checks user's permissions for a file.
	 * @param path file's pathname
	 * @param mode access mode bits
	 * @return true on success, false on error
	 */
	static bool access(const char *path, int mode) noexcept;
	/**
	 * Wrapper for faccessat(2): checks user's permissions for a file.
	 * @param dirfd base path's descriptor
	 * @param path relative or absolute pathname
	 * @param mode access mode bits
	 * @param flag operation flags
	 * @return true on success, false on error
	 */
	static bool access(int dirfd, const char *path, int mode, int flag) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for umask(2): sets file mode creation mask.
	 * @param mask mode bits
	 * @return previous value of mask
	 */
	static mode_t umask(mode_t mask) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for chmod(2): changes file permissions.
	 * @param path file's pathname
	 * @param mode permission bits
	 */
	static void chmod(const char *path, mode_t mode);
	/**
	 * Wrapper for fchmod(2): changes file permissions.
	 * @param fd file descriptor
	 * @param mode permission bits
	 */
	static void chmod(int fd, mode_t mode);
	/**
	 * Wrapper for fchmodat(2): changes file permissions.
	 * @param dirfd base path's descriptor
	 * @param path relative or absolute pathname
	 * @param mode permission bits
	 * @param flag operation flags
	 */
	static void chmod(int dirfd, const char *path, mode_t mode, int flag);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for chown(2) and lchown(2): changes ownership of a file.
	 * @param path file's pathname
	 * @param owner new owner's identifier
	 * @param group new group's identifier
	 * @param symLink true to follow a symbolic link, false otherwise
	 */
	static void chown(const char *path, uid_t owner, gid_t group, bool symLink =
			false);
	/**
	 * Wrapper for fchown(2): changes ownership of a file.
	 * @param fd file descriptor
	 * @param owner new owner's identifier
	 * @param group new group's identifier
	 */
	static void chown(int fd, uid_t owner, gid_t group);
	/**
	 * Wrapper for fchownat(2): changes ownership of a file.
	 * @param dirfd base path's descriptor
	 * @param path relative or absolute pathname
	 * @param owner new owner's identifier
	 * @param group new group's identifier
	 * @param flag operation flags
	 */
	static void chown(int dirfd, const char *path, uid_t owner, gid_t group,
			int flag);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for truncate(2): truncates a file to a specified length.
	 * @param path file's pathname
	 * @param length file's size (bytes) after truncation
	 */
	static void truncate(const char *path, off_t length);
	/**
	 * Wrapper for ftruncate(2): truncates a file to a specified length.
	 * @param fd file descriptor
	 * @param length file's size (bytes) after truncation
	 */
	static void truncate(int fd, off_t length);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for link(2): creates a hard link.
	 * @param oldPath existing pathname
	 * @param newPath additional pathname
	 */
	static void link(const char *oldPath, const char *newPath);
	/**
	 * Wrapper for linkat(2): creates a hard link.
	 * @param oldDirfd old base path descriptor
	 * @param oldPath existing pathname
	 * @param newDirfd new base path descriptor
	 * @param newPath additional pathname
	 * @param flag operation flags
	 */
	static void link(int oldDirfd, const char *oldPath, int newDirfd,
			const char *newPath, int flag);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for unlink(2): deletes a name from the filesystem.
	 * @param path file's pathname
	 */
	static void unlink(const char *path);
	/**
	 * Wrapper for unlinkat(2): deletes a name from the filesystem.
	 * @param dirfd base path descriptor
	 * @param path relative or absolute pathname
	 * @param flag operation flags
	 */
	static void unlink(int dirfd, const char *path, int flag);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for remove(3); removes a file or directory.
	 * @param path pathname for removal
	 */
	static void remove(const char *path);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for rename(2): changes the name or location of a file.
	 * @param oldPath old pathname
	 * @param newPath new pathname
	 */
	static void rename(const char *oldPath, const char *newPath);
	/**
	 * Wrapper for renameat2(2) [fallback: renameat(2)]: renames a file
	 * @param oldDirfd old base path descriptor
	 * @param oldPath old pathname
	 * @param newDirfd new base path descriptor
	 * @param newPath new pathname
	 * @param flag operation flags
	 */
	static void rename(int oldDirfd, const char *oldPath, int newDirfd,
			const char *newPath, unsigned int flag = 0);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for symlink(2): creates  a  symbolic  link.
	 * @param target target file's pathname
	 * @param linkPath symbolic link's pathname
	 */
	static void symLink(const char *target, const char *linkPath);
	/**
	 * Wrapper for symlinkat(2): creates  a  symbolic  link.
	 * @param dirfd symbolic link's base path descriptor
	 * @param target target file's pathname
	 * @param linkPath symbolic link's pathname
	 */
	static void symLink(int dirfd, const char *target, const char *linkPath);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for readlink(2): reads a symbolic link's value.
	 * @param path symbolic link's pathname
	 * @param buf stores the symbolic link's contents
	 * @param size buffer's size in bytes
	 * @return number of bytes placed in the buffer
	 */
	static size_t readLink(const char *path, char *buf, size_t size);
	/**
	 * Wrapper for readlinkat(2): reads the value of a symbolic link.
	 * @param dirfd symbolic link's base path descriptor
	 * @param path symbolic link's pathname
	 * @param buf stores the symbolic link's contents
	 * @param size buffer's size in bytes
	 * @return number of bytes placed in the buffer
	 */
	static size_t readLink(int dirfd, const char *path, char *buf, size_t size);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for futimens(2): changes file timestamps.
	 * @param fd file descriptor
	 * @param times timestamp values
	 */
	static void setTimestamp(int fd, const timespec times[2]);
	/**
	 * Wrapper for utimensat(2): changes file timestamps.
	 * @param dirfd base path descriptor
	 * @param path relative or absolute pathname
	 * @param times timestamp values
	 * @param flag operation flags
	 */
	static void setTimestamp(int dirfd, const char *path,
			const timespec times[2], int flag);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for dirname(3): strips last component from the file name.
	 * @param path valid pathname
	 * @return pointer to nul-terminated string
	 */
	static const char* directoryName(char *path) noexcept;
	/**
	 * Wrapper for basename(3): strips directory and suffix from file name.
	 * @param path valid pathname
	 * @return pointer to nul-terminated string
	 */
	static const char* baseName(char *path) noexcept;
	/**
	 * Wrapper for realpath(3): returns the canonicalized absolute pathname.
	 * @param path valid pathname
	 * @param resolved buffer for resolved pathname
	 * @return pointer to resolved pathname
	 */
	static char* realPath(const char *path, char *resolved);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FILESYSTEM_H_ */
