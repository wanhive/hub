/*
 * OverlayService.h
 *
 * Overlay network maintenance service
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_OVERLAYSERVICE_H_
#define WH_SERVER_OVERLAY_OVERLAYSERVICE_H_
#include "Node.h"
#include "OverlayProtocol.h"
#include "../../base/Condition.h"

namespace wanhive {
/**
 * DHT stabilization service
 * Thread safe at class level
 */
class OverlayService: private OverlayProtocol {
public:
	OverlayService(unsigned long long uid) noexcept;
	virtual ~OverlayService();
	//-----------------------------------------------------------------
	/**
	 * Performs cleanup and then loads new settings
	 */
	void configure(int connection, const unsigned long long *nodes,
			unsigned int updateCycle, unsigned int retryInterval) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Executes the stabilization routines periodically in a loop until
	 * an exception occurs or a notification is delivered. Calls the
	 * cleanup routine on exit.
	 */
	void periodic() noexcept;
	/*
	 * Execute this periodically to maintain network stability.
	 * Returns true on success, false on non-fatal error.
	 */
	bool execute();
	/*
	 * Waits for timeout or notification.
	 * Returns false on timeout, true on receipt of a notification.
	 */
	bool wait();
	/*
	 * Delivers a notification to this object. Returns true on success,
	 * false otherwise.
	 */
	bool notify() noexcept;
	/*
	 * Cleans up the resources and closes the existing connection.
	 */
	void cleanup() noexcept;
private:
	//-----------------------------------------------------------------
	/**
	 * Stabilization routines
	 */
	//Checks whether the remote node <id> is reachable or not
	bool isReachable(uint64_t id) noexcept;
	//Join as node identified by <id> using <startNode>
	bool join(uint64_t id, uint64_t startNode) noexcept;
	//Check the predecessor of the node identified by <id>
	bool checkPredecessor(uint64_t id);
	//Stabilize the node identified by <id>
	bool stabilize(uint64_t id);
	//Fix the finger table for the node identified by <id>
	bool fixFingerTable(uint64_t id) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following two functions are called by <stabilize>
	 */
	//Periodically update the successor list for the node <id>
	bool fixSuccessorsList(uint64_t id) noexcept;
	//Successor of the node <id> has failed, repair it using backup mechanism
	bool repairSuccessor(uint64_t id);
	//Check the connection with the controller
	bool checkController(uint64_t id);
	//-----------------------------------------------------------------
	//Sets things up
	void setup();
	//Cleans up the internal structures
	void clear() noexcept;
	//-----------------------------------------------------------------
	void setConnection(int connection) noexcept;
	void setBootstrapNodes(const unsigned long long *nodes) noexcept;
	void setUpdateCycle(unsigned int updateCycle) noexcept;
	void setRetryInterval(unsigned int retryInterval) noexcept;
private:
	//Identifier of the hub
	const unsigned long long uid;
	//Next successor to fix
	unsigned int sIndex;
	//Next finger to fix
	unsigned int fIndex;
	//Wait period
	unsigned int delay;
	//Set to true if connection with controller failed
	bool controllerFailed;
	//Initialization status
	bool initialized;
	//The condition variable for thread synchronization
	Condition condition;
	//-----------------------------------------------------------------
	/**
	 * The backup successors
	 * DHT should have (SUCCESSOR_LIST_LEN + 2) stable members
	 */
	static constexpr unsigned int SUCCESSOR_LIST_LEN = (
			Node::KEYLENGTH > 1 ? Node::KEYLENGTH - 1 : 1);
	//The backup successors list, excluding the immediate successor
	uint64_t successors[SUCCESSOR_LIST_LEN];
	//-----------------------------------------------------------------
	/**
	 * Configuration parameters: No shared states with the outside world
	 * except the socket connection.
	 */
	struct {
		//Bootstrap nodes
		unsigned long long nodes[16];
		//The socket connection descriptor
		int connection;
		//Wait period in milliseconds between routing table updates
		unsigned int updateCycle;
		//Wait period in milliseconds after stabilization error
		unsigned int retryInterval;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_OVERLAYSERVICE_H_ */
