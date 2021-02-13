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

bool Node::set(unsigned int index, unsigned int id) noexcept {
	if (index < TABLESIZE) {
		return setFinger(table[index], id);
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

unsigned int Node::makeConsistent(unsigned int index) noexcept {
	unsigned int oldId = 0;
	if (index < TABLESIZE) {
		oldId = table[index].getOldId();
		table[index].makeConsistent();
	}
	return oldId;
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

bool Node::setPredecessor(unsigned int id) noexcept {
	return setFinger(_predecessor, id, false, false);
}

bool Node::predessorChanged() const noexcept {
	return _predecessor.getId() && !_predecessor.isConsistent();
}

void Node::makePredecessorConsistent() noexcept {
	_predecessor.makeConsistent();
}

unsigned int Node::getSuccessor() const noexcept {
	return get(0);
}

bool Node::setSuccessor(unsigned int id) noexcept {
	return set(0, id);
}

bool Node::isStable() const noexcept {
	return stable;
}

void Node::setStable(bool stable) noexcept {
	this->stable = stable;
}

bool Node::isBetween(unsigned int key, unsigned int from,
		unsigned int to) noexcept {
	return (key <= MAX_ID) && Twiddler::isBetween(key, from, to);
}

bool Node::isInRange(unsigned int key, unsigned int from,
		unsigned int to) noexcept {
	return (key <= MAX_ID) && Twiddler::isInRange(key, from, to);
}

unsigned int Node::successor(unsigned int uid, unsigned int index) noexcept {
	return (uid + (1UL << index)) & MAX_ID;
}

unsigned int Node::predecessor(unsigned int uid, unsigned int index) noexcept {
	return (uid - (1UL << index)) & MAX_ID;
}

bool Node::isLocal(unsigned int id) const noexcept {
	//(key) E (predecessor, serverId]
	return (isBetween(id, getPredecessor(), _key) || (id == _key));
}

unsigned int Node::nextHop(unsigned int id) const noexcept {
	auto n = localSuccessor(id);
	if (n == 0) {
		n = closestPredecessor(id, true);
	}
	return n;
}

unsigned int Node::localSuccessor(unsigned int id) const noexcept {
	auto successor = getSuccessor();
	if (isBetween(id, _key, successor) || (id == successor)) {
		return successor;
	} else {
		return 0;
	}
}

unsigned int Node::closestPredecessor(unsigned int id,
		bool checkConnected) const noexcept {
	for (int i = (TABLESIZE - 1); i >= 0; --i) {
		auto f = table[i].getId();
		if (isBetween(f, _key, id)
				&& (!checkConnected || table[i].isConnected())) {
			return f;
		}
	}
	return _key;
}

void Node::join(unsigned int id) noexcept {
	setPredecessor(0);
	setSuccessor(id);
}

bool Node::stabilize(unsigned int id) noexcept {
	if (id != 0 && isBetween(id, _key, getSuccessor())) {
		return setSuccessor(id);
	} else {
		return true;
	}
}

bool Node::notify(unsigned int id) noexcept {
	auto predecessor = getPredecessor();
	if (predecessor == 0 || isBetween(id, predecessor, _key)) {
		return setPredecessor(id);
	} else {
		return false;
	}
}

bool Node::update(unsigned int id, bool joined) noexcept {
	if (joined) {
		return true;
	} else {
		return removeNode(id);
	}
}

bool Node::isInRoute(unsigned int id) const noexcept {
	if (id == _key || id == CONTROLLER) {
		return true;
	}
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		//Check whether the finger table contains <id>
		if (table[i].getId() == id) {
			return true;
		}
	}
	return false;
}

void Node::print() noexcept {
	fprintf(stderr, "\n=======================================\n");
	fprintf(stderr, "NODE SETTINGS\n");
	fprintf(stderr, "KEY: %u\n", _key);
	fprintf(stderr, "PREDECESSOR: %u, SUCCESSOR: %u\n", getPredecessor(),
			getSuccessor());
	fprintf(stderr, "FINGER TABLE [STATUS= %s]:\n",
			(isStable() ? "STABLE" : "UNSTABLE"));
	fprintf(stderr, "------------------------------------\n");
	fprintf(stderr, " SN    START  CURRENT  HISTORY   CONN\n");
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		auto f = getFinger(i);
		fprintf(stderr, "%3u%9u%9u%9u   %4s\n", (i + 1), f->getStart(),
				f->getId(), f->getOldId(), (f->isConnected() ? "Y" : "N"));
	}
	fprintf(stderr, "\n=======================================\n");
}

void Node::initialize() noexcept {
	//For correct routing on a stand-alone server (don't touch)
	setPredecessor(_key);
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		table[i].setStart(successor(_key, i));
		table[i].setId(_key);
		table[i].makeConsistent();
		table[i].setConnected(false);
	}
	setStable(true);
}

bool Node::setFinger(Finger &f, unsigned int id, bool checkConsistent,
		bool checkConnected) noexcept {
	if ((id <= MAX_ID) && (!checkConsistent || f.isConsistent())) {
		auto old = f.getId();
		f.setId(id);

		if (id && ((checkConnected && !f.isConnected()) || old != f.getId())) {
			setStable(false);
		}
		return true;
	} else {
		return false;
	}
}

bool Node::removeNode(unsigned int id) noexcept {
	auto found = false;
	//If the predecessor has failed/disconnected then invalidate the predecessor
	if (getPredecessor() == id) {
		setPredecessor(0);
		found = true;
	}
	//If a finger has failed then set it's connected status to false
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		if (table[i].getId() == id) {
			table[i].setConnected(false);
			found = true;
		}
	}
	return found;
}

} /* namespace wanhive */
