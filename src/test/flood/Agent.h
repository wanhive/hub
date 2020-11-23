/*
 * Agent.h
 *
 * For building synchronous test clients
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_TEST_FLOOD_AGENT_H_
#define WH_TEST_FLOOD_AGENT_H_
#include "../../hub/Protocol.h"
#include "../../util/Identity.h"

namespace wanhive {
/**
 * Agent can connect with an existing overlay network via
 * any participant node including the controller.
 * Extend this class to build network test tools.
 * Only asymmetric key based authentication supported.
 */
class Agent: protected Identity, protected Protocol {
public:
	Agent(uint64_t id, const char *path) noexcept;
	virtual ~Agent();
	//Identifier of the current host
	uint64_t getHostId() const noexcept;
	/*
	 * Connect with the network via host <id>; <id> can be 0 (Controller).
	 * On success the socket is created in blocking mode and the <timeout>
	 * value is preserved.
	 */
	void bootstrap(uint64_t id, int timeout = -1);
	/*
	 * Authenticate at the currently connected host.
	 * Default implementation uses public key for authentication.
	 * Must return true on successful authentication.
	 */
	virtual bool authenticate();
private:
	//Connect with the host <id>
	void connect(uint64_t id, int timeout);
private:
	uint64_t hostId;
};

} /* namespace wanhive */

#endif /* WH_TEST_FLOOD_AGENT_H_ */
