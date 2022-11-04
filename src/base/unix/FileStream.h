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
	 * Constructor: opens a stream.
	 * @param path file's pathname
	 * @param mode mode string
	 */
	FileStream(const char *path, const char *mode);
	/**
	 * Constructor: opens a stream.
	 * @param fd file descriptor
	 * @param mode mode string
	 */
	FileStream(int fd, const char *mode);
	/**
	 * Destructor: closes the stream.
	 */
	~FileStream();
	//-----------------------------------------------------------------
	/**
	 * Returns the managed stream.
	 * @return stream pointer
	 */
	FILE* get() const noexcept;
	/**
	 * Replaces the managed stream (closes the previously managed stream).
	 * @param fp new stream's pointer
	 */
	void set(FILE *fp) noexcept;
	/**
	 * Returns the managed stream and releases its ownership.
	 * @return stream pointer
	 */
	FILE* release() noexcept;
	/**
	 * Wrapper for fclose(3): flushes and closes the managed stream.
	 * @return true on success, false on error
	 */
	bool close() noexcept;
	//-----------------------------------------------------------------
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
	 * Wrapper for freopen(3): reopens the managed stream.
	 * @param path file's pathname
	 * @param mode mode string
	 */
	void reopen(const char *path, const char *mode);
private:
	FILE *fp { nullptr };
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FILESTREAM_H_ */
