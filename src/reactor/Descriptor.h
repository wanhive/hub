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
#include "../base/ds/State.h"
#include "../base/unix/File.h"
#include <sys/uio.h>

namespace wanhive {
/**
 * Abstraction of file descriptors
 * Thread safe at class level
 */
class Descriptor: public State, private File {
public:
	Descriptor() noexcept;
	Descriptor(int fd) noexcept;
	~Descriptor();

	//Set <uid> as the unique identifier
	void setUid(unsigned long long uid) noexcept;
	//Return the unique identifier
	unsigned long long getUid() const noexcept;
	//-----------------------------------------------------------------
	//Set blocking IO state of the file descriptor
	void setBlocking(bool block);
	//Check blocking IO state of the file descriptor
	bool isBlocking();
	//Return the associated file descriptor
	int getHandle() const noexcept;
protected:
	//Set the file descriptor (existing file descriptor is closed)
	void setHandle(int fd) noexcept;
	//Release and return the file descriptor
	int releaseHandle() noexcept;
	//Close and invalidate the file descriptor
	bool closeHandle() noexcept;
	/*
	 * Return true if (false otherwise):
	 * 1. IO error or peer shutdown reported
	 * 2. Read event reported
	 * 3. Write event reported and outgoing data is queued up
	 */
	bool isReady(bool outgoing) const noexcept;
	/*
	 * Read operations return the number of bytes read on success (possibly 0),
	 * -1 on EOF and 0 if the file descriptor is non-blocking and the operation
	 * would block.
	 */
	ssize_t readv(const iovec *vector, unsigned int count);
	ssize_t read(void *buf, size_t count);
	/*
	 * Write operations return the number of bytes written on success (possibly 0),
	 * and 0 if the file descriptor is non-blocking and the operation would block.
	 */
	ssize_t writev(const iovec *iov, unsigned int count);
	ssize_t write(const void *buf, size_t count);
private:
	//UID generator (thread safe)
	static unsigned long long nextUid() noexcept;
public:
	//[9223372036854775808, 18446744073709551615] are used as automatic identifiers
	static constexpr unsigned long long MIN_TMP_ID =
			((unsigned long long) INT64_MAX) + 1;
	static constexpr unsigned long long MAX_TMP_ID = UINT64_MAX;
private:
	//Counter for the UID generator
	static unsigned long long _nextUid;
	//The unique identifier (UID) of this object
	unsigned long long uid;
};

} /* namespace wanhive */

#endif /* WH_REACTOR_DESCRIPTOR_H_ */
