/*
 * Node.h
 *
 * Chord (distributed hash table) implementation
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_NODE_H_
#define WH_SERVER_OVERLAY_NODE_H_
#include "DHT.h"
#include "Finger.h"

namespace wanhive {
/**
 * Chord (distributed hash table) implementation
 * @ref https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf
 */
class Node {
public:
	/**
	 * Constructor: assigns an identifier to this node.
	 * @param key identifier's value (should not be greater than Node::MAX_ID)
	 */
	Node(unsigned int key);
	/**
	 * Destructor
	 */
	~Node();
	//-----------------------------------------------------------------
	/**
	 * Returns this node's identifier.
	 * @return this node's identifier
	 */
	unsigned int getKey() const noexcept;
	/**
	 * Returns an entry from the finger table.
	 * @param index finger table's index (should be less than Node::TABLESIZE)
	 * @return finger's pointer on success, nullptr on error (invalid index)
	 */
	const Finger* getFinger(unsigned int index) const noexcept;
	/**
	 * Returns a key from the finger table.
	 * @param index finger table's index (should be less than Node::TABLESIZE)
	 * @return key's value on success, 0 on error (invalid index)
	 */
	unsigned int get(unsigned int index) const noexcept;
	/**
	 * Sets a key in the finger table.
	 * @param index finger table's index (should be less than Node::TABLESIZE)
	 * @param key key's value (should not be greater than Node::MAX_ID)
	 * @return true on successful update, false on error (invalid index or key)
	 */
	bool set(unsigned int index, unsigned int key) noexcept;
	/**
	 * Checks whether an entry in the finger table is consistent.
	 * @param index finger table's index (should be less than Node::TABLESIZE)
	 * @return true if the finger at the given index is consistent, false if the
	 * finger is not consistent or invalid index.
	 */
	bool isConsistent(unsigned int index) const noexcept;
	/**
	 * Commits a finger table entry.
	 * @param index finger table's index (should be less than Node::TABLESIZE)
	 * @return old key's value before the update
	 */
	unsigned int commit(unsigned int index) noexcept;
	/**
	 * Checks the "connected" status of a finger table entry.
	 * @param index finger table's index (should be less than Node::TABLESIZE)
	 * @return true if connected, false if not connected or invalid index
	 */
	bool isConnected(unsigned int index) const noexcept;
	/**
	 * Updates the "connected" status of a finger table entry.
	 * @param index finger table's index (should be less than Node::TABLESIZE)
	 * @param status true if connected, false if not connected
	 */
	void setConnected(unsigned int index, bool status) noexcept;
	/**
	 * Returns this node's predecessor.
	 * @return predecessor's identifier
	 */
	unsigned int getPredecessor() const noexcept;
	/**
	 * Sets this node's predecessor.
	 * @param key predecessor's identifier
	 * @return true on success, false on error (invalid identifier)
	 */
	bool setPredecessor(unsigned int key) noexcept;
	/**
	 * Checks whether the predecessor's finger is consistent.
	 * @return true if the predecessor is consistent, false otherwise.
	 */
	bool predessorChanged() const noexcept;
	/**
	 * Commits the predecessor's identifier.
	 * @return previous predecessor's value
	 */
	unsigned int commitPredecessor() noexcept;
	/**
	 * Returns this node's successor (equivalent to Node::get(0)).
	 * @return successor's identifier
	 */
	unsigned int getSuccessor() const noexcept;
	/**
	 * Sets this node's successor (equivalent to Node::set(0, id)).
	 * @param key successor's identifier
	 * @return true on success, false on error (invalid identifier)
	 */
	bool setSuccessor(unsigned int key) noexcept;
	/**
	 * Checks if the finger table in "stable" state.
	 * @return true if the finger table is stable, false otherwise
	 */
	bool isStable() const noexcept;
	/**
	 * Updates the finger table's "stable" state.
	 * @param stable true for stable, false for not-stable
	 */
	void setStable(bool stable) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Recursive routing: checks whether this node the given key's root.
	 * @param key key's value
	 * @return true if this node is the given key's root, false otherwise
	 */
	bool isLocal(unsigned int key) const noexcept;
	/**
	 * Recursive routing: calculates the next hop destination in lookup for the
	 * given key.
	 * @param key key's value
	 * @return next hop's identifier on success, this node's identifier on error
	 */
	unsigned int nextHop(unsigned int key) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the given key's local root.
	 * @param key key's value
	 * @return this node's immediate successor's identifier if that is the given
	 * key's root, 0 otherwise.
	 */
	unsigned int localSuccessor(unsigned int key) const noexcept;
	/**
	 * Returns the closest predecessor of the given key.
	 * @param key key's value
	 * @param checkConnected true to skip the disconnected finger table entries,
	 * false otherwise.
	 * @return closest predecessor's identifier on success, this node's identifier
	 * on error.
	 */
	unsigned int closestPredecessor(unsigned int key, bool checkConnected =
			false) const noexcept;
	/**
	 * Joins this node to the distributed hash table using the given key.
	 * @param key bootstrapping key's value
	 * @return true on success, false otherwise
	 */
	bool join(unsigned int key) noexcept;
	/**
	 * Stabilizes this node.
	 * @param key current successor's predecessor
	 * @return true on success, false on error
	 */
	bool stabilize(unsigned int key) noexcept;
	/**
	 * Notifies this node about a potential predecessor.
	 * @param key predecessor's identifier
	 * @return true on success (predecessor updated), false otherwise
	 */
	bool notify(unsigned int key) noexcept;
	/**
	 * Updates the finger table entries associated with the given key. On finger
	 * table's update, this node is marked as unstable (see Node::isStable()).
	 * @param key key's value
	 * @param joined true if the key was added to the network, false if the key
	 * was removed from the network.
	 * @return true if the finger table got updated, false otherwise
	 */
	bool update(unsigned int key, bool joined) noexcept;
	/**
	 * Checks if the finger table contains the given key.
	 * @param key key's value
	 * @return true if the given key exists in the finger table (includes 0 and
	 * this node's identifier), false otherwise.
	 */
	bool isInRoute(unsigned int key) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For testing: prints this node's information on stderr.
	 */
	void print() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether the given key is inside an open circular interval.
	 * @param key key's value
	 * @param from circular interval's lower bound
	 * @param to circular interval's uper bound
	 * @return true if the key belongs to the open circular interval, false
	 * otherwise.
	 */
	static bool isBetween(unsigned int key, unsigned int from,
			unsigned int to) noexcept;
	/**
	 * Checks whether the given key is inside a closed circular interval.
	 * @param key key's value
	 * @param from circular interval's lower bound
	 * @param to circular interval's uper bound
	 * @return true if the key belongs to the closed circular interval, false
	 * otherwise.
	 */
	static bool isInRange(unsigned int key, unsigned int from,
			unsigned int to) noexcept;
	/**
	 * Calculates a key's successor on the identifier ring.
	 * @param key key's value
	 * @param index successor's index
	 * @return successor's value
	 */
	static unsigned int successor(unsigned int key, unsigned int index) noexcept;
	/**
	 * Calculates a key's predecessor on the identifier ring.
	 * @param key key's value
	 * @param index predecessor's index
	 * @return predecessor's value
	 */
	static unsigned int predecessor(unsigned int key,
			unsigned int index) noexcept;
private:
	void initialize() noexcept;
	bool setFinger(Finger &f, unsigned int key, bool checkConsistent = true,
			bool checkConnected = true) noexcept;
public:
	/** Identifier length in bits */
	static constexpr unsigned int KEYLENGTH = DHT::KEY_LENGTH;
	/** Maximum number of nodes in the identifier ring */
	static constexpr unsigned long MAX_NODES = ((1UL << KEYLENGTH));
	/** Controller's identifier, must be 0 */
	static constexpr unsigned int CONTROLLER = 0;
	/** Minimum value on the identifier ring */
	static constexpr unsigned int MIN_ID = 1;
	/** Maximum value on the identifier ring */
	static constexpr unsigned int MAX_ID = ((1UL << KEYLENGTH) - 1);
	/** Number of finger table entries */
	static constexpr unsigned int TABLESIZE = KEYLENGTH;
private:
	const unsigned int _key;
	Finger _predecessor;
	Finger table[TABLESIZE];
	bool stable;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_NODE_H_ */
