/*
 * FileStream.h
 *
 * File stream
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_FILESTREAM_H_
#define WH_BASE_UNIX_FILESTREAM_H_
#include <cstdio>

namespace wanhive {
/**
 * File stream
 */
class FileStream {
public:
	/**
	 * Default constructor: creates an empty object.
	 */
	FileStream() noexcept;
	/**
	 * Constructor: assigns a stream.
	 * @param fp stream pointer
	 */
	FileStream(FILE *fp) noexcept;
	/**
	 * Constructor: opens a file.
	 * @param path file's pathname
	 * @param mode mode string
	 */
	FileStream(const char *path, const char *mode);
	/**
	 * Associates the given file descriptor.
	 * @param fd file descriptor
	 * @param mode mode string
	 */
	FileStream(int fd, const char *mode);
	/**
	 * Destructor: closes the associated stream.
	 */
	~FileStream();
	//-----------------------------------------------------------------
	/**
	 * Returns the associated stream.
	 * @return stream pointer
	 */
	FILE* get() const noexcept;
	/**
	 * Sets a stream after closing the existing one.
	 * @param fp new stream pointer
	 */
	void set(FILE *fp) noexcept;
	/**
	 * Releases and returns the associated stream
	 * @return stream pointer
	 */
	FILE* release() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for fclose(3): flushes and closes the stream.
	 * @return true on success, false on error
	 */
	bool close() noexcept;
	/**
	 * Wrapper for fopen(3): opens a file stream.
	 * @param path file's pathname
	 * @param mode mode string
	 */
	void open(const char *path, const char *mode);
	/**
	 * Wrapper for fdopen(3): associates the stream with the given descriptor.
	 * @param fd file descriptor
	 * @param mode mode string
	 */
	void open(int fd, const char *mode);
	/**
	 * Wrapper for freopen(3): reopens the associated stream.
	 * @param path file's pathname
	 * @param mode mode string
	 */
	void reopen(const char *path, const char *mode);
private:
	FILE *fp;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FILESTREAM_H_ */
