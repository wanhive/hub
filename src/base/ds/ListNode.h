/*
 * ListNode.h
 *
 * Node of a doubly linked list
 *
 *
 * Copyright (C) 2020 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_LISTNODE_H_
#define WH_BASE_DS_LISTNODE_H_

namespace wanhive {
/**
 * Node of a doubly linked list
 */
class ListNode {
public:
	ListNode() noexcept;
	virtual ~ListNode();
	//-----------------------------------------------------------------
	/*
	 * Inserts the node into a linked list after the <head>. Returns true on
	 * success, false otherwise. Doesn't modify the ListNode::listed field of
	 * the <head> (allows the <head> to mark the list boundaries).
	 */
	bool link(ListNode *head) noexcept;
	/*
	 * Removes the node from a linked list. Returns the pointer to this node
	 * if it was in a list, nullptr otherwise.
	 */
	ListNode* delink() noexcept;
	//Returns true if this node is in a linked list.
	bool inList() const noexcept;
	//-----------------------------------------------------------------
	//Returns the predecessor (nullptr if no predecessor)
	ListNode* getPredecessor() const noexcept;
	//Sets <node> as the predecessor (silently fails if <node> is nullptr)
	void setPredecessor(ListNode *node) noexcept;
	//Returns the successor (nullptr if no successor)
	ListNode* getSuccessor() const noexcept;
	//Sets <node> as the successor (silently fails if <node> is nullptr)
	void setSuccessor(ListNode *node) noexcept;
	//-----------------------------------------------------------------
protected:
	//Resets the object to it's initial state
	void reset() noexcept;
private:
	bool listed;
	ListNode *contiguous[2];
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_LISTNODE_H_ */
