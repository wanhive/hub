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
	if ((topic >= Topic::COUNT) || !w) {
		return false;
	}

	int ret = 0;
	auto i = indexes.put( { w, topic }, ret);
	if (ret) { //Key was not present
		//Elements are always added at the list's end
		auto index = topics[topic].readSpace();
		indexes.setValue(i, index);
		topics[topic].put(w);
		return true;
	} else { //Key was already present
		return true;
	}
}

Watcher* Topics::get(unsigned int topic, unsigned int index) const noexcept {
	const Watcher *w = nullptr;
	if ((topic < Topic::COUNT) && topics[topic].get(w, index)) {
		return const_cast<Watcher*>(w);
	} else {
		return nullptr;
	}
}

void Topics::remove(unsigned int topic, const Watcher *w) noexcept {
	if ((topic >= Topic::COUNT) || !w) {
		return;
	}

	//Get the index to delete
	auto i = indexes.get( { w, topic });
	unsigned int index = 0;
	if (!indexes.getValue(i, index)) {
		return;
	}

	//Remove from the containers
	indexes.remove(i);
	topics[topic].remove(index);

	//Adjust replacement's index
	const Watcher *s = nullptr;
	if (!topics[topic].get(s, index)) { //The last entry
		return;
	} else {
		unsigned int tmp;
		indexes.hmReplace( { s, topic }, index, tmp);
	}
}

bool Topics::contains(unsigned int topic, const Watcher *w) const noexcept {
	return (topic < Topic::COUNT) && w && indexes.contains( { w, topic });
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
