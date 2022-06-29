/*
 * Node.cpp
 *
 * Chord (distributed hash table) implementation
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Node.h"
#include "../../base/common/Exception.h"
#include "../../base/ds/Twiddler.h"
#include <cstdio>

namespace wanhive {

Node::Node(unsigned int key) :
		_key(key) {
	if (key <= MAX_ID) {
		initialize();
	} else {
		throw Exception(EX_INVALIDPARAM);
	}
}

Node::~Node() {

}

unsigned int Node::getKey() const noexcept {
	return _key;
}

const Finger* Node::getFinger(unsigned int index) const noexcept {
	if (index < TABLESIZE) {
		return &table[index];
	} else {
		return nullptr;
	}
}

unsigned int Node::get(unsigned int index) const noexcept {
	if (index < TABLESIZE) {
		return table[index].getId();
	} else {
		return 0;
	}
}

bool Node::set(unsigned int index, unsigned int key) noexcept {
	if (index < TABLESIZE) {
		return setFinger(table[index], key);
	} else {
		return false;
	}
}

bool Node::isConsistent(unsigned int index) const noexcept {
	if (index < TABLESIZE) {
		return table[index].isConsistent();
	} else {
		return false;
	}
}

unsigned int Node::commit(unsigned int index) noexcept {
	if (index < TABLESIZE) {
		return table[index].commit();
	} else {
		return 0;
	}
}

bool Node::isConnected(unsigned int index) const noexcept {
	if (index < TABLESIZE) {
		return table[index].isConnected();
	} else {
		return false;
	}
}

void Node::setConnected(unsigned int index, bool status) noexcept {
	if (index < TABLESIZE) {
		table[index].setConnected(status);
	}
}

unsigned int Node::getPredecessor() const noexcept {
	return _predecessor.getId();
}

bool Node::setPredecessor(unsigned int key) noexcept {
	return setFinger(_predecessor, key, false, false);
}

bool Node::predessorChanged() const noexcept {
	return _predecessor.getId() && !_predecessor.isConsistent();
}

unsigned int Node::commitPredecessor() noexcept {
	return _predecessor.commit();
}

unsigned int Node::getSuccessor() const noexcept {
	return get(0);
}

bool Node::setSuccessor(unsigned int key) noexcept {
	return set(0, key);
}

bool Node::isStable() const noexcept {
	return stable;
}

void Node::setStable(bool stable) noexcept {
	this->stable = stable;
}

bool Node::isLocal(unsigned int key) const noexcept {
	//(key) E (predecessor, serverId]
	return (isBetween(key, getPredecessor(), getKey()) || (key == getKey()));
}

unsigned int Node::nextHop(unsigned int key) const noexcept {
	auto n = localSuccessor(key);
	if (n == 0) {
		n = closestPredecessor(key, true);
	}
	return n;
}

unsigned int Node::localSuccessor(unsigned int key) const noexcept {
	auto successor = getSuccessor();
	if (isBetween(key, getKey(), successor) || (key == successor)) {
		return successor;
	} else {
		return 0;
	}
}

unsigned int Node::closestPredecessor(unsigned int key,
		bool checkConnected) const noexcept {
	for (int i = (TABLESIZE - 1); i >= 0; --i) {
		auto f = table[i].getId();
		if (isBetween(f, getKey(), key)
				&& (!checkConnected || table[i].isConnected())) {
			return f;
		}
	}
	return getKey();
}

bool Node::join(unsigned int key) noexcept {
	return setPredecessor(0) && setSuccessor(key);
}

bool Node::stabilize(unsigned int key) noexcept {
	if (key != 0 && isBetween(key, getKey(), getSuccessor())) {
		return setSuccessor(key);
	} else {
		return true;
	}
}

bool Node::notify(unsigned int key) noexcept {
	auto predecessor = getPredecessor();
	if (predecessor == 0 || isBetween(key, predecessor, getKey())) {
		return setPredecessor(key);
	} else {
		return false;
	}
}

bool Node::update(unsigned int key, bool joined) noexcept {
	auto found = false;
	//If the predecessor has failed then invalidate the predecessor
	if (getPredecessor() == key && !joined) {
		setPredecessor(0);
		found = true;
	}

	//Update the finger table
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		if (table[i].getId() == key) {
			table[i].setConnected(joined);
			found = true;
		}
	}
	return found;
}

bool Node::isInRoute(unsigned int key) const noexcept {
	if (key == getKey() || key == CONTROLLER) {
		return true;
	}

	//Check the finger table
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		if (table[i].getId() == key) {
			return true;
		}
	}
	return false;
}

void Node::print() noexcept {
	fprintf(stderr, "\n==========================================\n");
	fprintf(stderr, "KEY: %u\n", getKey());
	fprintf(stderr, "PREDECESSOR: %u, SUCCESSOR: %u\n\n", getPredecessor(),
			getSuccessor());
	fprintf(stderr, "FINGER TABLE [STABLE: %s]\n", WH_BOOLF(isStable()));
	fprintf(stderr, "------------------------------------------\n");
	fprintf(stderr, " SN    START  CURRENT  HISTORY   CONNECTED\n");
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		auto f = getFinger(i);
		fprintf(stderr, "%3u%9u%9u%9u   %9s\n", (i + 1), f->getStart(),
				f->getId(), f->getOldId(), WH_BOOLF(f->isConnected()));
	}
	fprintf(stderr, "\n==========================================\n");
}

bool Node::isBetween(unsigned int key, unsigned int from,
		unsigned int to) noexcept {
	//<key> = Node::successor(<ret-val> , <index>))
	return (key <= MAX_ID) && Twiddler::isBetween(key, from, to);
}

bool Node::isInRange(unsigned int key, unsigned int from,
		unsigned int to) noexcept {
	return (key <= MAX_ID) && Twiddler::isInRange(key, from, to);
}

unsigned int Node::successor(unsigned int uid, unsigned int index) noexcept {
	return (uid + (1UL << index)) & MAX_ID;
}

unsigned int Node::predecessor(unsigned int key, unsigned int index) noexcept {
	return (key - (1UL << index)) & MAX_ID;
}

void Node::initialize() noexcept {
	//For correct routing on a stand-alone server (don't touch)
	setPredecessor(getKey());
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		table[i].setStart(successor(getKey(), i));
		table[i].setId(getKey());
		table[i].commit();
		table[i].setConnected(false);
	}
	setStable(true);
}

bool Node::setFinger(Finger &f, unsigned int key, bool checkConsistent,
		bool checkConnected) noexcept {
	if ((key <= MAX_ID) && (!checkConsistent || f.isConsistent())) {
		auto old = f.getId();
		f.setId(key);

		if (key && ((checkConnected && !f.isConnected()) || old != f.getId())) {
			setStable(false);
		}
		return true;
	} else {
		return false;
	}
}

} /* namespace wanhive */
