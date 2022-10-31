/*
 * Descriptor.h
 *
 * Resource descriptor
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_REACTOR_DESCRIPTOR_H_
#define WH_REACTOR_DESCRIPTOR_H_
#include "../base/Timer.h"
#include "../base/ds/State.h"
#include "../base/ds/UID.h"
#include "../base/unix/File.h"
#include <sys/uio.h>

namespace wanhive {
/**
 * File descriptor manager
 */
class Descriptor: public State, private File {
public:
	/**
	 * Default constructor: assigns an invalid file descriptor.
	 */
	Descriptor() noexcept;
	/**
	 * Constructor: assigns a file descriptor.
	 * @param fd file descriptor
	 */
	Descriptor(int fd) noexcept;
	/**
	 * Destructor: closes the managed file descriptor.
	 */
	~Descriptor();
	//-----------------------------------------------------------------
	/**
	 * Returns a unique identifier.
	 * @return current unique identifier
	 */
	unsigned long long getUid() const noexcept;
	/**
	 * Sets a unique identifier.
	 * @param uid new unique identifier
	 */
	void setUid(unsigned long long uid) noexcept;
	/**
	 * Checks whether this object has outlived the given duration.
	 * @param timeOut timeout value in milliseconds
	 * @return true on timeout, false otherwise
	 */
	bool hasTimedOut(unsigned int timeOut) const noexcept;
protected:
	/**
	 * Returns the managed file descriptor (call Descriptor::closeHandle() to
	 * close the file descriptor, don't use the close() system call).
	 * @return the managed file descriptor (can be invalid)
	 */
	int getHandle() const noexcept;
	/**
	 * Replaces the managed file descriptor (closes the previously managed
	 * file descriptor).
	 * @param fd new file descriptor
	 */
	void setHandle(int fd) noexcept;
	/**
	 * Returns the managed file descriptor and releases its ownership.
	 * @return the released file descriptor (can be invalid)
	 */
	int releaseHandle() noexcept;
	/**
	 * Closes and invalidates the managed file descriptor.
	 * @return true on success, false on system error
	 */
	bool closeHandle() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether an IO operation is pending on this object. Checks the
	 * following conditions:
	 * 1. IO error or peer shutdown reported
	 * 2. Read event reported
	 * 3. Write event reported and outgoing data is queued up
	 * @param outgoing true if the object has outgoing data, false otherwise.
	 * @return true if an IO operation is pending, false otherwise
	 */
	bool isReady(bool outgoing) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether the managed file descriptor is in blocking mode.
	 * @return true if the managed file descriptor is in blocking mode, false
	 * if non-blocking mode is enabled.
	 */
	bool isBlocking();
	/**
	 * Configures managed file descriptor's blocking mode.
	 * @param block true for blocking mode, false for non-blocking mode
	 */
	void setBlocking(bool block);
	/**
	 * Scatter read operation.
	 * @param vectors scatter input buffers
	 * @param count input buffers count
	 * @return the number of bytes read on success (possibly 0), -1 on EOF (end
	 * of file) and 0 if the file descriptor is non-blocking and the operation
	 * would block.
	 */
	ssize_t readv(const iovec *vectors, unsigned int count);
	/**
	 * Reads from the managed file descriptor.
	 * @param buffer input buffer
	 * @param count the maximum number of bytes to read
	 * @return the number of bytes read on success (possibly 0), -1 on EOF (end
	 * of file) and 0 if the file descriptor is non-blocking and the operation
	 * would block.
	 */
	ssize_t read(void *buffer, size_t count);
	/**
	 * Gather write operation.
	 * @param vectors gather output buffers
	 * @param count output buffers count
	 * @return the number of bytes written on success (possibly 0), and 0 if the
	 * file descriptor is non-blocking and the operation would block.
	 */
	ssize_t writev(const iovec *vectors, unsigned int count);
	/**
	 * Writes to the managed file descriptor.
	 * @param buffer output buffer
	 * @param count the maximum number of bytes to write
	 * @return the number of bytes written on success (possibly 0), and 0 if the
	 * file descriptor is non-blocking and the operation would block.
	 */
	ssize_t write(const void *buffer, size_t count);
private:
	UID uid;
	Timer timer;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_DESCRIPTOR_H_ */
