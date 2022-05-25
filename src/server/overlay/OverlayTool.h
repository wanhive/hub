/*
 * OverlayTool.h
 *
 * Command line tool for testing of the overlay network
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_OVERLAYTOOL_H_
#define WH_SERVER_OVERLAY_OVERLAYTOOL_H_
#include "OverlayProtocol.h"
#include "../../hub/Identity.h"
#include "../../util/Authenticator.h"

namespace wanhive {
/**
 * Command line tool for testing of the overlay network protocols
 * Supports SSL/TLS connection
 */
class OverlayTool: private OverlayProtocol, private Identity {
public:
	OverlayTool(const char *path, unsigned int timeout = 5000) noexcept;
	~OverlayTool();
	void run() noexcept;
private:
	//=================================================================
	void setup();
	void execute() noexcept;
	void connect();
	void disconnect() noexcept;
	//=================================================================
	/**
	 * Authentication commands
	 */
	void identifyCmd();
	void authenticateCmd();
	void authorizeCmd();
	void describeCmd();
	/**
	 * Registration and bootstrap commands
	 */
	void registerCmd();
	void getKeyCmd();
	void findRoot();
	void findBootstrapNode();
	/**
	 * Pub-Sub commands
	 */
	void publishCmd();
	void subscribeCmd();
	void unsubscribeCmd();
	/**
	 * Node management commands
	 */
	void getPredecessorCmd();
	void setPredecessorCmd();
	void getSuccessorCmd();
	void setSuccessorCmd();
	void getFingerCmd();
	void setFingerCmd();
	void getNeighboursCmd();
	void notifyCmd();
	/**
	 * Overlay management commands
	 */
	void findSuccessorCmd();
	void pingCmd();
	void mapCmd();
private:
	unsigned int timeout;
	unsigned long long hostId;
	unsigned long long destinationId;
	Authenticator auth;
};

} /* namespace wanhive */

#endif /* WH_SERVER_OVERLAY_OVERLAYTOOL_H_ */
