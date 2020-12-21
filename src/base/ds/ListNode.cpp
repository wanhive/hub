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
	reset();
}

ListNode::~ListNode() {

}

bool ListNode::link(ListNode *head) noexcept {
	if (!inList() && head) {
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

ListNode* ListNode::delink() noexcept {
	if (inList()) {
		getSuccessor()->setPredecessor(getPredecessor());
		getPredecessor()->setSuccessor(getSuccessor());
		reset();
		return this;
	} else {
		return nullptr;
	}
}

bool ListNode::inList() const noexcept {
	return listed;
}

ListNode* ListNode::getPredecessor() const noexcept {
	return contiguous[0];
}

void ListNode::setPredecessor(ListNode *node) noexcept {
	if (node) {
		contiguous[0] = node;
	}
}

ListNode* ListNode::getSuccessor() const noexcept {
	return contiguous[1];
}

void ListNode::setSuccessor(ListNode *node) noexcept {
	if (node) {
		contiguous[1] = node;
	}
}

void ListNode::reset() noexcept {
	listed = false;
	contiguous[0] = this;
	contiguous[1] = this;
}

} /* namespace wanhive */
