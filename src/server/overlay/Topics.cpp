/*
 * Topics.cpp
 *
 * Topics management
 *
 *
 * Copyright (C) 2020 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Topics.h"

namespace wanhive {

Topics::Topics() noexcept {

}

Topics::~Topics() {

}

bool Topics::put(unsigned int topic, const Watcher *w) noexcept {
	if ((topic < Topic::COUNT) && w) {
		int ret = 0;
		unsigned int i = indexes.put( { w, topic }, ret);
		if (ret) { //Key was not present
			//Elements are always added at the end of the array
			unsigned int index = topics[topic].readSpace();
			if (indexes.setValue(i, index)) {
				topics[topic].put(w);
				return true;
			} else {
				indexes.remove(i);
				return false;
			}
		} else { //Key already present
			return true;
		}
	} else {
		return false;
	}
}

Watcher* Topics::get(unsigned int topic, unsigned int index) const noexcept {
	if (topic < Topic::COUNT) {
		const Watcher *w = nullptr;
		if (topics[topic].get(w, index)) {
			return const_cast<Watcher*>(w);
		} else {
			return nullptr;
		}
	} else {
		return nullptr;
	}
}

void Topics::remove(unsigned int topic, const Watcher *w) noexcept {
	if ((topic < Topic::COUNT) && w) {
		//Get the iterator to the key
		unsigned int i = indexes.get( { w, topic });
		if (i == indexes.end()) {
			return;
		}

		//Get the index of the element to be deleted
		unsigned int index = 0;
		if (!indexes.getValue(i, index)) {
			return;
		}

		//Remove from the list
		topics[topic].remove(index);
		topics[topic].shrink(4096);
		//Remove from the hash table
		indexes.remove(i);

		//Adjust the index of the replacement
		const Watcher *s = nullptr;
		if (!topics[topic].get(s, index)) { //The last entry
			return;
		} else {
			unsigned int tmp;
			indexes.hmReplace( { s, topic }, index, tmp);
		}
	}
}

bool Topics::contains(unsigned int topic, const Watcher *w) const noexcept {
	if ((topic < Topic::COUNT) && w) {
		unsigned int index = 0;
		const Watcher *cmp = nullptr;

		//Get the iterator to the key
		unsigned int i = indexes.get( { w, topic });
		if (i == indexes.end()) {
			return false;
		} else if (!indexes.getValue(i, index)) {
			return false;
		} else if (!topics[topic].get(cmp, index)) {
			return false;
		} else {
			return (cmp == w);
		}
	} else {
		return false;
	}
}

unsigned int Topics::count(unsigned int topic) const noexcept {
	if (topic < Topic::COUNT) {
		return topics[topic].readSpace();
	} else {
		return 0;
	}
}

void Topics::clear() noexcept {
	for (unsigned int i = 0; i < Topic::COUNT; i++) {
		topics[i].clear();
	}

	indexes.clear();
}

} /* namespace wanhive */
