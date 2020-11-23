/*
 * StaticCircularBuffer.h
 *
 * Single producer - single consumer lock free circular buffer
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_STATICCIRCULARBUFFER_H_
#define WH_BASE_DS_STATICCIRCULARBUFFER_H_
#include "CircularBufferVector.h"
#include "Twiddler.h"
#include "../common/Atomic.h"
#include <cstring>

namespace wanhive {
/**
 * Lock free circular buffer for POD types
 * Read/write real-time safe for single-producer-single-consumer
 * REF: Dennis Lang's Ring Buffer (http://landenlabs.com/code/ring/ring.html)
 */
template<typename X, unsigned int SIZE, bool ATOMIC = false> class StaticCircularBuffer {
public:
	StaticCircularBuffer() noexcept;
	~StaticCircularBuffer();

	//Returns max capacity of this buffer
	unsigned int capacity() const noexcept;
	//How much data can be read from the buffer
	unsigned int readSpace() noexcept;
	//How much data can be written to the buffer
	unsigned int writeSpace() noexcept;
	/*
	 * Reset the read and write pointers, making an empty buffer.
	 * Not thread safe
	 */
	void clear() noexcept;
	/*
	 * Returns true if the buffer is full
	 */
	bool isFull() noexcept;
	/*
	 * Returns true if the buffer is empty
	 */
	bool isEmpty() noexcept;
	/*
	 * Read one element from the buffer, fails if the buffer is empty
	 */
	bool get(X &value) noexcept;
	/*
	 * Read one element from the buffer and return the pointer, returns nullptr
	 * if the buffer is empty. Not thread safe
	 */
	X* get() noexcept;
	/*
	 * Put an element into the buffer, fails if the buffer is full
	 */
	bool put(X const &value) noexcept;
	/*
	 * Stores description of readable linear segments into <vector>.
	 * Returns total count of readable slots
	 */
	unsigned int getReadable(CircularBufferVector<X> &vector) noexcept;
	/*
	 * Stores description of writable linear segments into <vector>.
	 * Returns total count of writable slots
	 */
	unsigned int getWritable(CircularBufferVector<X> &vector) noexcept;
	//Move the read index forward by <count> slots
	void skipRead(unsigned int count) noexcept;
	//Move the write index forward by <count> slots
	void skipWrite(unsigned int count) noexcept;
	/*
	 * Bulk read, transfers at most <length> elements from this buffer
	 * into the given destination array and the read index is updated,
	 * returns the actual number of elements transferred
	 */
	unsigned int read(X *dest, unsigned int length) noexcept;
	/*
	 * Bulk write, transfers at most <length> elements from the
	 * source array into the buffer, and the write index is updated,
	 * returns the actual number of elements transferred
	 */
	unsigned int write(const X *src, unsigned int length) noexcept;
	/*
	 * Returns the backing array
	 * (For IO Operations)
	 */
	X* array() noexcept;

	/**
	 * Used to maintain status of the buffer
	 */
	int getStatus() noexcept {
		if (ATOMIC) {
			return Atomic<int>::load(&status);
		} else {
			return status;
		}
	}
	void setStatus(int status) noexcept {
		if (ATOMIC) {
			Atomic<int>::store(&(this->status), status);
		} else {
			this->status = status;
		}
	}
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
	unsigned int loadReadIndex() noexcept {
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

	unsigned int loadWriteIndex() noexcept {
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

	/*
	 * Load-Load barrier:  prevent memory reordering of the read-acquire
	 * with any read or write operation that follows it. All memory operations
	 * below this barrier remain below it.
	 */
	void acquireBarrier() noexcept {
		if (ATOMIC) {
			Atomic<>::threadFence(MO_ACQUIRE);
		}
	}

	/*
	 * Store-Store barrier: prevent memory reordering of the write-release
	 * with any read or write operation that precedes it. All memory operations
	 * above this barrier remain above it.
	 */
	void releaseBarrier() noexcept {
		if (ATOMIC) {
			Atomic<>::threadFence(MO_RELEASE);
		}
	}
private:
	WH_POD_ASSERT(X);
	const unsigned int _size;
	const unsigned int _capacity;
	int status;
	unsigned int writeIndex;
	unsigned int readIndex;
	X storage[SIZE];
};

} /* namespace wanhive */

template<typename X, unsigned int SIZE, bool ATOMIC> wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::StaticCircularBuffer() noexcept :
		_size(SIZE), _capacity(SIZE ? (SIZE - 1) : SIZE) {
	WH_STATIC_ASSERT((SIZE && !(SIZE & (SIZE - 1))), "Bad buffer size");
	clear();
}

template<typename X, unsigned int SIZE, bool ATOMIC> wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::~StaticCircularBuffer() {
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::capacity() const noexcept {
	return _capacity;
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::readSpace() noexcept {
	return readSpaceInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::writeSpace() noexcept {
	return writeSpaceInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, unsigned int SIZE, bool ATOMIC> void wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::clear() noexcept {
	storeReadIndex(0);
	storeWriteIndex(0);
}

template<typename X, unsigned int SIZE, bool ATOMIC> bool wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::isFull() noexcept {
	return isFullInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, unsigned int SIZE, bool ATOMIC> bool wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::isEmpty() noexcept {
	return isEmptyInternal(loadReadIndex(), loadWriteIndex());
}

template<typename X, unsigned int SIZE, bool ATOMIC> bool wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::get(X &value) noexcept {
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

template<typename X, unsigned int SIZE, bool ATOMIC> X* wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::get() noexcept {
	auto w = writeIndex;
	auto r = readIndex;
	if (isEmptyInternal(r, w)) {
		return nullptr;
	}
	X *handle = &storage[r];

	readIndex = skip(r, 1);
	return handle;
}

template<typename X, unsigned int SIZE, bool ATOMIC> bool wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::put(const X &value) noexcept {
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

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::getReadable(CircularBufferVector<X> &vector) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();

	auto space = readSpaceInternal(r, w);
	getSegmentsInternal(r, space, vector);
	return space;
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::getWritable(CircularBufferVector<X> &vector) noexcept {
	auto w = loadWriteIndex();
	auto r = loadReadIndex();
	acquireBarrier();

	auto space = writeSpaceInternal(r, w);
	getSegmentsInternal(w, space, vector);
	return space;
}

template<typename X, unsigned int SIZE, bool ATOMIC> void wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::skipRead(unsigned int count) noexcept {
	releaseBarrier();
	storeReadIndex(skip(loadReadIndex(), count));
}

template<typename X, unsigned int SIZE, bool ATOMIC> void wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::skipWrite(unsigned int count) noexcept {
	releaseBarrier();
	storeWriteIndex(skip(loadWriteIndex(), count));
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::read(X *dest, unsigned int length) noexcept {
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

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::write(const X *src, unsigned int length) noexcept {
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

template<typename X, unsigned int SIZE, bool ATOMIC> X* wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::array() noexcept {
	return this->storage;
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::skip(unsigned int index,
		unsigned int count) const noexcept {
	return (index + count) & _capacity;
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::readSpaceInternal(unsigned int r,
		unsigned int w) const noexcept {
	//<write-index>-<read-index>
	return ((w - r) & _capacity);
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::writeSpaceInternal(unsigned int r,
		unsigned int w) const noexcept {
	//<capacity>-<readSpace>
	return (_capacity - ((w - r) & _capacity));
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::isFullInternal(unsigned int r,
		unsigned int w) const noexcept {
	return (((w + 1) & _capacity) == r);
}

template<typename X, unsigned int SIZE, bool ATOMIC> unsigned int wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::isEmptyInternal(unsigned int r,
		unsigned int w) const noexcept {
	return (r == w);
}

template<typename X, unsigned int SIZE, bool ATOMIC> void wanhive::StaticCircularBuffer<
		X, SIZE, ATOMIC>::getSegmentsInternal(unsigned int index,
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

#endif /* WH_BASE_DS_STATICCIRCULARBUFFER_H_ */
