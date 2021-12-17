/*
 * FileStream.h
 *
 * File stream management
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
 * File stream management
 */
class FileStream {
public:
	FileStream() noexcept;
	FileStream(FILE *fp) noexcept;
	FileStream(const char *path, const char *mode);
	FileStream(int fd, const char *mode);
	virtual ~FileStream();

	//Returns the stream associated with this object
	FILE* get() const noexcept;
	//Sets a stream after closing the existing one
	void set(FILE *fp) noexcept;
	//Releases the stream associated with this object
	FILE* release() noexcept;
	//Wrapper for fclose(3): flushes and closes the stream
	bool close() noexcept;

	//Wrapper for fopen(3): opens a file stream
	void open(const char *path, const char *mode);
	//Wrapper for fdopen(3): associates the stream with the given descriptor
	void open(int fd, const char *mode);
	//Wrapper for freopen(3): reopens the underlying file stream
	void reopen(const char *path, const char *mode);
private:
	FILE *fp;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_FILESTREAM_H_ */
