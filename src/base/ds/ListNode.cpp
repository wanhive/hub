/*
 * ListNode.cpp
 *
 * Node of a doubly linked list
 *
 *
 * Copyright (C) 2020 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "ListNode.h"

namespace wanhive {

ListNode::ListNode() noexcept {
	clear();
}

ListNode::~ListNode() {

}

bool ListNode::list(ListNode *head) noexcept {
	if (!isListed() && head) {
		// --(head)-[this]-[n1]-[n2]--
		setPredecessor(head);
		setSuccessor(head->getSuccessor());
		head->getSuccessor()->setPredecessor(this);
		head->setSuccessor(this);
		listed = true;
		return true;
	} else {
		return false;
	}
}

void ListNode::delist() noexcept {
	if (isListed()) {
		getSuccessor()->setPredecessor(getPredecessor());
		getPredecessor()->setSuccessor(getSuccessor());
		clear();
	}
}

bool ListNode::isListed() const noexcept {
	return listed;
}

ListNode* ListNode::getPredecessor() const noexcept {
	return contiguous[0];
}

ListNode* ListNode::getSuccessor() const noexcept {
	return contiguous[1];
}

void ListNode::setPredecessor(ListNode *node) noexcept {
	if (node) {
		contiguous[0] = node;
	} else {
		contiguous[0] = this;
	}
}

void ListNode::setSuccessor(ListNode *node) noexcept {
	if (node) {
		contiguous[1] = node;
	} else {
		contiguous[1] = this;
	}
}

void ListNode::clear() noexcept {
	listed = false;
	contiguous[0] = this;
	contiguous[1] = this;
}

bool ListNode::tie(ListNode *n1, ListNode *n2) noexcept {
	if (n1 && n2) {
		n1->setSuccessor(n2);
		n2->setPredecessor(n1);
		return true;
	} else {
		return false;
	}
}

} /* namespace wanhive */
