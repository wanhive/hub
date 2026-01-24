/*
 * OverlayTool.h
 *
 * Overlay network tests
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_CORE_OVERLAYTOOL_H_
#define WH_SERVER_CORE_OVERLAYTOOL_H_
#include "OverlayProtocol.h"
#include "../../hub/Identity.h"
#include "../../util/Verifier.h"

namespace wanhive {
/**
 * Overlay network tests
 */
class OverlayTool: private OverlayProtocol, private Identity {
public:
	/**
	 * Constructor: carries out basic initialization.
	 * @param path configuration file's path (can be nullptr)
	 * @param timeout socket I/O timeout in milliseconds
	 */
	OverlayTool(const char *path, unsigned int timeout) noexcept;
	/**
	 * Destructor
	 */
	~OverlayTool();
	/**
	 * Interactively runs the network tests.
	 */
	void run() noexcept;
private:
	//-----------------------------------------------------------------
	void setup();
	void execute() noexcept;
	void connect();
	void disconnect() noexcept;
	//-----------------------------------------------------------------
	/*
	 * Authentication commands
	 */
	void identifyCmd();
	void authenticateCmd();
	void authorizeCmd();
	void describeCmd();
	//-----------------------------------------------------------------
	/*
	 * Registration and bootstrap commands
	 */
	void registerCmd();
	void tokenCmd();
	void findRootCmd();
	void bootstrapCmd();
	//-----------------------------------------------------------------
	/*
	 * Pub-Sub commands
	 */
	void publishCmd();
	void subscribeCmd();
	void unsubscribeCmd();
	//-----------------------------------------------------------------
	/*
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
	//-----------------------------------------------------------------
	/*
	 * Network management commands
	 */
	void findSuccessorCmd();
	void pingCmd();
	void mapCmd();
private:
	unsigned int timeout;
	unsigned long long hostId;
	unsigned long long destinationId;
	Verifier auth;
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYTOOL_H_ */
