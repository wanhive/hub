/*
 * Stream.h
 *
 * Byte stream watcher
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_STREAM_H_
#define WH_HUB_STREAM_H_
#include "../reactor/Watcher.h"
#include "../base/common/Sink.h"
#include "../base/common/Source.h"
#include "../base/ds/StaticCircularBuffer.h"

namespace wanhive {
/**
 * Byte stream watcher
 * @note For lightweight real-time data streams
 */
class Stream final: public Sink<unsigned char>,
		public Source<unsigned char>,
		public Watcher {
public:
	/**
	 * Constructor: assigns a file descriptor.
	 * @param fd file descriptor
	 */
	Stream(int fd) noexcept;
	/**
	 * Destructor
	 */
	~Stream();
	//-----------------------------------------------------------------
	/*
	 * Sink interface implementation
	 */
	size_t ingest(const unsigned char *src, size_t count) noexcept override;
	bool ingest(const unsigned char &src) noexcept override;
	size_t space() const noexcept override;
	//-----------------------------------------------------------------
	/*
	 * Source interface implementation
	 */
	size_t emit(unsigned char *dest, size_t count) noexcept override;
	bool emit(unsigned char &dest) noexcept override;
	size_t available() const noexcept override;
	//-----------------------------------------------------------------
	/*
	 * Watcher interface implementation
	 */
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Reads incoming bytes from the managed stream.
	 * @return the number of bytes read on success (possible zero(0) if the
	 * internal buffer is full), zero(0) if the stream is non-blocking and
	 * the read operation would block, -1 if the stream was closed cleanly.
	 */
	ssize_t read();
	/**
	 * Writes data to the managed stream.
	 * @return the number of bytes written, possibly zero (0) if the outgoing
	 * data queue is empty.
	 */
	ssize_t write();
private:
	static constexpr unsigned IO_SIZE = 256;
	StaticCircularBuffer<unsigned char, IO_SIZE> in;
	StaticCircularBuffer<unsigned char, IO_SIZE> out;
};

} /* namespace wanhive */

#endif /* WH_HUB_STREAM_H_ */
