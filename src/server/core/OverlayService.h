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

#ifndef WH_SERVER_CORE_OVERLAYSERVICE_H_
#define WH_SERVER_CORE_OVERLAYSERVICE_H_
#include "Node.h"
#include "OverlayProtocol.h"
#include "../../base/TurnGate.h"

namespace wanhive {
/**
 * Overlay network stabilization service
 */
class OverlayService: private OverlayProtocol {
public:
	/**
	 * Constructor: sets up hub's identity.
	 * @param uid hub's identity
	 */
	OverlayService(unsigned long long uid) noexcept;
	/**
	 * Destructor
	 */
	~OverlayService();
	/**
	 * Reconfigures the object after cleaning up if required.
	 * @param connection blocking socket connection to the local hub
	 * @param nodes list of bootstrap node identities
	 * @param updateCycle stabilization cycle's period in milliseconds
	 * @param retryInterval time to wait in milliseconds after stabilization
	 * or network error.
	 */
	void configure(int connection, const unsigned long long *nodes,
			unsigned int updateCycle, unsigned int retryInterval) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Executes stabilization routines periodically until a notification or an
	 * exception. Cleans up before returning.
	 */
	void periodic() noexcept;
	/**
	 * Executes the stabilization routines.
	 * @return true on success, false on non-fatal or temporary error
	 */
	bool execute();
	/**
	 * Waits until a notification arrives or the timeout expires.
	 * @param timeout timeout value in milliseconds
	 * @return true on a notification, false on timeout.
	 */
	bool wait(unsigned int timeout);
	/**
	 * Delivers a notification to this object.
	 * @return true on success, false on error
	 */
	bool notify() noexcept;
	/**
	 * Frees the resources.
	 */
	void cleanup() noexcept;
private:
	//-----------------------------------------------------------------
	//Sets things up
	void setup();
	//Resets the internal structures to their default values
	void clear() noexcept;
	//Checks the network connection
	bool checkNetwork() noexcept;
	//Joins the network using a list of bootstrap nodes
	bool bootstrap() noexcept;
	//-----------------------------------------------------------------
	//Checks whether the remote node <id> is reachable or not
	bool isReachable(uint64_t id) noexcept;
	//Join as node identified by <id> using the <startNode>
	bool join(uint64_t id, uint64_t startNode) noexcept;
	//Check the predecessor of the node identified by <id>
	bool checkPredecessor(uint64_t id);
	//Stabilize the node identified by <id>
	bool stabilize(uint64_t id);
	//Fix the finger table for the node identified by <id>
	bool fixFingerTable(uint64_t id) noexcept;
	//-----------------------------------------------------------------
	//Periodically update the successors list for the node <id>
	bool fixSuccessorsList(uint64_t id) noexcept;
	//Successor of the node <id> has failed, repair it using backup mechanism
	bool repairSuccessor(uint64_t id);
	//Check the controller's connection through the node <id>
	bool checkController(uint64_t id);
	//-----------------------------------------------------------------
	void setConnection(int connection) noexcept;
	void setBootstrapNodes(const unsigned long long *nodes) noexcept;
	void setUpdateCycle(unsigned int updateCycle) noexcept;
	void setRetryInterval(unsigned int retryInterval) noexcept;
private:
	//Hub's identity
	const unsigned long long uid;
	//The next successor to fix
	unsigned int sIndex;
	//The next finger to fix
	unsigned int fIndex;
	//Connection with controller failed
	bool controllerFailed;
	//Initialization status
	bool initialized;
	//For thread synchronization
	TurnGate barrier;
	//-----------------------------------------------------------------
	/*
	 * List of backup successors excluding the immediate successor. The network
	 * should have (SUCCESSOR_LIST_LEN + 2) stable members.
	 */
	static constexpr unsigned int SUCCESSOR_LIST_LEN = (
			Node::KEYLENGTH > 1 ? Node::KEYLENGTH - 1 : 1);
	uint64_t successors[SUCCESSOR_LIST_LEN];
	//-----------------------------------------------------------------
	/*
	 * Configuration parameters: no shared states with the outside world except
	 * the socket connection.
	 */
	struct {
		//Bootstrap nodes
		unsigned long long nodes[16];
		//Socket connection to the hub
		int connection;
		//Wait period in milliseconds between routing table updates
		unsigned int updateCycle;
		//Wait period in milliseconds after stabilization error
		unsigned int retryInterval;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYSERVICE_H_ */
