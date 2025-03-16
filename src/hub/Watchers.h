/*
 * Watchers.h
 *
 * Hash table of watchers
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_WATCHERS_H_
#define WH_HUB_WATCHERS_H_
#include "../base/ds/Khash.h"
#include "../reactor/Watcher.h"

namespace wanhive {
/**
 * Hash table of watchers
 */
class Watchers {
public:
	/**
	 * Default constructor: initializes an empty container.
	 */
	Watchers() noexcept;
	/**
	 * Destructor
	 */
	~Watchers();
	/**
	 * Checks whether a key is present.
	 * @param key key's value
	 * @return true if the key exists, false otherwise
	 */
	bool contains(unsigned long long key) const noexcept;
	/**
	 * Returns watcher associated with a given key.
	 * @param key key's value
	 * @return associated watcher, nullptr if the key doesn't exist
	 */
	Watcher* select(unsigned long long key) const noexcept;
	/**
	 * Inserts a new (key, watcher) pair.
	 * @param key key's value
	 * @param w watcher's pointer, its UID is updated to match the key
	 * @return true on success, false otherwise
	 */
	bool insert(unsigned long long key, Watcher *w) noexcept;
	/**
	 * Inserts a new watcher. Watcher's UID is used as the key.
	 * @param w watcher's pointer
	 * @return true on success, false otherwise
	 */
	bool insert(Watcher *w) noexcept;
	/**
	 * Inserts a (key, watcher) pair. If the given key already exists then the
	 * associated watcher is replaced and returned.
	 * @param key key's value
	 * @param w watcher to associate with the given key, its UID is updated to
	 * match the key.
	 * @return replaced watcher (nullptr on a new insertion)
	 */
	Watcher* replace(unsigned long long key, Watcher *w) noexcept;
	/**
	 * Inserts a watcher after resolving any conflict. if another watcher is
	 * associated with the key equal in value to the given watcher's UID then
	 * the existing watcher will be replaced and returned.
	 * @param w watcher to insert into the hash table, its UID will be used as
	 * the key.
	 * @return replaced watcher (nullptr on a new insertion)
	 */
	Watcher* replace(Watcher *w) noexcept;
	/**
	 * Removes a key.
	 * @param key key's value
	 */
	void remove(unsigned long long key) noexcept;
	/**
	 * Swaps watchers associated with the given pair of keys. If only one of the
	 * two keys exists then the existing key is removed from the hash table, its
	 * associated watcher is reassigned to the missing key, and the watcher's
	 * UID is updated to match it's new key. If both the keys exist and swapping
	 * is allowed then watchers associated with the two keys will be swapped and
	 * their UIDs will be updated to match their respective keys.
	 * @param first the first key
	 * @param second the second key
	 * @param w stores watchers associated with the given keys (in order) after
	 * a successful operation.
	 * @param swap true to enable swapping, false otherwise
	 * @return true on success, false on failure (could not swap or neither of
	 * the two keys exists).
	 */
	bool move(unsigned long long first, unsigned long long second,
			Watcher *(&w)[2], bool swap) noexcept;
	/**
	 * Iterates over the hash table, callback function's return value determines
	 * the iteration's behavior:
	 * [0]: continue iteration,
	 * [1]: remove the current entry and continue iteration,
	 * [Any other value]: stop iteration.
	 * @param fn the callback function, receives the next watcher as it's first
	 * argument, and a generic pointer as it's second argument.
	 * @param arg callback function's second argument
	 */
	void iterate(int (*fn)(Watcher*, void*), void *arg);
private:
	//Iteration's entry point
	static int _iterator(unsigned int index, void *arg);
private:
	Kmap<unsigned long long, Watcher*> watchers; //Hash table
	int (*itfn)(Watcher*, void*) {nullptr}; //Actual iterator
	void *itfnarg { nullptr }; //Iterator's argument
};

} /* namespace wanhive */

#endif /* WH_HUB_WATCHERS_H_ */
