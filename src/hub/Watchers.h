/**
 * @file Watchers.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Hash table of watchers
 */
class Watchers {
public:
	/**
	 * Constructor: initializes an empty collection.
	 */
	Watchers() noexcept;
	/**
	 * Destructor
	 */
	~Watchers();
	/**
	 * Checks if a specified key is present in the collection.
	 * @param key the key
	 * @return true if the key exists, false otherwise
	 */
	bool contains(unsigned long long key) const noexcept;
	/**
	 * Retrieves the watcher associated with a given key.
	 * @param key the key
	 * @return the associated watcher, or nullptr if the key does not exist
	 */
	Watcher* select(unsigned long long key) const noexcept;
	/**
	 * Inserts a new (key, watcher) pair and sets the key as the identifier for
	 * the watcher.
	 * @param key the key
	 * @param watcher the watcher
	 * @return true on success, false otherwise
	 */
	bool insert(unsigned long long key, Watcher *watcher) noexcept;
	/**
	 * Inserts a new watcher and assigns its unique identifier as the key.
	 * @param watcher the watcher
	 * @return true on success, false otherwise
	 */
	bool insert(Watcher *watcher) noexcept;
	/**
	 * Inserts a (key, watcher) pair and sets the key as the identifier for the
	 * watcher. If there is a conflict with an existing key, the old watcher
	 * will be replaced and returned.
	 * @param key the key
	 * @param watcher the watcher
	 * @return the replaced watcher (nullptr if it is a new insertion)
	 */
	Watcher* replace(unsigned long long key, Watcher *watcher) noexcept;
	/**
	 * Inserts a watcher, resolving any conflicts, and assigns its unique
	 * identifier as the key. If another watcher is already associated with the
	 * given identifier, the conflicting watcher will be replaced and returned.
	 * @param watcher the watcher
	 * @return the replaced watcher (nullptr if it is a new insertion)
	 */
	Watcher* replace(Watcher *watcher) noexcept;
	/**
	 * Removes the specified key from the collection.
	 * @param key the key
	 */
	void remove(unsigned long long key) noexcept;
	/**
	 * Swaps the watchers associated with a pair of keys. If only one of the
	 * two keys exists, the existing watcher is removed and reassigned to the
	 * missing key, with the new key set as the watcher's identifier. If both
	 * keys exist and swapping is enabled, the watchers associated with the
	 * two keys will be swapped, and their respective identifiers will be
	 * updated to the new keys.
	 * @param first the first key
	 * @param second the second key
	 * @param w a reference to store the watchers associated with the given keys
	 * (in order) after a successful operation.
	 * @param swap set to true to enable swapping, or false otherwise
	 * @return true if the operation is successful, false if the swap could not
	 * be performed or if neither key exists.
	 */
	bool move(unsigned long long first, unsigned long long second,
			Watcher *(&w)[2], bool swap) noexcept;
	/**
	 * Iterates through the hash table. The callback function's return value
	 * determines the behavior:
	 * [0]: continue iteration,
	 * [1]: remove the current entry and continue iteration,
	 * [Any other value]: stop iteration.
	 * @param fn the callback function, which receives the next watcher as its
	 * first argument and a generic pointer as its second argument.
	 * @param arg the second argument for the callback function.
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
