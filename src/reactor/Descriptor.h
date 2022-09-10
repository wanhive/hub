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
 * Abstraction of file descriptors
 */
class Descriptor: public State, private File {
public:
	/**
	 * Default constructor: assigns an invalid file descriptor.
	 */
	Descriptor() noexcept;
	/**
	 * Constructor: assigns a file descriptor to this object.
	 * @param fd the file descriptor
	 */
	Descriptor(int fd) noexcept;
	/**
	 * Destructor: automatically closes the associated file descriptor.
	 */
	~Descriptor();
	//-----------------------------------------------------------------
	/**
	 * Returns the unique identifier.
	 * @return the current unique identifier
	 */
	unsigned long long getUid() const noexcept;
	/**
	 * Sets a new unique identifier.
	 * @param uid the unique identifier
	 */
	void setUid(unsigned long long uid) noexcept;
	/**
	 * Checks if this object has outlived the given timeout value.
	 * @param timeOut the timeout value in milliseconds
	 * @return true on timeout, false otherwise
	 */
	bool hasTimedOut(unsigned int timeOut) const noexcept;
protected:
	/**
	 * Returns the associated file descriptor (call Descriptor::closeHandle() to
	 * close the associated file descriptor, don't use the close() system call).
	 * @return the associated file descriptor (can be invalid)
	 */
	int getHandle() const noexcept;
	/**
	 * Sets a new file descriptor after closing the existing one.
	 * @param fd the new file descriptor
	 */
	void setHandle(int fd) noexcept;
	/**
	 * Releases and returns the associated file descriptor. This object no
	 * longer owns the file descriptor and hence the file will not be closed when
	 * this object's destructor is called.
	 * @return the released file descriptor (can be invalid)
	 */
	int releaseHandle() noexcept;
	/**
	 * Closes and invalidates the associated file descriptor.
	 * @return true on success, false on system error.
	 */
	bool closeHandle() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks if an IO operation is pending on this object:
	 * 1. IO error or peer shutdown reported
	 * 2. Read event reported
	 * 3. Write event reported and outgoing data is queued up
	 * @param outgoing true if the object has outgoing data, false otherwise.
	 * @return true if an IO operation is pending, false otherwise
	 */
	bool isReady(bool outgoing) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks the blocking IO mode.
	 * @return true if the underlying file descriptor has been configured for
	 * blocking IO, false if the file descriptor has been configured for non-
	 * blocking IO.
	 */
	bool isBlocking();
	/**
	 * Sets the blocking IO mode.
	 * @param block true for blocking IO, false for non-blocking IO
	 */
	void setBlocking(bool block);
	/**
	 * Scatter/gather read operation.
	 * @param vector describes the IO buffers for storing the data read from the
	 * associated file descriptor.
	 * @param count number of IO buffers
	 * @return the number of bytes read on success (possibly 0), -1 on EOF
	 * (end of file) and 0 if the file descriptor is non-blocking and the
	 * operation would block.
	 */
	ssize_t readv(const iovec *vector, unsigned int count);
	/**
	 * Reads data from the associated file descriptor into the given buffer.
	 * @param buf describes the buffer for storing the data
	 * @param count the maximum number of bytes to read
	 * @return the number of bytes read on success (possibly 0), -1 on EOF
	 * (end of file) and 0 if the file descriptor is non-blocking and the
	 * operation would block.
	 */
	ssize_t read(void *buf, size_t count);
	/**
	 * Scatter/gather write operation.
	 * @param iov the buffers of data for writing
	 * @param count the number of buffers
	 * @return the number of bytes written on success (possibly 0), and 0 if the
	 * file descriptor is non-blocking and the operation would block.
	 */
	ssize_t writev(const iovec *iov, unsigned int count);
	/**
	 * Writes data from the given buffer to the associated file descriptor.
	 * @param buf describes the data to write
	 * @param count the maximum number of bytes to write
	 * @return the number of bytes written on success (possibly 0), and 0 if the
	 * file descriptor is non-blocking and the operation would block.
	 */
	ssize_t write(const void *buf, size_t count);
private:
	UID uid;
	Timer timer;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_DESCRIPTOR_H_ */
