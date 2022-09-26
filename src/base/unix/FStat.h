/*
 * FStat.h
 *
 * File status information
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_FSTAT_H_
#define WH_BASE_UNIX_FSTAT_H_
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace wanhive {
/**
 * File status information
 */
class FStat {
public:
	/**
	 * Default constructor: creates an empty object.
	 */
	FStat() noexcept;
	/**
	 * Constructor: creates information about a file.
	 * @param fd file descriptor
	 */
	FStat(int fd);
	/**
	 * Constructor: creates information about a file.
	 * @param path file's pathname
	 * @param symLink true to follow a symbolic link, false otherwise
	 */
	FStat(const char *path, bool symLink = false);
	/**
	 * Constructor: creates information about a file.
	 * @param dirfd base path's descriptor
	 * @param path relative or absolute pathname
	 * @param flags operation flags
	 */
	FStat(int dirfd, const char *path, int flags);
	/**
	 * Destructor
	 */
	~FStat();
	//-----------------------------------------------------------------
	/**
	 * Checks regular file type.
	 * @return true on match, false otherwise
	 */
	bool isRegularFile() const noexcept;
	/**
	 * Checks directory file type.
	 * @return true on match, false otherwise
	 */
	bool isDirectory() const noexcept;
	/**
	 * Checks character device file type.
	 * @return true on match, false otherwise
	 */
	bool isCharSpecialFile() const noexcept;
	/**
	 * Checks block device file type.
	 * @return true on match, false otherwise
	 */
	bool isBlockSpecialFile() const noexcept;
	/**
	 * Checks fifo file type.
	 * @return true on match, false otherwise
	 */
	bool isFifo() const noexcept;
	/**
	 * Checks symbolic link file type.
	 * @return true on match, false otherwise
	 */
	bool isSymbolicLink() const noexcept;
	/**
	 * Checks socket file type.
	 * @return true on match, false otherwise
	 */
	bool isSocket() const noexcept;
	/**
	 * IPC: checks message queue file type.
	 * @return true on match, false otherwise
	 */
	bool isMessageQueue();
	/**
	 * IPC: checks semaphore file type.
	 * @return true on match, false otherwise
	 */
	bool isSemaphore();
	/**
	 * IPC: checks shared memory file type.
	 * @return true on match, false otherwise
	 */
	bool isSharedMemory();
	//-----------------------------------------------------------------
	/**
	 * Checks the set-user-id bit.
	 * @return true on match, false otherwise
	 */
	bool isSetUID() const noexcept;
	/**
	 * Checks the set-group-id bit.
	 * @return true on match, false otherwise
	 */
	bool isSetGID() const noexcept;
	/**
	 * Tests (bitwise) the file permission bits.
	 * @param mode permission bits
	 * @return true if a bit is set, false otherwise
	 */
	bool access(mode_t mode) const noexcept;
	/**
	 * Returns the file permission bits.
	 * @return permission bits
	 */
	mode_t access() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the file size.
	 * @return file size in bytes
	 */
	off_t size() const noexcept;
	/**
	 * Returns owner ID of a file.
	 * @return owner's identifier
	 */
	uid_t ownerId() const noexcept;
	/**
	 * Returns group ID of the file
	 * @return group's identifier
	 */
	gid_t groupId() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns major device number of the file system.
	 * @return major device identifier
	 */
	unsigned int majorId() const noexcept;
	/**
	 * Returns major device number of a character/block special device.
	 * @return major device identifier
	 */
	unsigned int majorDeviceId() const noexcept;
	/**
	 * Returns minor device number of the file system.
	 * @return minor device identifier
	 */
	unsigned int minorId() const noexcept;
	/**
	 * Returns minor device ID of a character/block special device.
	 * @return minor device identifier
	 */
	unsigned int minorDeviceId() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the last file data access time.
	 * @return timestamp's value
	 */
	const timespec& accessedOn() const noexcept;
	/**
	 * Returns the last file data modification time
	 * @return timestamp's value
	 */
	const timespec& modifiedOn() const noexcept;
	/**
	 * Returns the last inode status change time.
	 * @return timestamp's value
	 */
	const timespec& statusChangedOn() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns pointer to the internal object.
	 * @return pointer to stat structure
	 */
	struct stat* get() noexcept;
	/**
	 * Constant pointer to the internal object.
	 * @return pointer to stat structure
	 */
	const struct stat* get() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for fstat(2): gets file status.
	 * @param fd file descriptor
	 */
	void read(int fd);
	/**
	 * Wrapper for stat(2) and lstat(2): gets file status
	 * @param path file's pathname
	 * @param symLink true to follow a symbolic link, false otherwise
	 */
	void read(const char *path, bool symLink);
	/**
	 * Wrapper for fstatat(2): gets file status.
	 * @param dirfd base path's descriptor
	 * @param path relative or absolute pathname
	 * @param flags operation flags
	 */
	void read(int dirfd, const char *path, int flags);
	/**
	 * Clears (zeroes-out) the file status structure.
	 */
	void clear() noexcept;
private:
	struct stat _stat;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FSTAT_H_ */
