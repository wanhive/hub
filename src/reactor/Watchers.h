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
 * Thread safe at class level
 */
class Watchers {
public:
	Watchers() noexcept;
	~Watchers();
	//Returns true if a watcher is registered with the given key
	bool contains(unsigned long long key) const noexcept;
	//Returns true is watcher is present in the hash table (UID used as the key)
	bool contains(const Watcher *w) const noexcept;
	//Returns the watcher (or nullptr) registered with the given key
	Watcher* get(unsigned long long uid) const noexcept;
	/*
	 * Inserts a watcher with the given <key> into the hash table if the key
	 * doesn't exist. On success function returns true and watcher's uid is
	 * set to <key>.
	 */
	bool put(unsigned long long key, Watcher *w) noexcept;
	/*
	 * Inserts a watcher into the hash table with it's uid as the key if the key
	 * doesn't exist. Returns true on success, false otherwise (key already exists).
	 */
	bool put(Watcher *w) noexcept;
	/*
	 * Same as put, however if another watcher is already registered with the key
	 * equal to uid of <w> then the old one is kicked out and returned (nullptr is
	 * returned otherwise).
	 */
	Watcher* replace(Watcher *w) noexcept;
	//Removes the watcher with the given key from the hash table
	void remove(unsigned long long key) noexcept;
	//Removes the watcher from the hash table using it's UID as the key
	void remove(const Watcher *w) noexcept;
	/*
	 * If the <first> watcher exists in the hash table and the <second> doesn't
	 * exist then the <first> watcher is re-registered as the <second> and vice
	 * versa. Moved watcher's UID is updated to match the key at the new position.
	 * If both <first> and <second> watchers exist and if <swap> is true then
	 * the two watchers are swapped, their UIDS are updated to match the keys
	 * at their new positions. On failure (could not swap or neither of the two
	 * watchers exists) false is returned, otherwise true is returned. Watchers
	 * associated with the given UIDs are returned back to the caller via <w>.
	 */
	bool move(unsigned long long first, unsigned long long second,
			Watcher *w[2], bool swap) noexcept;
	/*
	 * Iterate over the hash table, return values of <fn>:
	 * [0]: Continue iteration
	 * [1]: Remove from the hash table and continue
	 * [Any other value]: Stop iteration
	 */
	void iterate(int (*fn)(Watcher *w, void *arg), void *arg);
private:
	Khash<unsigned long long, Watcher*> watchers;
	int (*itfn)(Watcher *w, void *arg); //The actual iterator
	void *itfnarg;						//Iterator argument
	//Calls <itfn> internally
	static int _iterator(unsigned int index, void *arg);
};

} /* namespace wanhive */

#endif /* WH_REACTOR_WATCHERS_H_ */
