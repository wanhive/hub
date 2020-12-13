/*
 * BinaryHeap.h
 *
 * Binary heap implementation
 *
 *
 * Copyright (C) 2020 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_BINARYHEAP_H_
#define WH_BASE_DS_BINARYHEAP_H_
#include "functors.h"
#include "Twiddler.h"
#include "../common/Exception.h"
#include "../common/pod.h"

namespace wanhive {
//-----------------------------------------------------------------
/**
 * The sample functor for BHFN
 */
template<typename X> struct wh_bhfn_fn {
	void operator()(X const &e, unsigned int index) const noexcept {

	}
};
//-----------------------------------------------------------------
/**
 * Binary heap implementation
 * CMPFN compares two objects. To create a minheap CMPFN must return true if
 * it's first argument is less than the second one. BHFN is called whenever
 * the heap is updated. The first argument of BHFN must be a reference to the
 * updated slot, while the second argument is that slot's index.
 */
template<typename X = int, typename CMPFN = wh_lt_fn,
		typename BHFN = wh_bhfn_fn<X> > class BinaryHeap {
public:
	BinaryHeap() noexcept;
	BinaryHeap(unsigned int size);
	~BinaryHeap();

	void initialize(unsigned int size);
	//Clears the heap (doesn't clean up the contents)
	void clear() noexcept;

	//Returns heap's capacity
	unsigned int capacity() const noexcept;
	//Returns the number of elements in the heap
	unsigned int size() const noexcept;
	//Returns true if the heap is empty, false otherwise
	bool isEmpty() const noexcept;
	//Returns true if the heap is full, false otherwise
	bool isFull() const noexcept;

	//Reads the element at the given <index> (the root is at index = 0)
	bool get(X &e, unsigned int index = 0) const noexcept;
	//Insert the element <e> into the binary heap
	bool insert(const X &e) noexcept;
	//Remove the element at the given <index> (the root is at index = 0)
	bool remove(unsigned int index = 0) noexcept;
	//Binary heap traversal, <f> must return 0 to continue
	void map(int (*f)(const X &e, void *arg), void *data);
	//Returns a pointer to the backing array
	X* array() noexcept;
	//Returns a const pointer to the backing array
	const X* array() const noexcept;
private:
	//Swaps the slots at the indices <i> and <j>
	void swap(unsigned int i, unsigned int j) noexcept {
		X tmp = storage[i];
		set(storage[j], i);
		set(tmp, j);
	}

	//Writes the value <e> at the <index>
	void set(const X &e, unsigned int index) noexcept {
		storage[index] = e;
		indexer(storage[index], index);
	}

	unsigned int getLeftChildIndex(unsigned int index) const noexcept {
		return (index << 1) + 1; //2 * index + 1
	}
	unsigned int getRightChildIndex(unsigned int index) const noexcept {
		return (index << 1) + 2; //2 * index + 2
	}
	unsigned int getParentIndex(unsigned int index) const noexcept {
		return ((index - 1) >> 1); //(index - 1) / 2
	}
	unsigned int shiftUp(unsigned int index) noexcept;
	unsigned int shiftDown(unsigned int index) noexcept;

private:
	WH_POD_ASSERT(X);
	X *storage;
	unsigned int _capacity;
	unsigned int _size;
	BHFN indexer;
	CMPFN cmp;
};

} /* namespace wanhive */

template<typename X, typename CMPFN, typename BHFN>
wanhive::BinaryHeap<X, CMPFN, BHFN>::BinaryHeap() noexcept {
	storage = nullptr;
	_capacity = 0;
	_size = 0;
}

template<typename X, typename CMPFN, typename BHFN>
wanhive::BinaryHeap<X, CMPFN, BHFN>::BinaryHeap(unsigned int size) {
	storage = nullptr;
	initialize(size);
}

template<typename X, typename CMPFN, typename BHFN>
wanhive::BinaryHeap<X, CMPFN, BHFN>::~BinaryHeap() {
	delete[] storage;
}

template<typename X, typename CMPFN, typename BHFN>
void wanhive::BinaryHeap<X, CMPFN, BHFN>::initialize(unsigned int size) {
	try {
		size = Twiddler::power2Ceil(size);
		delete[] storage;
		storage = new X[size];
		_capacity = size;
		_size = 0;
	} catch (...) {
		throw Exception(EX_ALLOCFAILED);
	}
}

template<typename X, typename CMPFN, typename BHFN>
void wanhive::BinaryHeap<X, CMPFN, BHFN>::clear() noexcept {
	_size = 0;
}

template<typename X, typename CMPFN, typename BHFN>
unsigned int wanhive::BinaryHeap<X, CMPFN, BHFN>::capacity() const noexcept {
	return _capacity;
}

template<typename X, typename CMPFN, typename BHFN>
unsigned int wanhive::BinaryHeap<X, CMPFN, BHFN>::size() const noexcept {
	return _size;
}

template<typename X, typename CMPFN, typename BHFN>
bool wanhive::BinaryHeap<X, CMPFN, BHFN>::isEmpty() const noexcept {
	return _size == 0;
}

template<typename X, typename CMPFN, typename BHFN>
bool wanhive::BinaryHeap<X, CMPFN, BHFN>::isFull() const noexcept {
	return _size == _capacity;
}

template<typename X, typename CMPFN, typename BHFN>
bool wanhive::BinaryHeap<X, CMPFN, BHFN>::get(X &e,
		unsigned int index) const noexcept {
	if (index < _size) {
		e = storage[index];
		return true;
	} else {
		return false;
	}
}

template<typename X, typename CMPFN, typename BHFN>
bool wanhive::BinaryHeap<X, CMPFN, BHFN>::insert(const X &e) noexcept {
	if (!isFull()) {
		set(e, _size++);
		shiftUp(_size - 1);
		return true;
	} else {
		return false;
	}
}

template<typename X, typename CMPFN, typename BHFN>
bool wanhive::BinaryHeap<X, CMPFN, BHFN>::remove(unsigned int index) noexcept {
	if (index < _size) {
		//Replace the deleted node with the right-most leaf
		set(storage[--_size], index);
		/*
		 * REF: http://www.mathcs.emory.edu/~cheung/Courses/171/Syllabus/9-BinTree/heap-delete.html
		 * If the replacement is greater (minheap) than the parent, shift down,
		 * Otherwise shift up. Does the right thing for maxheap as well.
		 */
		auto parent = getParentIndex(index);
		if ((index == 0) || cmp(storage[parent], storage[index])) {
			shiftDown(index);
		} else {
			shiftUp(index);
		}
		return true;
	} else {
		return false;
	}
}

template<typename X, typename CMPFN, typename BHFN>
void wanhive::BinaryHeap<X, CMPFN, BHFN>::map(int (*f)(const X &e, void *arg),
		void *data) {
	for (unsigned int i = 0; f && i < _size; ++i) {
		if (f(storage[i], data)) {
			break;
		}
	}
}

template<typename X, typename CMPFN, typename BHFN>
X* wanhive::BinaryHeap<X, CMPFN, BHFN>::array() noexcept {
	return storage;
}

template<typename X, typename CMPFN, typename BHFN>
const X* wanhive::BinaryHeap<X, CMPFN, BHFN>::array() const noexcept {
	return storage;
}

template<typename X, typename CMPFN, typename BHFN>
unsigned int wanhive::BinaryHeap<X, CMPFN, BHFN>::shiftUp(
		unsigned int index) noexcept {
	while (index != 0) {
		auto parent = getParentIndex(index);
		if (cmp(storage[index], storage[parent])) {
			swap(parent, index);
			index = parent;
		} else {
			break;
		}
	}
	//Return the final position
	return index;
}

template<typename X, typename CMPFN, typename BHFN>
unsigned int wanhive::BinaryHeap<X, CMPFN, BHFN>::shiftDown(
		unsigned int index) noexcept {
	while (index < _size) {
		unsigned int min = 0;
		auto left = getLeftChildIndex(index);
		auto right = getRightChildIndex(index);

		//Get the index to smaller child
		if ((left >= _size) || (left <= index)) {
			//We reached the leaf
			break;
		} else if (right >= _size) {
			//Only the left child exists
			min = left;
		} else {
			//Both children exist
			min = (cmp(storage[right], storage[left]) ? right : left);
		}

		//If current node's value is greater than min-index's value then swap
		if (cmp(storage[min], storage[index])) {
			swap(min, index);
			index = min;
		} else {
			break;
		}
	}
	//Return the final position
	return index;
}

#endif /* WH_BASE_DS_BINARYHEAP_H_ */
