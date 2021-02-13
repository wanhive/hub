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
 * Ref: https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf
 * Values of Identifiers passed to the functions must remain within the range
 * of the DHT Key Space [0, MAX_ID].
 */
class Node {
public:
	Node(unsigned int key);
	~Node();
	unsigned int getKey() const noexcept;
	//=================================================================
	/**
	 * Finger table management primitives
	 */
	//Returns a reference to finger at the given <index>
	const Finger* getFinger(unsigned int index) const noexcept;
	//Returns the key at the given <index>
	unsigned int get(unsigned int index) const noexcept;
	//Sets a key <id> at the given <index>
	bool set(unsigned int index, unsigned int id) noexcept;
	//Was the modification in key at given <index> committed
	bool isConsistent(unsigned int index) const noexcept;
	//Commits the change in key's value at give <index> and returns the old value
	unsigned int makeConsistent(unsigned int index) noexcept;
	//Is the finger at given <index> in connected state
	bool isConnected(unsigned int index) const noexcept;
	//Updates the connected <status> of the finger at the given <index>
	void setConnected(unsigned int index, bool status) noexcept;

	//Returns this node's predecessor
	unsigned int getPredecessor() const noexcept;
	//Sets this node's predecessor
	bool setPredecessor(unsigned int id) noexcept;
	//Was the value of predecessor changed but not committed
	bool predessorChanged() const noexcept;
	//Commit the last change in predecessor's value
	void makePredecessorConsistent() noexcept;

	//Returns this node's successor
	unsigned int getSuccessor() const noexcept;
	//Set this node's successor
	bool setSuccessor(unsigned int id) noexcept;

	//Is the finger table in a stable/consistent state
	bool isStable() const noexcept;
	//Set finger table's <stable> state
	void setStable(bool stable) noexcept;

	//Checks whether <key>  is inside the *CIRCULAR* interval (<from>, <to>)
	static bool isBetween(unsigned int key, unsigned int from,
			unsigned int to) noexcept;
	//Checks whether <key> is inside the *CIRCULAR* interval [<from>, <to>]
	static bool isInRange(unsigned int key, unsigned int from,
			unsigned int to) noexcept;

	//Returns successor(<uid> + 2^<index>) on the identifier ring
	static unsigned int successor(unsigned int uid, unsigned int index) noexcept;
	//Returns a value such that <uid> = successor(<value> + 2^<index>) on the identifier ring
	static unsigned int predecessor(unsigned int uid,
			unsigned int index) noexcept;
	//=================================================================
	/**
	 * Modified Chord DHT Recursive Routing Routines
	 */
	//Returns true if this node is the root of <id>
	bool isLocal(unsigned int id) const noexcept;
	/*
	 * Equivalent to the find_successor algorithm of Chord DHT
	 * Finds next hop in the lookup for destination <id>,
	 * customized for recursive routing.
	 */
	unsigned int nextHop(unsigned int id) const noexcept;
	//=================================================================
	/**
	 * Modified Chord DHT Stabilization Routines: WIKI
	 * IDs passed to the functions must strictly remain within
	 * the DHT Key Space [0, MAX_ID]
	 */

	/*
	 * Returns this node's immediate successor if that is the root of <id>,
	 * returns 0 otherwise
	 */
	unsigned int localSuccessor(unsigned int id) const noexcept;
	/*
	 * Returns the highest predecessor of <id> in the local finger table
	 * If <checkConnected> is true, then returns an entry which is connected.
	 */
	unsigned int closestPredecessor(unsigned int id,
			bool checkConnected = false) const noexcept;
	//<id> is the successor of this node
	void join(unsigned int id) noexcept;
	//<id> is current successor's predecessor, returns true on success, false on error
	bool stabilize(unsigned int id) noexcept;
	//<id> might be this node's predecessor
	bool notify(unsigned int id) noexcept;
	//Node <id> has either joined or left the network, returns true if internal records modified
	bool update(unsigned int id, bool joined) noexcept;
	//Returns true if <id> exists in the internal records (incl. 0 and node's key)
	bool isInRoute(unsigned int id) const noexcept;
	//For testing purposes
	void print() noexcept;
private:
	void initialize() noexcept;
	/*
	 * If <checkConsistent> then update will fail if the finger is not consistent
	 * If <checkConnected> then routing table will become unstable post update
	 * If <id>==0 then routing table's stability flag remains untouched
	 * Caller must make sure that <id> is valid and remains in range [0, MAX_ID]
	 */
	bool setFinger(Finger &f, unsigned int id, bool checkConsistent = true,
			bool checkConnected = true) noexcept;
	/*
	 * Clean up records associated with <id> in the routing table
	 * Returns true if a record was found and subsequently removed
	 */
	bool removeNode(unsigned int id) noexcept;
public:
	//Limit on key length in bits (this is an architectural limit)
	static constexpr unsigned int MAXKEYLENGTH = DHT::IDENTIFIER_LENGTH;
#ifndef WH_DHT_KEYLEN
#define WH_DHT_KEYLEN 10U
#endif
	//Current key length in bits, must not be greater than MAXKEYLENGTH
	static constexpr unsigned int KEYLENGTH = (
	WH_DHT_KEYLEN <= MAXKEYLENGTH ? WH_DHT_KEYLEN : MAXKEYLENGTH);
#undef WH_DHT_KEYLEN
	static constexpr unsigned long MAX_NODES = ((1UL << KEYLENGTH)); //Max number of nodes
	static constexpr unsigned int CONTROLLER = 0; //Special ID, must be 0
	static constexpr unsigned int MIN_ID = 1; //Controller is not a part of the DHT
	static constexpr unsigned int MAX_ID = ((1UL << KEYLENGTH) - 1); //2^KEYLENGTH - 1
	static constexpr unsigned int TABLESIZE = KEYLENGTH; //Size of the finger table
private:
	//This node's key
	const unsigned int _key;
	//Current predecessor
	Finger _predecessor;
	//table[0] is the immediate successor
	Finger table[TABLESIZE];
	//On update <stable> is set to false
	bool stable;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_NODE_H_ */
