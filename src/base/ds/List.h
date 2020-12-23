/*
 * List.h
 *
 * Doubly linked list
 *
 *
 * Copyright (C) 2020 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_LIST_H_
#define WH_BASE_DS_LIST_H_
#include "ListNode.h"

namespace wanhive {
/**
 * Doubly linked list implementation.
 * NOTE: Do not mix List and ListNode operations.
 */
class List: private ListNode {
public:
	//Creates an empty list
	List() noexcept;
	virtual ~List();

	//Returns true if the list is empty
	bool isEmpty() const noexcept;
	//Returns number of nodes in the list
	unsigned int size() const noexcept;
	/*
	 * Inserts <node> into this list. If the iterator was reset then the <node>
	 * is inserted at the front of the list. Otherwise, the node is inserted in
	 * front of the iterator.
	 */
	bool insert(ListNode *node) noexcept;
	//Resets the iterator
	void mark() noexcept;
	//Moves the iterator to the first node
	void first() noexcept;
	//Moves the iterator to the last node
	void last() noexcept;
	//Moves the iterator forward
	void next() noexcept;
	//Moves the iterator backward
	void previous() noexcept;
	//Returns the node at the iterator's current position (possibly nullptr)
	ListNode* current() const noexcept;
	/*
	 * Removes the node at iterator's current position and moves the iterator
	 * forward. If only one node existed then the iterator is reset. If the
	 * list was empty then the call results in a no-op.
	 */
	void remove() noexcept;
	/*
	 * Joins the <list> into this list after the iterator's position. If the
	 * iterator has been reset then <list> is joined in front of this list.
	 * After the operation the <list> becomes empty.
	 */
	void join(List &list) noexcept;
private:
	void clear() noexcept;
private:
	ListNode *_mark;
	unsigned int _size;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_LIST_H_ */
