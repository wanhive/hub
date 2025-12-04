/*
 * TestClient.cpp
 *
 * For building synchronous test clients
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "TestClient.h"
#include "../../base/common/Exception.h"
#include "../../base/common/Logger.h"
#include <cinttypes>

namespace wanhive {

TestClient::TestClient(uint64_t id, const char *path) noexcept :
		Identity(path) {
	setSource(id);
	hostId = id;
}

TestClient::~TestClient() {

}

uint64_t TestClient::getHostId() const noexcept {
	return hostId;
}

void TestClient::bootstrap(uint64_t id, int timeout) {
	try {
		Identity::reset();
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
				throw Exception(EX_OPERATION);
			}
		} else {
			throw Exception(EX_OPERATION);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

bool TestClient::authenticate() {
	try {
		Digest hc;
		useKeyPair(getPKI());
		return tokenRequest( { 0, hostId }, &hc, verifyHost())
				&& registerRequest( { getSource(), hostId }, &hc);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void TestClient::connect(uint64_t id, int timeout) {
	try {
		NameInfo ni;
		Identity::getAddress(id, ni);
		Endpoint::setSSLContext(Identity::getSSLContext());
		Endpoint::connect(ni, timeout);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

} /* namespace wanhive */
