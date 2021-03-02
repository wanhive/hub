/*
 * Khash.h
 *
 * Hash table implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

/*
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * The MIT License
 Copyright (c) 2008, 2009, 2011 by Attractive Chaos <attractor@live.co.uk>
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef WH_BASE_DS_KHASH_H_
#define WH_BASE_DS_KHASH_H_
#include "functors.h"
#include "Twiddler.h"
#include "../common/Memory.h"
#include "../common/pod.h"

namespace wanhive {
/**
 * C++ adaption of khash version 0.2.8 by Attractive Chaos
 * Both KEY and VALUE must be **POD** (Pointers are OK)
 * REF: https://github.com/attractivechaos/klib/blob/master/khash.h
 */
template<typename KEY = int, typename VALUE = char, bool ISMAP = true,
		typename HFN = wh_hash_fn, typename EQFN = wh_eq_fn> class Khash {
public:
	Khash() noexcept;
	~Khash();

	//Returns true if the <key> is in the table
	bool contains(KEY const &key) const noexcept;
	//Removes the <key> from the table
	bool removeKey(KEY const &key) noexcept;
	//Gets the <value> associated with the <key>, returns true on success
	bool hmGet(KEY const &key, VALUE &val) const noexcept;
	//Stores the <key>, <val> pair, fails if the <key> already exists
	bool hmPut(KEY const &key, VALUE const &val) noexcept;
	/*
	 * Returns true if old value was replaced, false otherwise
	 * Extra Return Value <oldVal>: Old Value associated with the key
	 */
	bool hmReplace(KEY const &key, VALUE const &val, VALUE &oldVal) noexcept;
	/*
	 * If <first> key exists and <second> key doesn't exist then the <first> value is re-registered
	 * as <second> (and vice-versa). If both <first> and <second> keys exist and if <swap> is true
	 * then the values associated with the keys are swapped. On success returns true, otherwise
	 * false (could not swap or neither of the two keys exist). Iterators to <first> and <second>
	 * keys after operation are returned in that order via <iterators>.
	 *
	 * NOTE: subsequent method calls which modify the hash table invalidate the iterators.
	 */
	bool hmSwap(KEY const &first, KEY const &second, unsigned int iterators[2],
			bool swap) noexcept;
	bool hsPut(KEY const &key) noexcept;
	//=================================================================
	/**
	 * Primitive hash table operations
	 * NOTE: iterator is invalidated by the operations which modify the hash
	 * table hence an iterator should be used immediately after retrieval.
	 */

	/*
	 * Resize the hash table to new capacity (resize or bust mode).
	 * Resizing invalidates the iterators. Always returns 0.
	 */
	int resize(unsigned int newCapacity) noexcept;
	/*
	 * Returns iterator to the found element, or end() if the element is absent
	 */
	unsigned int get(KEY const &key) const noexcept;
	/*
	 * Inserts a key into the hash table. Returns iterator to the inserted element.
	 * Check the extra return codes in <ret> which have been described below.
	 * Extra return codes: 0 if the key is already present in the hash table;
	 * 1 if the bucket was empty (never used); 2 if the element in the bucket
	 * had been deleted.
	 */
	unsigned int put(KEY const &key, int &ret) noexcept;
	/*
	 * Remove a key from the hash table and if specified, try to shrink the
	 * sparsely populated hash table. Shrinking invalidates the iterators.
	 */
	void remove(unsigned int x, bool shrink = true) noexcept;
	/*
	 * Iterates the function <fn> over the hash table.
	 * <fn> receives iterator of the next filled bucket via <index>
	 * <fn> also receives the caller-provided argument via <arg>
	 * To continue iterating <fn> must return 0
	 * To remove the key at the current position and continue <fn> must return 1
	 * Any other value returned by <fn> will stop the iteration
	 */
	void iterate(int (*fn)(unsigned int index, void *arg), void *arg);
	//Returns the number of buckets in the hash table
	unsigned int capacity() const noexcept;
	//Returns the number of elements in the hash table (deleted slots not included)
	unsigned int size() const noexcept;
	/*
	 * Returns the number of buckets currently occupied. A bucket is occupied if
	 * it is either filled or deleted.
	 */
	unsigned int occupied() const noexcept;
	//The upper bound on the number of occupied slots permitted at the current capacity.
	unsigned int upperBound() const noexcept;
	/*
	 * Returns true if the bucket at the index <x> is filled i.e. it is neither
	 * empty nor deleted.
	 */
	bool exists(unsigned int x) const noexcept;
	//Returns the key present in the bucket at given index
	bool getKey(unsigned int x, KEY &key) const noexcept;
	//Returns the value present in the bucket at given index
	bool getValue(unsigned int x, VALUE &value) const noexcept;
	//Sets a value in the bucket at given index
	bool setValue(unsigned int x, VALUE const &value) noexcept;
	//Returns the pointer to the stored value at given index <x>
	VALUE* getValueReference(unsigned int x) const noexcept;
	//Returns the start iterator
	unsigned int begin() const noexcept;
	//Returns the end iterator
	unsigned int end() const noexcept;
	//Reset the hash table without deallocating memory
	void clear() noexcept;
private:
	//Initialize a hash table.
	void init() noexcept;
	//Destroy a hash table.
	void destroy() noexcept;
	//Update <capacity>
	void capacity(unsigned int capacity) noexcept {
		bucket.capacity = capacity;
	}
	//Update <size>
	void size(unsigned int size) noexcept {
		bucket.size = size;
	}
	//Update <occupied>
	void occupied(unsigned int occupied) noexcept {
		bucket.occupied = occupied;
	}
	//Update the upper bound
	void upperBound(unsigned int upperBound) noexcept {
		bucket.upperBound = upperBound;
	}
	uint32_t* getFlags() const noexcept {
		return (bucket.flags);
	}
	void setFlags(uint32_t *flags) noexcept {
		WH_free(bucket.flags);
		bucket.flags = flags;
	}

	void resetFlags() noexcept {
		if (bucket.flags) {
			memset(bucket.flags, 0xaa,
					fSize(bucket.capacity) * sizeof(uint32_t));
		}
	}

	//Delete the storage
	void deleteContainer() noexcept {
		if (ISMAP) {
			WH_free(bucket.entry);
			bucket.entry = nullptr;
		} else {
			WH_free(bucket.keys);
			bucket.keys = nullptr;
		}
	}
	//Resize the internal storage
	void resizeContainer(unsigned int size) noexcept {
		if (ISMAP) {
			WH_resize(bucket.entry, size);
		} else {
			WH_resize(bucket.keys, size);
		}
	}

	//Delete the flags container
	void deleteFlags() noexcept {
		WH_free(bucket.flags);
		bucket.flags = nullptr;
	}

	//Get key at given an iterator
	const KEY& getKey(unsigned int x) const noexcept {
		return (ISMAP ? bucket.entry[x].key : bucket.keys[x]);
	}

	//Set the key at given iterator
	void setKey(unsigned int x, KEY const &key) noexcept {
		if (ISMAP) {
			bucket.entry[x].key = key;
		} else {
			bucket.keys[x] = key;
		}
	}

	//Get value given an iterator
	const VALUE& getValue(unsigned int x) const noexcept {
		return (bucket.entry[x].value);
	}

	//Get value as lvalue given an iterator
	VALUE& valueAt(unsigned int x) noexcept {
		return (bucket.entry[x].value);
	}

	//Create flags container for bucket size of <size>
	static uint32_t* createFlags(unsigned int size) noexcept {
		uint32_t *new_flags = (uint32_t*) WH_malloc(
				fSize(size) * sizeof(uint32_t));
		memset(new_flags, 0xaa, fSize(size) * sizeof(uint32_t));
		return new_flags;
	}

	//Check whether the bucket at <i> is empty
	static bool isEmpty(const uint32_t *flag, unsigned int i) noexcept {
		return ((flag[i >> 4] >> ((i & 0xfU) << 1)) & 2);
	}
	//Check whether the value in the bucket at <i> has been deleted
	static bool isDeleted(const uint32_t *flag, unsigned int i) noexcept {
		return ((flag[i >> 4] >> ((i & 0xfU) << 1)) & 1);
	}
	//Check whether bucket at <i> is either empty or deleted
	static bool isEither(const uint32_t *flag, unsigned int i) noexcept {
		return ((flag[i >> 4] >> ((i & 0xfU) << 1)) & 3);
	}
	//Set delete bit=false for bucket at <i>
	static void setIsdeletedFalse(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] &= ~(1ul << ((i & 0xfU) << 1)));
	}
	//set empty bit=false for the bucket at <i>
	static void setIsemptyFalse(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] &= ~(2ul << ((i & 0xfU) << 1)));
	}
	//Set both delete and empty bits=false for the bucket at <i>
	static void setIsbothFalse(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] &= ~(3ul << ((i & 0xfU) << 1)));
	}
	//Set delete bit=true for the bucket at <i>
	static void setIsdeletedTrue(uint32_t *flag, unsigned int i) noexcept {
		(flag[i >> 4] |= 1ul << ((i & 0xfU) << 1));
	}
	//Size of flags container (@2 bits per slot)
	static unsigned int fSize(unsigned int slots) noexcept {
		return ((slots) < 16 ? 1 : (slots) >> 4);
	}
	//Linear probe
	static unsigned int linearProbe(const unsigned int index,
			const unsigned int step, const unsigned int mask) noexcept {
		return (index + step) & mask;
	}
	//Calculate the upper bound for given capacity
	static unsigned int calculateUpperBound(unsigned int capacity) noexcept {
		return (unsigned int) ((capacity * LOAD_FACTOR) + 0.5);
	}
private:
	//Hashtable container
	struct {
		unsigned int capacity;	//Total number of slots, always power of two
		unsigned int size;			//Total number of filled up slots
		unsigned int occupied;//Total number of occupied slots (size + deleted)
		unsigned int upperBound;	//Upper Limit on occupied slots
		uint32_t *flags;	//Status of each slot, need to specify data width
		struct {
			KEY key;
			VALUE value;
		} *entry; //Used by HASH-MAP
		KEY *keys; //Used by HASH-SET
	} bucket;

	HFN hash; //Hash functor (must return unsigned int)
	EQFN equal; //Equality functor

	//Minimum designated capacity of the hash table
	static constexpr unsigned int MIN_CAPACITY = 16;
	//The load factor
	static constexpr double LOAD_FACTOR = 0.77;
	//KEY must be POD
	WH_POD_ASSERT(KEY);
	//VALUE must be POD
	WH_POD_ASSERT(VALUE);
};

} /* namespace wanhive */

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::Khash() noexcept {
	init();
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::~Khash() {
	destroy();
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::contains(const KEY &key) const noexcept {
	auto i = get(key);
	return (i != end());
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::removeKey(const KEY &key) noexcept {
	auto i = get(key);
	if (i != end()) {
		remove(i);
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::hmGet(const KEY &key,
		VALUE &val) const noexcept {
	auto i = get(key);
	if (i != end()) {
		val = getValue(i);
		return true;
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::hmPut(const KEY &key,
		const VALUE &val) noexcept {
	if (ISMAP) {
		int ret;
		unsigned int i = put(key, ret);
		if (ret) {
			valueAt(i) = val;
			return true;
		}
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN>
bool wanhive::Khash<KEY, VALUE, ISMAP, HFN, EQFN>::hmReplace(const KEY &key,
		const VALUE &val, VALUE &oldVal) noexcept {
	if (ISMAP) {
		int ret;
		auto i = put(key, ret);
		if (ret) {
			//Key did not exist
			valueAt(i) = val;
			return false;
		} else {
			oldVal = getValue(i);
			valueAt(i) = val;
			return true;
		}
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::hmSwap(KEY const &first,
		KEY const &second, unsigned int iterators[2], bool swap) noexcept {

	if (!ISMAP) {
		iterators[0] = end();
		iterators[1] = end();
		return false;
	}

	auto fi = get(first);
	auto si = (first != second) ? get(second) : fi;
	//Correct iterators are returned even on failure
	iterators[0] = fi;
	iterators[1] = si;

	if (fi == si) {
		return exists(fi);
	} else if (exists(fi) && exists(si) && swap) {
		VALUE fv = getValue(fi);
		VALUE sv = getValue(si);
		setValue(fi, sv);
		setValue(si, fv);
		return true;
	} else if (exists(fi) && !exists(si)) {
		int x;
		VALUE fv = getValue(fi);
		remove(fi);
		si = put(second, x);
		setValue(si, fv);
		iterators[0] = end();
		iterators[1] = si;
		return true;
	} else if (!exists(fi) && exists(si)) {
		int x;
		VALUE sv = getValue(si);
		remove(si);
		fi = put(first, x);
		setValue(fi, sv);
		iterators[0] = fi;
		iterators[1] = end();
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::hsPut(const KEY &key) noexcept {
	if (!ISMAP) {
		int ret;
		put(key, ret);
		if (ret) {
			return true;
		}
	}
	return false;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::resize(unsigned int newCapacity) noexcept {
	/* This function uses 0.25*nBuckets bytes of working space instead of [sizeof(key_t+val_t)+.25]*nBuckets. */
	uint32_t *newFlags = nullptr;
	bool rehash = true;
	{
		newCapacity = Twiddler::power2Ceil(newCapacity);
		if (newCapacity < MIN_CAPACITY) {
			newCapacity = MIN_CAPACITY;
		}
		if (size() >= calculateUpperBound(newCapacity)) {
			/* requested size is too small */
			rehash = false;
		} else {
			/* hash table size to be changed (shrink or expand); rehash */
			newFlags = createFlags(newCapacity);

			/*
			 * If the requested size is bigger then expand
			 * otherwise shrink (inside rehash block)
			 */
			if (capacity() < newCapacity) {
				/* expand */
				resizeContainer(newCapacity);
			}
		}
	}

	/* rehashing is needed */
	if (rehash) {
		for (unsigned int j = 0; j != capacity(); ++j) {
			if (!isEither(getFlags(), j)) {
				KEY key = getKey(j);
				VALUE val;
				auto newMask = newCapacity - 1;
				if (ISMAP) {
					val = valueAt(j);
				}
				setIsdeletedTrue(getFlags(), j);

				/*
				 * The kick-out process; sort of like in *Cuckoo Hashing*
				 * If the slot is free then just insert the item
				 * If the slot is occupied then kick out the occupant to
				 * accommodate the item. The kicked-out item will be inserted
				 * back in the next iteration in similar fashion.
				 */
				while (true) {
					auto i = newMask & (unsigned int) hash(key);
					unsigned int step = 0;
					while (!isEmpty(newFlags, i)) {
						//Quadratic Probe
						i = linearProbe(i, (++step), newMask);
					}
					setIsemptyFalse(newFlags, i);
					if (i < capacity() && !isEither(getFlags(), i)) {
						/* kick out the existing element */
						{
							KEY tmp = getKey(i);
							setKey(i, key);
							key = tmp;
						}
						if (ISMAP) {
							VALUE tmp = getValue(i);
							valueAt(i) = val;
							val = tmp;
						}
						/* mark it as deleted in the old hash table */
						setIsdeletedTrue(getFlags(), i);
					} else {
						/* write the element and jump out of the loop */
						setKey(i, key);
						if (ISMAP) {
							valueAt(i) = val;
						}
						break;
					}
				}
			}
		}
		if (capacity() > newCapacity) {
			/* shrink the hash table */
			resizeContainer(newCapacity);
		}
		setFlags(newFlags);
		capacity(newCapacity);
		occupied(size());
		upperBound(calculateUpperBound(capacity()));
	}
	return 0;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::get(const KEY &key) const noexcept {
	if (capacity()) {
		auto mask = capacity() - 1;
		auto index = mask & (unsigned int) hash(key);
		auto last = index;	//the initial position
		unsigned int step = 0;
		while (!isEmpty(getFlags(), index)
				&& (isDeleted(getFlags(), index) || !equal(getKey(index), key))) {
			//Quadratic Probe
			index = linearProbe(index, (++step), mask);
			if (index == last) {
				return end();
			}
		}
		return isEither(getFlags(), index) ? end() : index;
	} else {
		return 0;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::put(const KEY &key, int &ret) noexcept {
	if (occupied() >= upperBound()) { /* update the hash table */
		if (capacity() > (size() << 1)) {
			resize(capacity() - 1); /* clear "deleted" elements */
		} else {
			resize(capacity() + 1); /* expand the hash table */
		}
	}

	unsigned int index;
	{
		auto mask = capacity() - 1;
		auto i = mask & (unsigned int) hash(key);
		if (isEmpty(getFlags(), i)) {
			index = i; /* for speed up */
		} else {
			auto start = i;			//save the initial position
			auto step = 0;			//counter for quadratic probing
			auto site = end();		//the last deleted slot
			index = end();
			while (!isEmpty(getFlags(), i)
					&& (isDeleted(getFlags(), i) || !equal(getKey(i), key))) {
				if (isDeleted(getFlags(), i)) {
					//Record the last deleted slot and probe ahead
					site = i;
				}
				//Quadratic Probe
				i = linearProbe(i, (++step), mask);
				if (i == start) {
					//We went full circle, record the last deleted slot and break
					index = site;
					break;
				}
			}
			if (index == end()) {
				if (isEmpty(getFlags(), i) && site != end()) {
					//We hit an empty slot succeeding a number of deleted ones
					index = site;
				} else {
					//Key already present
					index = i;
				}
			}
		}
	}

	if (isEmpty(getFlags(), index)) { /* not present at all */
		setKey(index, key);
		setIsbothFalse(getFlags(), index);
		size(size() + 1);
		occupied(occupied() + 1);
		ret = 1;
	} else if (isDeleted(getFlags(), index)) { /* deleted */
		setKey(index, key);
		setIsbothFalse(getFlags(), index);
		size(size() + 1);
		ret = 2;
	} else {
		/* Don't touch bucket.keys[x] if present and not deleted */
		ret = 0;
	}
	return index;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> void wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::remove(unsigned int x,
		bool shrink) noexcept {
	if (exists(x)) {
		setIsdeletedTrue(getFlags(), x);
		size(size() - 1);
	}

	//If the hash table has become too sparse then fix it
	if (shrink && (size() > 4096) && (size() < (capacity() >> 2))) {
		Khash::resize((size() / LOAD_FACTOR) * 1.5);
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> void wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::iterate(
		int (*fn)(unsigned int index, void *arg), void *arg) {
	for (auto k = begin(); fn && (k < end()); ++k) {
		if (!exists(k)) {
			continue;
		}

		int ret = fn(k, arg);
		if (ret == 0) {
			continue;
		} else if (ret == 1) { //Remove the key, useful for cleanup
			remove(k, false); //Shrinking here will invalidate the indexes
		} else { //Stop iterating
			break;
		}
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::capacity() const noexcept {
	return (bucket.capacity);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::size() const noexcept {
	return (bucket.size);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::occupied() const noexcept {
	return (bucket.occupied);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::upperBound() const noexcept {
	return (bucket.upperBound);
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::exists(unsigned int x) const noexcept {
	return (x < end() && !isEither(bucket.flags, (x)));
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::getKey(unsigned int x,
		KEY &key) const noexcept {
	if (exists(x)) {
		key = ISMAP ? bucket.entry[x].key : bucket.keys[x];
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::getValue(unsigned int x,
		VALUE &value) const noexcept {
	if (ISMAP && exists(x)) {
		value = bucket.entry[x].value;
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> bool wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::setValue(unsigned int x,
		VALUE const &value) noexcept {
	if (ISMAP && exists(x)) {
		bucket.entry[x].value = value;
		return true;
	} else {
		return false;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> VALUE* wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::getValueReference(
		unsigned int x) const noexcept {
	if (ISMAP && exists(x)) {
		return &(bucket.entry[x].value);
	} else {
		return nullptr;
	}
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::begin() const noexcept {
	return 0;
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> unsigned int wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::end() const noexcept {
	return capacity();
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> void wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::clear() noexcept {
	resetFlags();
	size(0);		//No elements in the hash table
	occupied(0);	//No deleted slots in the hash table
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> void wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::init() noexcept {
	memset(&bucket, 0, sizeof(bucket));
}

template<typename KEY, typename VALUE, bool ISMAP, typename HFN, typename EQFN> void wanhive::Khash<
		KEY, VALUE, ISMAP, HFN, EQFN>::destroy() noexcept {
	deleteContainer();
	deleteFlags();
}

#endif /* WH_BASE_DS_KHASH_H_ */
