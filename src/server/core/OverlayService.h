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
	 * Constructor: assigns hub's identity.
	 * @param uid hub's identity
	 */
	OverlayService(unsigned long long uid) noexcept;
	/**
	 * Destructor
	 */
	~OverlayService();
	/**
	 * Reconfigures the object after cleaning up if required.
	 * @param connection blocking socket connection to the hub
	 * @param nodes bootstrap nodes' list
	 * @param period stabilization cycle's period in milliseconds
	 * @param delay time to wait in milliseconds after stabilization
	 * or network error.
	 */
	void configure(int connection, const unsigned long long *nodes,
			unsigned int period, unsigned int delay) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Executes stabilization routines periodically until a notification or an
	 * exception. Performs cleanup of internal resources prior to returning.
	 */
	void periodic() noexcept;
	/**
	 * Executes the stabilization routines.
	 * @return true on success, false on non-fatal or temporary error
	 */
	bool execute();
	/**
	 * Waits for a notification.
	 * @param timeout wait period in milliseconds
	 * @return true if a notification arrived, false if timeout expired
	 */
	bool wait(unsigned int timeout);
	/**
	 * Delivers a notification.
	 * @return true on success, false on error
	 */
	bool notify() noexcept;
	/**
	 * Cleans up the internal resources.
	 */
	void cleanup() noexcept;
private:
	//-----------------------------------------------------------------
	//Sets things up
	void setup();
	//Resets the internal structures
	void clear() noexcept;
	//Checks the network connection
	bool checkNetwork() noexcept;
	//Joins the network using a list of bootstrap nodes
	bool bootstrap() noexcept;
	//-----------------------------------------------------------------
	//Checks whether the remote node <id> is reachable
	bool isReachable(uint64_t id) noexcept;
	//Join as the node <id> using the <start> node
	bool join(uint64_t id, uint64_t start) noexcept;
	//Check the predecessor of the node <id>
	bool checkPredecessor(uint64_t id);
	//Stabilize the node <id>
	bool stabilize(uint64_t id);
	//Fix the finger table for the node <id>
	bool fixFingerTable(uint64_t id) noexcept;
	//-----------------------------------------------------------------
	//Update the successors list for the node <id>
	bool fixSuccessorsList(uint64_t id) noexcept;
	//Repair failed successor of the node <id>
	bool repairSuccessor(uint64_t id);
	//Check controller's status with assistance from the node <id>
	bool checkController(uint64_t id);
	//-----------------------------------------------------------------
	void setConnection(int connection) noexcept;
	void setBootstrapNodes(const unsigned long long *nodes) noexcept;
	void setPeriod(unsigned int period) noexcept;
	void setDelay(unsigned int delay) noexcept;
private:
	//Hub's identity
	const unsigned long long uid;
	//Next successor to fix
	unsigned int sIndex;
	//Next finger to fix
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
		//Maintenance period in milliseconds
		unsigned int period;
		//Wait period in milliseconds after error
		unsigned int delay;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYSERVICE_H_ */
