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
	 * Inserts a (key, watcher) pair. In case of conflict the old watcher is
	 * replaced and returned.
	 * @param key key's value
	 * @param w watcher to associate with the key, the key is set as watcher's
	 * unique identifier (UID).
	 * @return replaced watcher (nullptr on a new insertion)
	 */
	Watcher* replace(unsigned long long key, Watcher *w) noexcept;
	/**
	 * Inserts a watcher after resolving any conflict. if another watcher is
	 * associated with the given watcher's identifier then the conflicting
	 * watcher will be replaced and returned.
	 * @param w watcher to insert into the hash table, its identifier will be
	 * used as the key.
	 * @return replaced watcher (nullptr on a new insertion)
	 */
	Watcher* replace(Watcher *w) noexcept;
	/**
	 * Removes a key.
	 * @param key key's value
	 */
	void remove(unsigned long long key) noexcept;
	/**
	 * Swaps the watchers associated with a pair of keys. If only one of the two
	 * keys exists then the existing key is removed from the hash table and
	 * its watcher is reassigned to the missing key and the new key is set as
	 * the moved watcher's identifier. If both the keys exist and swapping
	 * is enabled then the watchers associated with the two keys will be
	 * swapped and their respective identifiers will be set to the new keys.
	 * @param first first key
	 * @param second second key
	 * @param w stores the watchers associated with the given keys (in order)
	 * after successful operation.
	 * @param swap true to enable swapping, false otherwise
	 * @return true on success, false on failure (could not swap or neither of
	 * the two keys exists).
	 */
	bool move(unsigned long long first, unsigned long long second,
			Watcher *(&w)[2], bool swap) noexcept;
	/**
	 * Iterates through the hash table. The callback function's return value
	 * determines the behavior:
	 * [0]: continue,
	 * [1]: remove the current entry and continue,
	 * [Any other value]: stop.
	 * @param fn callback function, receives the next watcher as it's first
	 * argument, and a generic pointer as it's second argument.
	 * @param arg callback function's second argument
	 */
	void iterate(int (*fn)(Watcher*, void*), void *arg);
private:
	static int _iterator(unsigned int index, void *arg);
private:
	Kmap<unsigned long long, Watcher*> watchers;
	int (*itfn)(Watcher*, void*) {};
	void *itfnarg { };
};

} /* namespace wanhive */

#endif /* WH_HUB_WATCHERS_H_ */
