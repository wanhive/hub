/*
 * Agent.cpp
 *
 * For building synchronous test clients
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Agent.h"
#include "../../base/Logger.h"
#include "../../base/common/Exception.h"
#include <cinttypes>

namespace wanhive {

Agent::Agent(uint64_t id, const char *path) noexcept :
		Identity(path) {
	setSource(id);
	hostId = id;
}

Agent::~Agent() {

}

uint64_t Agent::getHostId() const noexcept {
	return hostId;
}

void Agent::bootstrap(uint64_t id, int timeout) {
	try {
		Identity::initialize();
		Protocol::setSSLContext(Identity::getSSLContext());
		//-----------------------------------------------------------------
		//Connect with the bootstrap node
		WH_LOG_INFO("Connecting to the bootstrap node: % " PRIu64, id);
		connect(id, timeout);
		hostId = id;
		//-----------------------------------------------------------------
		//Find the Master Node and connect with it
		WH_LOG_INFO("Searching for the master node");
		if (findRootRequest(hostId, getSource(), id)) {
			WH_LOG_INFO("Master node found: %" PRIu64, id);
			//Bootstrap node different from the master node, connect with the master node
			if (hostId != id) {
				hostId = id;
				connect(id, timeout);
			}

			WH_LOG_INFO("Authenticating...");
			//Authenticate at the master node
			if (authenticate()) {
				WH_LOG_INFO("Authentication successful");
			} else {
				throw Exception(EX_INVALIDOPERATION);
			}
		} else {
			throw Exception(EX_INVALIDOPERATION);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

bool Agent::authenticate() {
	try {
		Digest hc;
		useKeyPair(getPKI());
		return getKeyRequest(hostId, &hc, verifyHost())
				&& registerRequest(hostId, getSource(), &hc);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Agent::connect(uint64_t id, int timeout) {
	try {
		NameInfo ni;
		Identity::getAddress(id, ni);
		Protocol::connect(ni, timeout);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

} /* namespace wanhive */
