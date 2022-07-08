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
	//Creates an empty object (call read later)
	FStat() noexcept;
	//Creates information about a file
	FStat(int fd);
	//Creates information about a file
	FStat(const char *path, bool symLink = false);
	//Creates information about a file
	FStat(int dirfd, const char *path, int flags);
	~FStat();

	//File type is a regular file
	bool isRegularFile() const noexcept;
	//File type is a directory
	bool isDirectory() const noexcept;
	//File type is a character device file
	bool isCharSpecialFile() const noexcept;
	//File type is a block device file
	bool isBlockSpecialFile() const noexcept;
	//File type is a fifo
	bool isFifo() const noexcept;
	//File type is a symbolic link
	bool isSymbolicLink() const noexcept;
	//File type is a socket
	bool isSocket() const noexcept;

	//IPC: file type is message queue
	bool isMessageQueue();
	//IPC: file type is semaphore
	bool isSemaphore();
	//IPC: file type is shared memory
	bool isSharedMemory();

	//Set-user-id bit is set
	bool isSetUID() const noexcept;
	//Set-group-id bit is set
	bool isSetGID() const noexcept;
	//Tests the permission bits
	bool access(mode_t mode) const noexcept;

	//File size in bytes
	off_t size() const noexcept;
	//Owner ID of the file
	uid_t ownerId() const noexcept;
	//Group ID of the file
	gid_t groupId() const noexcept;

	//The major device number of the file system
	unsigned int majorId() const noexcept;
	//The major device ID (if character/block special device)
	unsigned int majorDeviceId() const noexcept;
	//The minor device number of the file system
	unsigned int minorId() const noexcept;
	//The minor device ID (if character/block special device)
	unsigned int minorDeviceId() const noexcept;

	//Last file data access time
	const timespec& accessedOn() const noexcept;
	//Last file data modification time
	const timespec& modifiedOn() const noexcept;
	//Last inode status change time
	const timespec& statusChangedOn() const noexcept;

	//Pointer to the internal object
	struct stat* get() noexcept;
	//Constant pointer to the internal object
	const struct stat* get() const noexcept;

	//Wrapper for fstat(2): gets file status
	void read(int fd);
	//Wrapper for stat(2) and lstat(2): gets file status
	void read(const char *path, bool symLink);
	//Wrapper for fstatat(2): gets file status
	void read(int dirfd, const char *path, int flags);

	//Clears (zeroes-out) the file status structure
	void clear() noexcept;
private:
	struct stat _stat;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FSTAT_H_ */
