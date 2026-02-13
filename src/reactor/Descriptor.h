/**
 * @file Descriptor.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * File descriptor manager
 */
class Descriptor: public State, private File {
public:
	/**
	 * Constructor: assigns an invalid file descriptor.
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
	 * @return unique identifier
	 */
	unsigned long long getUid() const noexcept;
	/**
	 * Sets a unique identifier.
	 * @param uid unique identifier
	 */
	void setUid(unsigned long long uid) noexcept;
	/**
	 * Checks if the object has outlived the specified duration.
	 * @param duration value in milliseconds
	 * @return true if exceeded, false otherwise
	 */
	bool aged(unsigned int duration) const noexcept;
protected:
	//-----------------------------------------------------------------
	using File::get;
	using File::set;
	using File::release;
	using File::close;
	//-----------------------------------------------------------------
	/**
	 * Checks if an I/O operation is pending on this object by evaluating:
	 * 1. An I/O error or peer shutdown
	 * 2. A read event
	 * 3. A write event with outgoing data
	 * @param outgoing true if outgoing data is available, false otherwise
	 * @return true if an IO operation is pending; false otherwise
	 */
	bool isReady(bool outgoing) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks if the managed file descriptor is in blocking mode.
	 * @return true if blocking mode is enabled; false if non-blocking
	 */
	bool isBlocking();
	/**
	 * Configures the blocking mode of a managed file descriptor.
	 * @param block true to enable blocking mode; false for non-blocking
	 */
	void setBlocking(bool block);
	/**
	 * Performs a scatter read operation.
	 * @param vectors input buffers
	 * @param count input buffers count
	 * @return bytes read on success (possibly 0), -1 on EOF (end of file), 0 if
	 * a non-blocking operation would block.
	 */
	ssize_t readv(const iovec *vectors, unsigned int count);
	/**
	 * Reads from the managed file descriptor.
	 * @param buffer input buffer
	 * @param count the maximum bytes to write
	 * @return bytes read on success (possibly 0), -1 on EOF (end of file), 0 if
	 * a non-blocking operation would block.
	 */
	ssize_t read(void *buffer, size_t count);
	/**
	 * Performs a gather write operation.
	 * @param vectors output buffers
	 * @param count output buffers count
	 * @return bytes written on success (could be 0), or 0 if a non-blocking
	 * operation would block.
	 */
	ssize_t writev(const iovec *vectors, unsigned int count);
	/**
	 * Writes to the managed file descriptor.
	 * @param buffer the data to write
	 * @param count the maximum bytes to write
	 * @return bytes written on success (could be 0), or 0 if a non-blocking
	 * operation would block.
	 */
	ssize_t write(const void *buffer, size_t count);
private:
	UID uid;
	Timer timer;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_DESCRIPTOR_H_ */
