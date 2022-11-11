/*
 * CircularBuffer.h
 *
 * Single producer - single consumer lock free circular buffer
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_CIRCULARBUFFER_H_
#define WH_BASE_DS_CIRCULARBUFFER_H_
#include "CircularBufferVector.h"
#include "Twiddler.h"
#include "../common/Atomic.h"
#include "../common/Exception.h"
#include <cstring>

namespace wanhive {
/**
 * Lock free circular buffer for POD types, read/write operations are real-time
 * safe in single-producer-single-consumer context.
 * @ref Dennis Lang's Ring Buffer (http://landenlabs.com/code/ring/ring.html)
 * @tparam X storage type
 * @tparam ATOMIC true for thread safety, false otherwise
 */
template<typename X, bool ATOMIC = false> class CircularBuffer {
public:
	/**
	 * Default constructor: creates a zero-size empty buffer.
	 */
	CircularBuffer() noexcept;
	/**
	 * Constructor: creates an empty buffer of given size.
	 * @param size buffer's size
	 */
	CircularBuffer(unsigned int size);
	/**
	 * Destructor
	 */
	~CircularBuffer();
	//-----------------------------------------------------------------
	/**
	 * Clears and resizes the buffer (not thread safe).
	 * @param size buffer's new size
	 */
	void initialize(unsigned int size);
	/**
	 * Empties the buffer (not thread safe).
	 */
	void clear() noexcept;
	/**
	 * Returns this buffer's capacity.
	 * @return buffer's capacity
	 */
	unsigned int capacity() const noexcept;
	/**
	 * Returns the number of elements available for reading from the buffer.
	 * @return how much data can be read from the buffer
	 */
	unsigned int readSpace() const noexcept;
	/**
	 * Returns the number of elements which can be written to the buffer.
	 * @return how much data can be written to the buffer
	 */
	unsigned int writeSpace() const noexcept;
	/**
	 * Checks if the buffer is full (cannot add more elements into the buffer).
	 * @return true if the buffer is full, false otherwise
	 */
	bool isFull() const noexcept;
	/**
	 * Checks if the buffer is empty (cannot read values from the buffer).
	 * @return true if the buffer is empty, false otherwise
	 */
	bool isEmpty() const noexcept;
	/**
	 * Gets the status of this buffer.
	 * @return this buffer's status
	 */
	int getStatus() const noexcept;
	/**
	 * Sets the status of this buffer.
	 * @param status the new status value
	 */
	void setStatus(int status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the description of readable linear segments.
	 * @param vector object for storing the information
	 * @return the total readable elements count
	 */
	unsigned int getReadable(CircularBufferVector<X> &vector) noexcept;
	/**
	 * Returns the description of writable linear segments.
	 * @param vector object for storing the information
	 * @return the total number of slots available for writing
	 */
	unsigned int getWritable(CircularBufferVector<X> &vector) noexcept;
	/**
	 * Moves the read index forward.
	 * @param count the number of readable slots to skip
	 */
	void skipRead(unsigned int count) noexcept;
	/**
	 * Move the write index forward.
	 * @param count the number of writable slots to skip
	 */
	void skipWrite(unsigned int count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads one element from the buffer, fails if the buffer is empty.
	 * @param value object for storing the value
	 * @return true on success, false on failure (empty buffer)
	 */
	bool get(X &value) noexcept;
	/**
	 * Reads one element from the buffer and returns its pointer (not thread safe).
	 * @return pointer to the next element, nullptr if the buffer is empty
	 */
	X* get() noexcept;
	/**
	 * Writes an element into the buffer, fails if the buffer is full.
	 * @param value the value to write
	 * @return true on success, false otherwise (buffer full)
	 */
	bool put(X const &value) noexcept;
	/**
	 * Bulk read operation: transfers elements from the buffer into the given
	 * destination and updates the read index.
	 * @param dest memory space for storing the values
	 * @param length maximum number of elements to read
	 * @return the actual number of elements transferred
	 */
	unsigned int read(X *dest, unsigned int length) noexcept;
	/**
	 * Bulk write operation: transfers values into the buffer from the given source
	 * and updates the write index.
	 * @param src source of data
	 * @param length maximum number of element to write
	 * @return actual number of elements copied into the buffer
	 */
	unsigned int write(const X *src, unsigned int length) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a pointer to the backing array.
	 * @return pointer to the backing array
	 */
	X* array() noexcept;
	/**
	 * Returns a constant pointer to the backing array.
	 * @return pointer to the backing array
	 */
	const X* array() const noexcept;
private:
	unsigned int skip(unsigned int index, unsigned int count) const noexcept;
	unsigned int readSpaceInternal(unsigned int r,
			unsigned int w) const noexcept;
	unsigned int writeSpaceInternal(unsigned int r,
			unsigned int w) const noexcept;
	unsigned int isFullInternal(unsigned int r, unsigned int w) const noexcept;
	unsigned int isEmptyInternal(unsigned int r, unsigned int w) const noexcept;
	void getSegmentsInternal(unsigned int index, unsigned int length,
			CircularBufferVector<X> &vector) noexcept;

	unsigned int loadReadIndex() const noexcept {
		if (ATOMIC) {
			return Atomic<>::load(&readIndex);
		} else {
			return readIndex;
		}
	}

	void storeReadIndex(unsigned int val) noexcept {
		if (ATOMIC) {
			Atomic<>::store(&readIndex, val);
		} else {
			readIndex = val;
		}
	}

	unsigned int loadWriteIndex() const noexcept {
		if (ATOMIC) {
			return Atomic<>::load(&writeIndex);
		} else {
			return writeIndex;
		}
	}

	void storeWriteIndex(unsigned int val) noexcept {
		if (ATOMIC) {
			Atomic<>::store(&writeIndex, val);
		} else {
			writeIndex = val;
		}
	}

	//Load-Load barrier:  memory operations below this barrier remain below it.
	void acquireBarrier() const noexcept {
		if (ATOMIC) {
			Atomic<>::threadFence(MO_ACQUIRE);
		}
	}

	//Store-Store barrier: memory operations above this barrier remain above it.
	void releaseBarrier() const noexcept {
		if (ATOMIC) {
			Atomic<>::threadFence(MO_RELEASE);
		}
	}
private:
	WH_POD_ASSERT(X);
	X *storage { nullptr };
	unsigned int _size { 0 };
	unsigned int _capacity { 0 };
	unsigned int writeIndex { 0 };
	unsigned int readIndex { 0 };
	int status { 0 };
};

} /* namespace wanhive */

template<typename X, bool ATOMIC>
wanhive::CircularBuffer<X, ATOMIC>::CircularBuffer() noexcept {
	clear();
}

template<typename X, bool ATOMIC>
wanhive::CircularBuffer<X, ATOMIC>::CircularBuffer(unsigned int size) {
	initialize(size);
}

template<typename X, bool ATOMIC>
wanhive::CircularBuffer<X, ATOMIC>::~CircularBuffer() {
	delete[] storage;
}

template<typename X, bool ATOMIC>
void wanhive::CircularBuffer<X, ATOMIC>::initialize(unsigned int size) {
	try {
		size = Twiddler::power2Ceil(size);
		delete[] storage;
		storage = new X[size];
		_size = size;
		//capacity of buffer is (total size)-1
		_capacity = size ? (size - 1) : size;
		clear();
	} catch (...) {
		storage = nullptr;
		_size = 0;
		_capacity = 0;
		clear();
		throw Exception(EX_MEMORY);
	}
}

template<typename X, bool ATOMIC>
void wanhive::CircularBuffer<X, ATOMIC>::clear() noexcept {
	storeReadIndex(0);
	storeWriteIndex(0);
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::capacity() const noexcept {
	return _capacity;
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::readSpace() const noexcept {
	return readSpaceInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::writeSpace() const noexcept {
	return writeSpaceInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, bool ATOMIC>
bool wanhive::CircularBuffer<X, ATOMIC>::isFull() const noexcept {
	return isFullInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, bool ATOMIC>
bool wanhive::CircularBuffer<X, ATOMIC>::isEmpty() const noexcept {
	return isEmptyInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, bool ATOMIC>
int wanhive::CircularBuffer<X, ATOMIC>::getStatus() const noexcept {
	if (ATOMIC) {
		return Atomic<int>::load(&status);
	} else {
		return status;
	}
}

template<typename X, bool ATOMIC>
void wanhive::CircularBuffer<X, ATOMIC>::setStatus(int status) noexcept {
	if (ATOMIC) {
		Atomic<int>::store(&this->status, status);
	} else {
		this->status = status;
	}
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::getReadable(
		CircularBufferVector<X> &vector) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();

	auto space = readSpaceInternal(r, w);
	getSegmentsInternal(r, space, vector);
	return space;
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::getWritable(
		CircularBufferVector<X> &vector) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();

	auto space = writeSpaceInternal(r, w);
	getSegmentsInternal(w, space, vector);
	return space;
}

template<typename X, bool ATOMIC>
void wanhive::CircularBuffer<X, ATOMIC>::skipRead(unsigned int count) noexcept {
	releaseBarrier();
	storeReadIndex(skip(loadReadIndex(), count));
}

template<typename X, bool ATOMIC>
void wanhive::CircularBuffer<X, ATOMIC>::skipWrite(unsigned int count) noexcept {
	releaseBarrier();
	storeWriteIndex(skip(loadWriteIndex(), count));
}

template<typename X, bool ATOMIC>
bool wanhive::CircularBuffer<X, ATOMIC>::get(X &value) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();
	if (isEmptyInternal(r, w)) {
		return false;
	}
	value = storage[r];
	releaseBarrier();
	storeReadIndex(skip(r, 1));
	return true;
}

template<typename X, bool ATOMIC>
X* wanhive::CircularBuffer<X, ATOMIC>::get() noexcept {
	auto w = writeIndex;
	auto r = readIndex;
	if (isEmptyInternal(r, w)) {
		return nullptr;
	}
	X *handle = &storage[r];

	readIndex = skip(r, 1);
	return handle;
}

template<typename X, bool ATOMIC>
bool wanhive::CircularBuffer<X, ATOMIC>::put(const X &value) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();
	if (isFullInternal(r, w)) {
		return false;
	}
	storage[w] = value;
	releaseBarrier();
	storeWriteIndex(skip(w, 1));
	return true;
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::read(X *dest,
		unsigned int length) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();

	auto space = readSpaceInternal(r, w);
	space = Twiddler::min(length, space);
	if (!dest || !space) {
		return 0;
	}

	auto total = r + space;
	if (total > _size) {
		//Two segments (write index trails)
		auto f = _size - r;
		memcpy(dest, &storage[r], f * sizeof(X));
		auto s = total - _size;
		memcpy(dest + f, storage, s * sizeof(X));
	} else {
		//Single segment
		memcpy(dest, &storage[r], space * sizeof(X));
	}
	releaseBarrier();
	storeReadIndex(skip(r, space));
	return space;
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::write(const X *src,
		unsigned int length) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();

	auto space = writeSpaceInternal(r, w);
	space = Twiddler::min(space, length);
	if (!src || !space) {
		return 0;
	}

	auto total = w + space;
	if (total > _size) {
		//Two segments (read index trails)
		auto f = _size - w;
		memcpy(&storage[w], src, f * sizeof(X));
		auto s = total - _size;
		memcpy(storage, src + f, s * sizeof(X));
	} else {
		//Single segment
		memcpy(&storage[w], src, space * sizeof(X));
	}
	releaseBarrier();
	storeWriteIndex(skip(w, space));
	return space;
}

template<typename X, bool ATOMIC>
X* wanhive::CircularBuffer<X, ATOMIC>::array() noexcept {
	return this->storage;
}

template<typename X, bool ATOMIC>
const X* wanhive::CircularBuffer<X, ATOMIC>::array() const noexcept {
	return this->storage;
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::skip(unsigned int index,
		unsigned int count) const noexcept {
	return (index + count) & _capacity;
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::readSpaceInternal(
		unsigned int r, unsigned int w) const noexcept {
	//<write-index>-<read-index>
	return ((w - r) & _capacity);
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::writeSpaceInternal(
		unsigned int r, unsigned int w) const noexcept {
	//<capacity>-<readSpace>
	return (_capacity - ((w - r) & _capacity));
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::isFullInternal(unsigned int r,
		unsigned int w) const noexcept {
	return (((w + 1) & _capacity) == r);
}

template<typename X, bool ATOMIC>
unsigned int wanhive::CircularBuffer<X, ATOMIC>::isEmptyInternal(unsigned int r,
		unsigned int w) const noexcept {
	return (r == w);
}

template<typename X, bool ATOMIC>
void wanhive::CircularBuffer<X, ATOMIC>::getSegmentsInternal(unsigned int index,
		unsigned int length, CircularBufferVector<X> &vector) noexcept {
	auto total = index + length;
	if (total > _size) {
		//Two segments (other index wrapped around)
		vector.part[0] = { &storage[index], (_size - index) };
		vector.part[1] = { storage, (total - _size) };
	} else {
		//Single segment
		vector.part[0] = { (length ? &storage[index] : nullptr), length };
		vector.part[1] = { nullptr, 0 };
	}
}

#endif /* WH_BASE_DS_CIRCULARBUFFER_H_ */
