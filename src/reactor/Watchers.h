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

#ifndef WH_REACTOR_WATCHERS_H_
#define WH_REACTOR_WATCHERS_H_
#include "../base/ds/Khash.h"
#include "Watcher.h"

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
	 * Checks if a key is present in the hash table.
	 * @param key key's value
	 * @return true if the key exists in the hash table, false otherwise
	 */
	bool contains(unsigned long long key) const noexcept;
	/**
	 * Returns the watcher associated with the given key in the hash table.
	 * @param key key's value
	 * @return watcher associated with the key, nullptr if key doesn't exist
	 */
	Watcher* get(unsigned long long key) const noexcept;
	/**
	 * Inserts a (key, watcher) pair into the hash table.
	 * @param key key's value
	 * @param w the watcher to associate with the given key, it's UID will be
	 * updated to match the given key.
	 * @return true on success, false otherwise
	 */
	bool put(unsigned long long key, Watcher *w) noexcept;
	/**
	 * Inserts a watcher into the hash table.
	 * @param w the watcher to insert into the hash table, it's UID will be used
	 * as the key.
	 * @return true on success, false otherwise
	 */
	bool put(Watcher *w) noexcept;
	/**
	 * Inserts a (key, watcher) pair into the hash table. If the given key exists
	 * then the associated watcher will be replaced and returned.
	 * @param key key's value
	 * @param w the watcher to associate with the given key, it's UID will be
	 * updated to match the given key.
	 * @return the old watcher in case of a conflict, nullptr otherwise
	 */
	Watcher* replace(unsigned long long key, Watcher *w) noexcept;
	/**
	 * Inserts a watcher into the hash table after resolving any conflict. if a
	 * watcher is already associated with a key equal in value to the UID of the
	 * given watcher then the existing watcher will be replaced and returned.
	 * @param w the watcher to insert into the hash table. It's UID will be used
	 * as the key.
	 * @return the old watcher in case of a conflict, nullptr otherwise
	 */
	Watcher* replace(Watcher *w) noexcept;
	/**
	 * Removes a key from the hash table.
	 * @param key key's value
	 */
	void remove(unsigned long long key) noexcept;
	/**
	 * Swaps the watchers associated with the given key-pair. If only one of the
	 * two keys exists then the existing key will be removed from the hash table
	 * and it's associated watcher will be reassigned to the non- existent key
	 * (after making a new entry for the non-existent key) and the watcher's UID
	 * will be updated to match it's new key. If both keys exist and swapping is
	 * allowed then the watchers associated with the two keys will be swapped and
	 * their UIDs will be updated to match their new keys.
	 * @param first the first key in the key pair
	 * @param second the second key in the key pair
	 * @param w if not nullptr, then the watchers associated with the first and
	 * the second key after the operation will be returned here (in that order).
	 * @param swap true to enable swapping, false otherwise
	 * @return true on success, false on failure (could not swap or neither of
	 * the two keys exists).
	 */
	bool move(unsigned long long first, unsigned long long second,
			Watcher *w[2], bool swap) noexcept;
	/**
	 * Iterates over the hash table, callback function's return value determines
	 * the actual behavior:
	 * [0]: continue iteration
	 * [1]: remove the current entry from the hash table and continue iteration
	 * [Any other value]: stop iteration
	 * @param fn the callback function
	 * @param arg additional argument (for passing on to the callback function)
	 */
	void iterate(int (*fn)(Watcher *w, void *arg), void *arg);
private:
	//Insertion point for the iterator
	static int _iterator(unsigned int index, void *arg);
private:
	Khash<unsigned long long, Watcher*> watchers; //The hash table
	int (*itfn)(Watcher *w, void *arg); //The actual iterator
	void *itfnarg; //Iterator's argument
};

} /* namespace wanhive */

#endif /* WH_REACTOR_WATCHERS_H_ */
