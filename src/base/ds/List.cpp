/*
 * List.cpp
 *
 * Doubly linked list
 *
 *
 * Copyright (C) 2020 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "List.h"

namespace wanhive {

List::List() noexcept {
	clear();

}

List::~List() {
	auto node = getSuccessor();
	while (node != this) {
		auto tmp = node->getSuccessor();
		delete node;
		node = tmp;
	}
}

bool List::isEmpty() const noexcept {
	return (_size == 0);
}

unsigned int List::size() const noexcept {
	return _size;
}

bool List::insert(ListNode *node) noexcept {
	if (node && node->list(_mark)) {
		++_size;
		return true;
	} else {
		return false;
	}
}

void List::mark() noexcept {
	_mark = this;
}

void List::first() noexcept {
	_mark = getSuccessor();
}

void List::last() noexcept {
	_mark = getPredecessor();
}

void List::next() noexcept {
	_mark = _mark->getSuccessor();
}

void List::previous() noexcept {
	_mark = _mark->getPredecessor();
}

ListNode* List::current() const noexcept {
	return (_mark != this) ? _mark : nullptr;
}

void List::remove() noexcept {
	if (_mark != this) {
		auto tmp = _mark->getSuccessor();
		_mark->delist();
		_mark = tmp;
		--_size;
	}
}

void List::join(List &list) noexcept {
	if (!list.isEmpty() && (&list != this)) {
		//(_mark) -- [<list>] -- [tail]
		auto head = list.getPredecessor();
		auto tail = _mark->getSuccessor();
		ListNode::tie(head, tail);
		ListNode::tie(_mark, list.getSuccessor());
		_size += list._size;
		list.clear();
	}
}

void List::clear() noexcept {
	ListNode::clear();
	_mark = this;
	_size = 0;
}

} /* namespace wanhive */
