/*
 * TestClient.h
 *
 * For building synchronous test clients
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_TEST_FLOOD_TESTCLIENT_H_
#define WH_TEST_FLOOD_TESTCLIENT_H_
#include "../../hub/Identity.h"
#include "../../hub/Protocol.h"

namespace wanhive {
/**
 * Connects with an existing overlay network via any participant node, including
 * the controller. Extend this class to build network testing tools. Asymmetric
 * key based authentication supported.
 */
class TestClient: protected Identity, protected Protocol {
public:
	TestClient(uint64_t id, const char *path) noexcept;
	virtual ~TestClient();
	uint64_t getHostId() const noexcept;
	void bootstrap(uint64_t id, int timeout = -1);
	virtual bool authenticate();
private:
	void connect(uint64_t id, int timeout);
private:
	uint64_t hostId;
};

} /* namespace wanhive */

#endif /* WH_TEST_FLOOD_TESTCLIENT_H_ */
