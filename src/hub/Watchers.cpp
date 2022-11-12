/*
 * Watchers.cpp
 *
 * Hash table of watchers
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Watchers.h"

namespace wanhive {

Watchers::Watchers() noexcept {

}

Watchers::~Watchers() {

}

bool Watchers::contains(unsigned long long key) const noexcept {
	return watchers.contains(key);
}

Watcher* Watchers::get(unsigned long long key) const noexcept {
	Watcher *w = nullptr;
	watchers.hmGet(key, w);
	return w;
}

bool Watchers::put(unsigned long long key, Watcher *w) noexcept {
	if (w && watchers.hmPut(key, w)) {
		w->setUid(key);
		return true;
	} else {
		return false;
	}
}

bool Watchers::put(Watcher *w) noexcept {
	return w && put(w->getUid(), w);
}

Watcher* Watchers::replace(unsigned long long key, Watcher *w) noexcept {
	if (w) {
		Watcher *old = nullptr;
		watchers.hmReplace(key, w, old);
		w->setUid(key);
		return old;
	} else {
		return nullptr;
	}
}

Watcher* Watchers::replace(Watcher *w) noexcept {
	if (w) {
		Watcher *old = nullptr;
		watchers.hmReplace(w->getUid(), w, old);
		return old;
	} else {
		return nullptr;
	}
}

void Watchers::remove(unsigned long long key) noexcept {
	watchers.removeKey(key);
}

bool Watchers::move(unsigned long long first, unsigned long long second,
		Watcher *(&w)[2], bool swap) noexcept {

	unsigned int iterators[2];
	auto success = watchers.hmSwap(first, second, iterators, swap);

	w[0] = nullptr;
	w[1] = nullptr;
	watchers.getValue(iterators[0], w[0]);
	watchers.getValue(iterators[1], w[1]);

	if (success) {
		if (w[0]) {
			w[0]->setUid(first);
		}

		if (w[1]) {
			w[1]->setUid(second);
		}
	}

	return success;
}

void Watchers::iterate(int (*fn)(Watcher*, void*), void *arg) {
	itfn = fn;
	itfnarg = arg;
	watchers.iterate(Watchers::_iterator, this);
}

int Watchers::_iterator(unsigned int index, void *arg) {
	auto ws = static_cast<Watchers*>(arg);
	if (ws->itfn) {
		Watcher *w = nullptr;
		ws->watchers.getValue(index, w);
		return ws->itfn(w, ws->itfnarg);
	} else {
		return -1;
	}
}

} /* namespace wanhive */
