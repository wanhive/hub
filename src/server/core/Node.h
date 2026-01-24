/*
 * Node.h
 *
 * Chord DHT (distributed hash table)
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_CORE_NODE_H_
#define WH_SERVER_CORE_NODE_H_
#include "DHT.h"
#include "Finger.h"

namespace wanhive {
/**
 * Chord DHT (distributed hash table) implementation
 * @ref https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf
 */
class Node {
public:
	/**
	 * Constructor: assigns an identity.
	 * @param key node's identity
	 */
	Node(unsigned int key);
	/**
	 * Destructor
	 */
	~Node();
	//-----------------------------------------------------------------
	/**
	 * Returns this node's identity.
	 * @return numeric identity
	 */
	unsigned int self() const noexcept;
	/**
	 * Returns an entry from the finger table.
	 * @param index finger table's index
	 * @return finger's pointer on success, nullptr on error (invalid index)
	 */
	const Finger* getFinger(unsigned int index) const noexcept;
	/**
	 * Returns a key from the finger table.
	 * @param index finger table's index
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
	 * @param index finger table's index
	 * @return true if consistent, false otherwise
	 */
	bool isConsistent(unsigned int index) const noexcept;
	/**
	 * Commits a finger table entry.
	 * @param index finger table's index
	 * @return replaced value
	 */
	unsigned int commit(unsigned int index) noexcept;
	/**
	 * Checks the "connected" status of a finger table entry.
	 * @param index finger table's index
	 * @return true if connected, false otherwise
	 */
	bool isConnected(unsigned int index) const noexcept;
	/**
	 * Updates the "connected" status of a finger table entry.
	 * @param index finger table's index
	 * @param status true to connect, false to disconnect
	 */
	void setConnected(unsigned int index, bool status) noexcept;
	/**
	 * Returns node's predecessor.
	 * @return predecessor's identity
	 */
	unsigned int getPredecessor() const noexcept;
	/**
	 * Sets node's predecessor.
	 * @param key predecessor's identity
	 * @return true on success, false on error
	 */
	bool setPredecessor(unsigned int key) noexcept;
	/**
	 * Checks whether the predecessor's finger is consistent.
	 * @return true if consistent, false otherwise.
	 */
	bool predessorChanged() const noexcept;
	/**
	 * Commits the predecessor's identity.
	 * @return previous predecessor
	 */
	unsigned int commitPredecessor() noexcept;
	/**
	 * Returns node's successor (equivalent to Node::get(0)).
	 * @return successor's identity
	 */
	unsigned int getSuccessor() const noexcept;
	/**
	 * Sets this node's successor (equivalent to Node::set(0, id)).
	 * @param key successor's identity
	 * @return true on success, false on error
	 */
	bool setSuccessor(unsigned int key) noexcept;
	/**
	 * Checks whether the finger table is in "stable" state.
	 * @return true if the finger table is stable, false otherwise
	 */
	bool isStable() const noexcept;
	/**
	 * Updates finger table's "stable" state.
	 * @param stable true for stable, false for unstable
	 */
	void setStable(bool stable) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Recursive routing: checks whether this node is a key's root.
	 * @param key key's value
	 * @return true if this node is key's root, false otherwise
	 */
	bool isLocal(unsigned int key) const noexcept;
	/**
	 * Recursive routing: calculates the next hop in lookup for a key.
	 * @param key key's value
	 * @return next hop's identity on success, this node's identity on error
	 */
	unsigned int nextHop(unsigned int key) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a key's local root.
	 * @param key key's value
	 * @return this node's immediate successor if that is the root, 0 otherwise
	 */
	unsigned int localSuccessor(unsigned int key) const noexcept;
	/**
	 * Returns the closest predecessor of a key.
	 * @param key key's value
	 * @param checkConnected true to skip disconnected fingers, false otherwise
	 * @return closest predecessor's identity on success, this node's identity
	 * on error
	 */
	unsigned int closestPredecessor(unsigned int key, bool checkConnected =
			false) const noexcept;
	/**
	 * Joins this node to a distributed hash table (DHT).
	 * @param key bootstrapping key
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
	 * @param key predecessor's identity
	 * @return true on success (predecessor updated), false otherwise
	 */
	bool notify(unsigned int key) noexcept;
	/**
	 * Updates the finger table entries associated with a key. On finger table's
	 * update, this node is marked as unstable.
	 * @param key key's value
	 * @param joined true if the key was added to the network, false if the key
	 * was removed from the network.
	 * @return true if the finger table got updated, false otherwise
	 */
	bool update(unsigned int key, bool joined) noexcept;
	/**
	 * Checks if the finger table contains a key.
	 * @param key key's value
	 * @return true if the key exists in the finger table (includes 0 and
	 * this node's identity), false otherwise.
	 */
	bool isInRoute(unsigned int key) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * For testing: prints this node's information on stderr.
	 */
	void print() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks whether key belongs to an open circular interval.
	 * @param key key's value
	 * @param from lower bound
	 * @param to upper bound
	 * @return true if key is in the interval, false otherwise
	 */
	static bool isBetween(unsigned int key, unsigned int from,
			unsigned int to) noexcept;
	/**
	 * Checks whether key belongs to a closed circular interval.
	 * @param key key's value
	 * @param from lower bound
	 * @param to upper bound
	 * @return true if key is in the interval, false otherwise
	 */
	static bool isInRange(unsigned int key, unsigned int from,
			unsigned int to) noexcept;
	/**
	 * Calculates a key's successor on the identifier ring.
	 * @param key key's value
	 * @param index successor's index
	 * @return successor's identity
	 */
	static unsigned int successor(unsigned int key, unsigned int index) noexcept;
	/**
	 * Calculates a key's predecessor on the identifier ring.
	 * @param key key's value
	 * @param index predecessor's index
	 * @return predecessor's identity
	 */
	static unsigned int predecessor(unsigned int key,
			unsigned int index) noexcept;
private:
	void initialize() noexcept;
	bool setFinger(Finger &f, unsigned int key, bool checkConsistent = true,
			bool checkConnected = true) noexcept;
public:
	/*! Identifier length in bits */
	static constexpr unsigned int KEYLENGTH = DHT::KEY_LENGTH;
	/*! Maximum number of nodes in the identifier ring */
	static constexpr unsigned long MAX_NODES = ((1UL << KEYLENGTH));
	/*! Controller's identifier, must be 0 */
	static constexpr unsigned int CONTROLLER = 0;
	/*! Minimum value on the identifier ring */
	static constexpr unsigned int MIN_ID = 1;
	/*! Maximum value on the identifier ring */
	static constexpr unsigned int MAX_ID = ((1UL << KEYLENGTH) - 1);
	/*! Number of finger table entries */
	static constexpr unsigned int TABLESIZE = KEYLENGTH;
private:
	const unsigned int _key;
	Finger _predecessor;
	Finger table[TABLESIZE];
	bool stable;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_NODE_H_ */
