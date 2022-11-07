/*
 * OverlayService.cpp
 *
 * Overlay network maintenance service
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "OverlayService.h"
#include "../../base/common/Exception.h"
#include "../../base/common/Logger.h"
#include <cstring>

namespace wanhive {

OverlayService::OverlayService(unsigned long long uid) noexcept :
		uid(uid) {
	clear();
}

OverlayService::~OverlayService() {
	cleanup();
}

void OverlayService::configure(int connection, const unsigned long long *nodes,
		unsigned int updateCycle, unsigned int retryInterval) noexcept {
	cleanup();
	setConnection(connection);
	setBootstrapNodes(nodes);
	setUpdateCycle(updateCycle);
	setRetryInterval(retryInterval);
}

void OverlayService::periodic() noexcept {
	try {
		while (true) {
			auto delay = execute() ? ctx.updateCycle : ctx.retryInterval;
			if (wait(delay)) {
				break;
			}
		}
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
	}

	cleanup(); //Prevent resource leak
}

bool OverlayService::execute() {
	try {
		if (!initialized) {
			setup();
		}
		//-----------------------------------------------------------------
		//STEP 1: Check whether the predecessor has failed
		if (!checkPredecessor(uid)) {
			WH_LOG_ERROR("Predecessor check failed");
			return false;
		}
		//STEP 2: Allow the network to recover from controller failure
		if (controllerFailed) {
			controllerFailed = false;
			return false;
		}
		//STEP 3: Check if the successor is alive and perform consistency check
		if (!stabilize(uid)) {
			WH_LOG_ERROR("Stabilization failed");
			return false;
		}
		//STEP 4: Now Fix the finger table (round-robin)
		if (!fixFingerTable(uid)) {
			WH_LOG_ERROR("Finger table repair failed");
			return false;
		}

		//STEP 5: success
		return true;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

bool OverlayService::wait(unsigned int timeout) {
	try {
		return condition.timedWait(timeout);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

bool OverlayService::notify() noexcept {
	try {
		condition.notify();
		return true;
	} catch (const BaseException &e) {
		return false;
	}
}

void OverlayService::cleanup() noexcept {
	releaseSocket();
	Network::close(ctx.connection);
	clear();
}

void OverlayService::setup() {
	try {
		if (!uid || initialized) {
			throw Exception(EX_INVALIDOPERATION);
		}

		//Set the flag for book-keeping
		initialized = true;
		//This socket will be automatically closed on exit
		setSocket(ctx.connection);
		//All the messages will carry this source ID
		setSource(uid);
		//-----------------------------------------------------------------
		//Force the hub to connect to the controller
		WH_LOG_INFO("Joining the overlay network, this will take a while");
		if (checkNetwork()) {
			WH_LOG_INFO("Connection to the controller established");
		} else {
			WH_LOG_ERROR("Controller is unreachable");
			throw Exception(EX_INVALIDSTATE);
		}
		//-----------------------------------------------------------------
		//Bootstrap using a predefined list of external nodes
		if (bootstrap()) {
			WH_LOG_INFO("Bootstrap succeeded");
		} else {
			WH_LOG_ERROR("Bootstrap failed");
			throw Exception(EX_INVALIDSTATE);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void OverlayService::clear() noexcept {
	sIndex = 0;
	fIndex = 0;
	controllerFailed = false;
	initialized = false;
	memset(successors, 0, sizeof(successors));
	memset(&ctx, 0, sizeof(ctx));
	ctx.connection = -1;
}

bool OverlayService::checkNetwork() noexcept {
	for (unsigned int i = 0; i < 4; ++i) {
		if (!isReachable(uid)) {
			return false;
		} else if (!isReachable(0)) {
			continue;
		} else {
			return true;
		}
	}

	return false;
}

bool OverlayService::bootstrap() noexcept {
	auto nodes = ctx.nodes;
	auto joinSelf = false;
	for (unsigned int i = 0; nodes[i]; ++i) {
		WH_LOG_DEBUG("Contacting %llu ...", nodes[i]);
		if (nodes[i] == uid) {
			joinSelf = true;
			continue;
		} else if (join(uid, nodes[i])) {
			WH_LOG_DEBUG("Join succeeded for %llu using %llu", uid, nodes[i]);
			return true;
		} else {
			continue;
		}
	}

	if (joinSelf && join(uid, uid)) {
		WH_LOG_DEBUG("Join succeeded for %llu using %llu", uid, uid);
		return true;
	} else {
		return false;
	}
}

bool OverlayService::isReachable(uint64_t id) noexcept {
	try {
		return pingRequest(id);
	} catch (...) {
		return false;
	}
}

bool OverlayService::join(uint64_t id, uint64_t startNode) noexcept {
	try {
		uint64_t successor = 0;
		if (!findSuccessorRequest(startNode, id, successor)) {
			return false;
		}

		if (!setPredecessorRequest(id, 0)
				|| !setSuccessorRequest(id, successor)) {
			return false;
		}

		/*
		 * Aggressive join for fast convergence. Set the correct predecessor
		 * immediately and notify the immediate successor.
		 */
		uint64_t sPredecessor = 0;
		uint64_t sSuccessor = 0;
		if (getNeighboursRequest(successor, sPredecessor, sSuccessor)) {
			successors[0] = sSuccessor;	//The first backup successor
			return setPredecessorRequest(id, sPredecessor)
					&& notifyRequest(successor, id);
		} else {
			return false;
		}
	} catch (const BaseException &e) {
		return false;
	}
}

bool OverlayService::checkPredecessor(uint64_t id) {
	try {
		uint64_t predecessor = 0;
		if (!getPredecessorRequest(id, predecessor)) {
			return false;
		} else if (!predecessor) {
			//HACK: checking the controller
			return checkController(id);
		} else if (isReachable(predecessor)) {
			//Predecessor is alive
			return true;
		} else if (checkController(id)) {
			//Predecessor has failed
			return setPredecessorRequest(id, 0);
		} else {
			//Controller failure
			return false;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

bool OverlayService::stabilize(uint64_t id) {
	try {
		uint64_t successor;
		if (!getSuccessorRequest(id, successor)) {
			return false;
		}
		//-----------------------------------------------------------------
		uint64_t sPredecessor = 0; //predecessor of the current successor
		uint64_t sSuccessor = 0; //successor of the current successor
		//Get the neighbors of the current successor
		if (!getNeighboursRequest(successor, sPredecessor, sSuccessor)) {
			return false;
		}
		//-----------------------------------------------------------------
		//Stabilize the local node
		if (sPredecessor != 0 && Node::isBetween(sPredecessor, id, successor)) {
			if (setSuccessorRequest(id, sPredecessor)) {
				//Successor changed
				successor = sPredecessor;
				successors[0] = successor;
			} else {
				return false;
			}
		} else {
			successors[0] = sSuccessor;
		}
		//-----------------------------------------------------------------
		//Tell the current successor that this node is it's predecessor
		if (!notifyRequest(successor, id)) {
			return false;
		}
		//-----------------------------------------------------------------
		//Update one of the backup successors (round-robin)
		fixSuccessorsList(id);
		return true;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		//Stabilization failed, try to recover
		return repairSuccessor(id);
	}
}

bool OverlayService::fixFingerTable(uint64_t id) noexcept {
	try {
		fIndex = (fIndex + 1) % Node::TABLESIZE;
		uint64_t start = Node::successor(id, fIndex);
		uint64_t target = id; //Use this node to resolve the finger
		uint64_t key = 0;

		if (fIndex == 0 && !getSuccessorRequest(id, target)) {
			return false;
		} else if (!findSuccessorRequest(target, start, key)) {
			return false;
		} else if (!setFingerRequest(id, fIndex, key)) {
			return false;
		} else {
			return true;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return false;
	}
}

bool OverlayService::fixSuccessorsList(uint64_t id) noexcept {
	try {
		if (SUCCESSOR_LIST_LEN > 1) {
			sIndex += 1;
			sIndex = (sIndex >= SUCCESSOR_LIST_LEN ? 1 : sIndex);
			//Store successor of _successors[sIndex - 1] in _successors[sIndex]
			return getSuccessorRequest(successors[sIndex - 1],
					successors[sIndex]);
		} else {
			//First backup successor is fixed during stabilization
			return true;
		}
	} catch (const BaseException &e) {
		sIndex = 0;
		return false;
	}
}

bool OverlayService::repairSuccessor(uint64_t id) {
	try {
		if (!checkController(id)) {
			//controller failure
			return false;
		}

		//Fix using the successors list
		for (unsigned int i = 0; i < SUCCESSOR_LIST_LEN; i++) {
			if (!successors[i]) {
				continue;
			} else if (successors[i] == id || isReachable(successors[i])) {
				return setSuccessorRequest(id, successors[i]);
			} else if (!checkController(id)) {
				//controller failed mid-way
				return false;
			} else {
				continue;
			}
		}
		//Could not recover, bail out
		throw Exception(EX_INVALIDSTATE);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

bool OverlayService::checkController(uint64_t id) {
	if (isReachable(0)) {
		return true;
	} else {
		controllerFailed = true;
		pingRequest(id);
		return false;
	}
}

void OverlayService::setConnection(int connection) noexcept {
	ctx.connection = connection;
}

void OverlayService::setBootstrapNodes(const unsigned long long *nodes) noexcept {
	unsigned int i = 0;
	for (; nodes && nodes[i] && (i < (ArraySize(ctx.nodes) - 1)); ++i) {
		ctx.nodes[i] = nodes[i];
	}

	ctx.nodes[i] = 0;
}

void OverlayService::setUpdateCycle(unsigned int updateCycle) noexcept {
	ctx.updateCycle = updateCycle;
}

void OverlayService::setRetryInterval(unsigned int retryInterval) noexcept {
	ctx.retryInterval = retryInterval;
}

} /* namespace wanhive */
