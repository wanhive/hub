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
	void setup();
	void clear() noexcept;
	bool checkNetwork() noexcept;
	bool bootstrap() noexcept;
	//-----------------------------------------------------------------
	bool isReachable(uint64_t id) noexcept;
	bool join(uint64_t id, uint64_t start) noexcept;
	bool checkPredecessor(uint64_t id);
	bool stabilize(uint64_t id);
	bool fixFingerTable(uint64_t id) noexcept;
	//-----------------------------------------------------------------
	bool fixSuccessorsList(uint64_t id) noexcept;
	bool repairSuccessor(uint64_t id);
	bool checkController(uint64_t id);
	//-----------------------------------------------------------------
	void setConnection(int connection) noexcept;
	void setBootstrapNodes(const unsigned long long *nodes) noexcept;
	void setPeriod(unsigned int period) noexcept;
	void setDelay(unsigned int delay) noexcept;
private:
	const unsigned long long uid;
	unsigned int sIndex;
	unsigned int fIndex;
	bool controllerFailed;
	bool initialized;
	TurnGate barrier;
	//-----------------------------------------------------------------
	static constexpr unsigned int SUCCESSOR_LIST_LEN = (
			Node::KEYLENGTH > 1 ? Node::KEYLENGTH - 1 : 1);
	uint64_t successors[SUCCESSOR_LIST_LEN];
	//-----------------------------------------------------------------
	struct {
		unsigned long long nodes[16];
		int connection;
		unsigned int period;
		unsigned int delay;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYSERVICE_H_ */
