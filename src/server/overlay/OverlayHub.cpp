/*
 * OverlayHub.cpp
 *
 * Overlay hub (server) implementation
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "OverlayHub.h"
#include "commands.h"
#include "../../base/common/Logger.h"
#include <cinttypes>

namespace wanhive {

OverlayHub::OverlayHub(unsigned long long uid, const char *path) :
		Hub(uid, path), Node(uid), stabilizer(uid) {
	clear();
}

OverlayHub::~OverlayHub() {

}

void OverlayHub::stop(Watcher *w) noexcept {
	onRecycle(w);
	Hub::stop(w);
}

void OverlayHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		decltype(auto) conf = Identity::getConfiguration();
		ctx.enableRegistration = conf.getBoolean("OVERLAY",
				"enableRegistration");
		ctx.authenticateClient = conf.getBoolean("OVERLAY",
				"authenticateClient");
		ctx.connectToOverlay = conf.getBoolean("OVERLAY", "connectToOverlay");
		ctx.updateCycle = conf.getNumber("OVERLAY", "updateCycle", 5000);
		ctx.requestTimeout = conf.getNumber("OVERLAY", "timeOut", 5000);
		ctx.retryInterval = conf.getNumber("OVERLAY", "retryInterval", 10000);
		auto netmaskStr = conf.getString("OVERLAY", "netMask", "0x0");
		sscanf(netmaskStr, "%llx", &ctx.netMask);
		ctx.groupId = conf.getNumber("OVERLAY", "groupId");

		auto n = Identity::getIdentifiers("BOOTSTRAP", "nodes",
				ctx.bootstrapNodes,
				WH_ARRAYLEN(ctx.bootstrapNodes) - 1);
		if (!n) {
			n = Identity::getIdentifiers(ctx.bootstrapNodes,
			WH_ARRAYLEN(ctx.bootstrapNodes) - 1, Hosts::BOOTSTRAP);
		}
		ctx.bootstrapNodes[n] = 0;

		WH_LOG_DEBUG(
				"Overlay hub settings: \n" "ENABLE_REGISTRATION=%s, AUTHENTICATE_CLIENTS=%s, CONNECT_TO_OVERLAY=%s,\n" "TABLE_UPDATE_CYCLE=%ums, BLOCKING_IO_TIMEOUT=%ums, RETRY_INTERVAL=%ums,\n" "NETMASK=%s, GROUP_ID=%u\n",
				WH_BOOLF(ctx.enableRegistration),
				WH_BOOLF(ctx.authenticateClient),
				WH_BOOLF(ctx.connectToOverlay), ctx.updateCycle,
				ctx.requestTimeout, ctx.retryInterval, netmaskStr, ctx.groupId);
		installService();
		installSettingsMonitor();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void OverlayHub::cleanup() noexcept {
	Watcher *w = nullptr;
	if (!isHostId(getWorkerId()) && (w = getWatcher(getWorkerId()))) {
		//Shut down hub's end of the socket pair
		w->stop();
		stabilizer.notify();
	}

	clear();
	//Clean up the base class
	Hub::cleanup();
}

bool OverlayHub::trapMessage(Message *message) noexcept {
	return (bool) processRegistrationRequest(message);
}

void OverlayHub::route(Message *message) noexcept {
	//-----------------------------------------------------------------
	/*
	 * [REGISTRATION]: Intercept and handle registration and session-key requests
	 * because modification in the message will result in verification failure.
	 */
	if (interceptMessage(message)) {
		message->setGroup(0); //Ignore the group ID
		return;
	}
	//-----------------------------------------------------------------
	/*
	 * [FLOW CONTROL]: apply correct source, group, and label
	 */
	applyFlowControl(message);
	//-----------------------------------------------------------------
	/*
	 * [ROUTING]: Hub's ID is the sink
	 */
	createRoute(message);
	//-----------------------------------------------------------------
	/*
	 * [PROCESSING]: Process the local requests
	 */
	if (isHostId(message->getDestination())
			&& !message->testFlags(MSG_INVALID)) {
		//Maintain this order
		process(message); //Process the local request
		message->setGroup(0); //Ignore the group ID
	}
	//-----------------------------------------------------------------
	/*
	 * [DELIVERY]: deliver to the local destination
	 */
	if (isExternalNode(message->getDestination())) {
		message->writeLabel(0); //Clean up the label
	}
}

void OverlayHub::maintain() noexcept {
	if (!isStable()) {
		setStable(true);
		if (fixController()) {
			fixRoutingTable();
		}
	}
}

void OverlayHub::processInotification(unsigned long long uid,
		const InotifyEvent *event) noexcept {
	if (event->wd == -1) { //overflow notification
		return;
	}

	for (unsigned int i = 0; i < WATCHLIST_SIZE; ++i) {
		if (watchlist[i].identifier == event->wd) {
			watchlist[i].events |= event->mask;
			updateSettings(i);
			break;
		}
	}
}

bool OverlayHub::enableWorker() const noexcept {
	return isSupernode();
}

void OverlayHub::doWork(void *arg) noexcept {
	//Execute the stabilization loop
	stabilizer.periodic();
}

void OverlayHub::stopWork() noexcept {
	/*
	 * Just in case the worker thread failed to start and hence did
	 * not perform a cleanup on exit. No race condition because the
	 * worker thread has already exited.
	 */
	stabilizer.cleanup();
}

void OverlayHub::installService() {
	if (enableWorker()) {
		int fd = -1;
		Watcher *w = connect(fd, true, ctx.requestTimeout);
		onRegistration(w);

		worker.id = w->getUid();
		stabilizer.configure(fd, ctx.bootstrapNodes, ctx.updateCycle,
				ctx.retryInterval);
	} else {
		//Worker thread not required
	}
}

void OverlayHub::installSettingsMonitor() {
	try {
		//Events we are interested in: modify-> close
		uint32_t events = IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE;
		if (getConfigurationFile()) {
			watchlist[0].identifier = addToInotifier(getConfigurationFile(),
					events);
		}

		if (getHostsDatabase()) {
			watchlist[1].identifier = addToInotifier(getHostsDatabase(),
					events);
		}

		if (getHostsFile()) {
			watchlist[2].identifier = addToInotifier(getHostsFile(), events);
		}

		if (getPrivateKeyFile()) {
			watchlist[3].identifier = addToInotifier(getPrivateKeyFile(),
					events);
		}

		if (getPublicKeyFile()) {
			watchlist[4].identifier = addToInotifier(getPublicKeyFile(),
					events);
		}

		if (getSSLTrustedCertificateFile()) {
			watchlist[5].identifier = addToInotifier(
					getSSLTrustedCertificateFile(), events);
		}

		if (getSSLCertificateFile()) {
			watchlist[6].identifier = addToInotifier(getSSLCertificateFile(),
					events);
		}

		if (getSSLHostKeyFile()) {
			watchlist[7].identifier = addToInotifier(getSSLHostKeyFile(),
					events);
		}

	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void OverlayHub::updateSettings(unsigned int index) noexcept {
	//REF: https://github.com/guard/guard/wiki/Analysis-of-inotify-events-for-different-editors
	if (watchlist[index].events & IN_IGNORED) {
		//Associated file will no longer be monitored
		watchlist[index].identifier = -1;
		watchlist[index].events = 0;
	} else if (!(watchlist[index].events & IN_CLOSE_WRITE)) { //Write-close
		return;
	} else if (!(watchlist[index].events & (IN_MODIFY | IN_ATTRIB))) { //Modification
		//Closed without modification
		watchlist[index].events = 0;
		return;
	} else {
		//Reset for next cycle
		watchlist[index].events = 0;
	}
	//-----------------------------------------------------------------
	/*
	 * Reload the settings
	 */
	try {
		switch (index) {
		case 0:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG(
						"Configuration file has been modified (restart required)");
			} else {
				WH_LOG_DEBUG("Configuration file has been ignored");
			}
			break;
		case 1:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG("Hosts database has been modified");
			} else {
				WH_LOG_DEBUG("Hosts database has been ignored");
			}
			break;
		case 2:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG("Hosts file has been modified");
				Identity::loadHostsFile();
			} else {
				WH_LOG_DEBUG("Hosts file has been ignored");
			}
			break;
		case 3:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG("Private key file has been modified");
				Identity::loadPrivateKey();
			} else {
				WH_LOG_DEBUG("Private key file has been ignored");
			}
			break;
		case 4:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG("Public key file has been modified");
				Identity::loadPublicKey();
			} else {
				WH_LOG_DEBUG("Public key file has been ignored");
			}
			break;
		case 5:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG(
						"SSL trusted certificate has been modified (restart required)");
			} else {
				WH_LOG_DEBUG("SSL trusted certificate has been ignored");
			}
			break;
		case 6:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG("SSL certificate has been modified");
				loadSSLCertificate();
			} else {
				WH_LOG_DEBUG("SSL certificate has been ignored");
			}
			break;
		case 7:
			if (watchlist[index].identifier != -1) {
				WH_LOG_DEBUG("SSL host key has been modified");
				loadSSLHostKey();
			} else {
				WH_LOG_DEBUG("SSL host key has been ignored");
			}
			break;
		default:
			WH_LOG_DEBUG("Martian attack!");
			break;
		}
	} catch (const BaseException &e) {
		//Hub has been damaged, report problem and exit
		WH_LOG_EXCEPTION(e);
		exit(EXIT_FAILURE);
	}
}

bool OverlayHub::fixController() noexcept {
	//Establish a connection with the controller
	return connectToRoute(CONTROLLER, &sessions[TABLESIZE]);
}

bool OverlayHub::fixRoutingTable() noexcept {
	//Fall through the routing table and fix the errors
	for (unsigned int i = 0; i < TABLESIZE; i++) {
		if (!isConsistent(i)) {
			auto old = makeConsistent(i);
			if (!isInRoute(old)) {
				auto conn = getWatcher(old);
				//Take care of the reference asymmetry
				if (conn && conn->isType(SOCKET_PROXY)) {
					disable(conn);
				}
			}
		}

		//Update the connection status
		setConnected(i, connectToRoute(get(i), &sessions[i]));
	}

	//If the predecessor has changed, certain connections need to be removed
	if (predessorChanged()) {
		makePredecessorConsistent();
		purgeConnections(1);
	}

	return true;
}

bool OverlayHub::connectToRoute(unsigned long long id, Digest *hc) noexcept {
	try {
		return connect(id, hc);
	} catch (const BaseException &e) {
		if (!Socket::unallocated() || !Message::unallocated()) {
			purgeConnections(3, 2);
			setStable(false);
		}
		return false;
	}
}

void OverlayHub::onRegistration(Watcher *w) noexcept {
	auto id = w->getUid();
	if (!isSupernode()) {
		return;
	} else if (isController(id) || isWorkerId(id)) {
		w->setFlags(SOCKET_PRIORITY);
		((Socket*) w)->setOutputQueueLimit(0);
	} else if (isInternalNode(id)) {
		w->setFlags(SOCKET_OVERLAY);
		((Socket*) w)->setOutputQueueLimit(0);
		Node::update(id, true);
	} else {
		return;
	}
}

void OverlayHub::onRecycle(Watcher *w) noexcept {
	//If the worker connection failed then initiate shutdown
	if (isWorkerId(w->getUid())) {
		Hub::cancel();
	}

	//Remove from the routing table
	if (isInternalNode(w->getUid())) {
		Node::update(w->getUid(), false);
	}

	//Remove from the topics
	if (w->testFlags(WATCHER_MULTICAST)) {
		for (unsigned int i = 0; i < Topic::COUNT; i++) {
			if (w->testTopic(i)) {
				topics.remove(i, w);
			}
		}
	}
}

void OverlayHub::addToCache(unsigned long long id) noexcept {
	if (id && isInternalNode(id) && !isHostId(id)) {
		nodes.cache[nodes.index] = id;
		nodes.index = (nodes.index + 1) & (NODECACHE_SIZE - 1);
	}
}

bool OverlayHub::isValidStabilizationResponse(const Message *msg) const noexcept {
	auto &sh = worker.header;
	return msg->getStatus() != WH_DHT_AQLF_REQUEST
			&& msg->getLabel() == sh.getLabel()
			&& isHostId(msg->getDestination())
			&& msg->getSequenceNumber() == sh.getSequenceNumber()
			&& msg->getSession() == sh.getSession()
			&& msg->getCommand() == sh.getCommand()
			&& msg->getQualifier() == sh.getQualifier();
}

bool OverlayHub::isValidRegistrationRequest(const Message *msg) noexcept {
	/*
	 * 1. Confirm that the requested ID is valid
	 * 2. Analyze the security features (to prevent attacks)
	 */
	auto origin = msg->getOrigin();
	auto requestedId = msg->getSource();

	if (!allowRegistration(origin, requestedId)) {
		//CASE 1
		return false;
	} else if (!ctx.authenticateClient && !isInternalNode(requestedId)) {
		//CASE 2
		return true;
	} else if (!getPKI()) {
		//CASE 2
		return true;
	} else if (msg->getPayloadLength() == Hash::SIZE + PKI::SIGNATURE_LENGTH) {
		//CASE 2
		return verifyNonce(hash, origin, getUid(), (Digest*) msg->getBytes(0))
				&& msg->verify(getPKI());
	} else {
		return false;
	}
}

int OverlayHub::processRegistrationRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto oldUid = message->getSource();
	auto newUid = message->getDestination();
	auto aqlf = message->getStatus();
	//-----------------------------------------------------------------
	/*
	 * VALIDATE THE REGISTRATION REQUEST
	 * Origin must match the oldUid in all cases (only direct requests).
	 */
	if (origin != oldUid) {
		//If we are here then there is a bug in the request handler's logic
		return -1;
	}
	//-----------------------------------------------------------------
	/*
	 * REGISTRATION PROCEEDS
	 * Get the connection which is to be Activated/Deactivated
	 */
	int mode = -1;
	if (ctx.enableRegistration && aqlf == WH_DHT_AQLF_ACCEPTED) {
		mode = getModeOfRegistration(oldUid, newUid);
	}

	if (mode == -1) {
		removeWatcher(oldUid);
		return -1;
	}

	auto conn = registerWatcher(oldUid, newUid, (mode == 2) ? true : false);
	if (!conn) {
		return -1;
	} else {
		conn->setGroup(message->getSession());
		onRegistration(conn);
		return (mode == 0) ? 1 : 0;
	}
}

bool OverlayHub::allowRegistration(unsigned long long source,
		unsigned long long requestedId) const noexcept {
	/*
	 * 1. Registration should be enabled
	 * 2. Only fresh request and the Requested ID must be an Active ID
	 * 3. Requested ID cannot be one of the Host/Controller/Worker IDs
	 * 4. Requested Client ID must be "local"
	 */
	if (!ctx.enableRegistration) {
		//CASE 1
		return false;
	} else if (!Socket::isEphemeralId(source)
			|| Socket::isEphemeralId(requestedId)) {
		//CASE 2
		return false;
	} else if (isHostId(requestedId) || isController(requestedId)
			|| isWorkerId(requestedId)) {
		//CASE 3
		return false;
	} else if (isExternalNode(requestedId) && !isLocal(mapKey(requestedId))) {
		//CASE 4
		return false;
	} else {
		return true;
	}
}

int OverlayHub::getModeOfRegistration(unsigned long long oldUid,
		unsigned long long newUid) noexcept {
	if (isHostId(newUid) || isWorkerId(newUid)) {
		return -1;
	} else if (oldUid == newUid) {
		//Just activate
		return 0;
	} else if (isInternalNode(newUid)) {
		//Precedence Rule in case both sides are trying to connect (race condition)
		return ((newUid < getUid()) ? 1 : 2);
	} else {
		/*
		 * Default case: Reserve a few connections in the pool for internal usage.
		 * Replace the existing one on conflict.
		 */
		return isLocal(mapKey(newUid))
				&& !(isSupernode() && (Socket::unallocated() <= TABLESIZE)) ?
				2 : -1;
	}
}

bool OverlayHub::interceptMessage(Message *message) noexcept {
	if (message->getCommand() != WH_DHT_CMD_BASIC) {
		return false;
	} else if (message->getQualifier() == WH_DHT_QLF_REGISTER) {
		handleRegistrationRequest(message);
		return true;
	} else if (message->getQualifier() == WH_DHT_QLF_GETKEY) {
		handleGetKeyRequest(message);
		return true;
	} else {
		return false;
	}
}

void OverlayHub::applyFlowControl(Message *message) noexcept {
	if (isWorkerId(message->getOrigin())) {
		message->putLabel(getWorkerId() + getUid());
		message->getHeader(worker.header);
	} else if (isExternalNode(message->getOrigin())) {
		//Preserve the group ID at insertion
		message->writeLabel(message->getGroup());
		//Assign the correct source ID
		message->putSource(message->getOrigin());
	} else if (isExternalNode(message->getSource())) {
		//Retrieve the group ID during routing
		message->setGroup(message->getLabel());
	} else {
		addToCache(message->getSource());
	}
}

bool OverlayHub::createRoute(Message *message) noexcept {
	//Message's origin is immutable
	auto origin = message->getOrigin();
	auto destination = message->getDestination();
	//-----------------------------------------------------------------
	/*
	 * CREATE THE ROUTE
	 */
	if (isWorkerId(origin)) {
		if (!isHostId(destination)) {
			//Stabilization request sent via controller
			message->setDestination(CONTROLLER);
		}
	} else if (isController(origin)) {
		if (isValidStabilizationResponse(message)) {
			//Stabilization response returned via controller
			message->setDestination(getWorkerId());
		}
	} else if (allowCommunication(origin, destination)) {
		message->setDestination(getNextHop(destination));
	} else {
		//Highly likely a miscommunication
		if (!(isHostId(destination) || isController(destination))) {
			message->setFlags(MSG_INVALID);
		}
		message->setDestination(getUid());
	}

	return true;
}

unsigned long long OverlayHub::getNextHop(
		unsigned long long destination) const noexcept {
	/*
	 * CASE 1: destination is "local" or <destination> = <Controller>
	 * In such case do nothing and allow the server take care of it
	 * This is always the case with a stand-alone server
	 *
	 * CASE 2: Destination lies somewhere else on the network
	 * FIND THE NEXT HOP
	 */
	auto k = mapKey(destination);
	if (!isLocal(k) && !isController(destination)) {
		//Case 2
		return nextHop(k);
	} else {
		return destination;
	}
}

bool OverlayHub::allowCommunication(unsigned long long source,
		unsigned long long destination) const noexcept {
	/*
	 * 1. Both Source and Destination must be active IDs
	 * 2. Destination cannot be controller or worker IDs
	 * 3. Allow client -> supernode communication only via controller
	 * 4. Apply netmask over all client -> * communications
	 */
	auto checkActive = !Socket::isEphemeralId(source)
			&& !Socket::isEphemeralId(destination);
	auto checkDestinations = !(isController(destination)
			|| isWorkerId(destination));
	auto checkPrivilege = isController(getUid())
			|| !(isExternalNode(source) && isInternalNode(destination));

	return checkActive && checkDestinations && checkPrivilege
			&& checkMask(source, destination);
}

bool OverlayHub::checkMask(unsigned long long source,
		unsigned long long destination) const noexcept {
	return isInternalNode(source)
			|| ((source & ctx.netMask) == (destination & ctx.netMask));
}

bool OverlayHub::process(Message *message) noexcept {
	switch (message->getCommand()) {
	case WH_DHT_CMD_NULL:
		return processNullRequest(message);
	case WH_DHT_CMD_BASIC:
		return processBasicRequest(message);
	case WH_DHT_CMD_MULTICAST:
		return processMulticastRequest(message);
	case WH_DHT_CMD_NODE:
		return processNodeRequest(message);
	case WH_DHT_CMD_OVERLAY:
		return processOverlayRequest(message);
	default:
		return handleInvalidRequest(message);
	}
}

bool OverlayHub::processNullRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();
	auto status = message->getStatus();
	if (cmd != WH_DHT_CMD_NULL) {
		return handleInvalidRequest(message);
	} else if (!isPrivileged(origin) || status != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(message);
	} else if (qlf == WH_DHT_QLF_DESCRIBE) {
		return handleDescribeNodeRequest(message);
	} else {
		return handleInvalidRequest(message);
	}
}

bool OverlayHub::processBasicRequest(Message *message) noexcept {
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();
	auto status = message->getStatus();
	if (cmd != WH_DHT_CMD_BASIC) {
		return handleFindRootRequest(message);
	} else if (qlf == WH_DHT_QLF_FINDROOT) {
		return handleFindRootRequest(message);
	} else if (qlf == WH_DHT_QLF_BOOTSTRAP && status == WH_DHT_AQLF_REQUEST) {
		return handleBootstrapRequest(message);
	} else {
		return handleInvalidRequest(message);
	}
}

bool OverlayHub::processMulticastRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();
	auto status = message->getStatus();
	if (cmd != WH_DHT_CMD_MULTICAST) {
		return handleInvalidRequest(message);
	} else if (isSupernode() || isInternalNode(origin)
			|| Socket::isEphemeralId(origin) || status != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(message);
	} else if (qlf == WH_DHT_QLF_PUBLISH) {
		return handlePublishRequest(message);
	} else if (qlf == WH_DHT_QLF_SUBSCRIBE) {
		return handleSubscribeRequest(message);
	} else if (qlf == WH_DHT_QLF_UNSUBSCRIBE) {
		return handleUnsubscribeRequest(message);
	} else {
		return handleInvalidRequest(message);
	}
}

bool OverlayHub::processNodeRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();
	auto status = message->getStatus();
	if (cmd != WH_DHT_CMD_NODE) {
		return handleInvalidRequest(message);
	} else if (!(isController(origin) || isWorkerId(origin))
			|| status != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(message);
	} else if (qlf == WH_DHT_QLF_GETPREDECESSOR) {
		return handleGetPredecessorRequest(message);
	} else if (qlf == WH_DHT_QLF_SETPREDECESSOR) {
		return handleSetPredecessorRequest(message);
	} else if (qlf == WH_DHT_QLF_GETSUCCESSOR) {
		return handleGetSuccessorRequest(message);
	} else if (qlf == WH_DHT_QLF_SETSUCCESSOR) {
		return handleSetSuccessorRequest(message);
	} else if (qlf == WH_DHT_QLF_GETFINGER) {
		return handleGetFingerRequest(message);
	} else if (qlf == WH_DHT_QLF_SETFINGER) {
		return handleSetFingerRequest(message);
	} else if (qlf == WH_DHT_QLF_GETNEIGHBOURS) {
		return handleGetNeighboursRequest(message);
	} else if (qlf == WH_DHT_QLF_NOTIFY) {
		return handleNotifyRequest(message);
	} else {
		return handleInvalidRequest(message);
	}
}

bool OverlayHub::processOverlayRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto cmd = message->getCommand();
	auto qlf = message->getQualifier();
	auto status = message->getStatus();
	if (cmd != WH_DHT_CMD_OVERLAY) {
		return handleInvalidRequest(message);
	} else if (!isPrivileged(origin) || status != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(message);
	} else if (qlf == WH_DHT_QLF_FINDSUCCESSOR) {
		return handleFindSuccesssorRequest(message);
	} else if (qlf == WH_DHT_QLF_PING) {
		return handlePingNodeRequest(message);
	} else if (qlf == WH_DHT_QLF_MAP) {
		return handleMapRequest(message);
	} else {
		return handleInvalidRequest(message);
	}
}

bool OverlayHub::handleInvalidRequest(Message *msg) noexcept {
	//The message will be recycled
	msg->setCommand(WH_DHT_CMD_NULL);
	msg->setStatus(WH_DHT_AQLF_REJECTED);
	msg->setDestination(getUid());
	return true;
}

bool OverlayHub::handleDescribeNodeRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=0, QLF=127, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 84+25*Node::TABLESIZE bytes in Response
	 * TOTAL: 32 bytes in Request; 116+25*Node::TABLESIZE bytes in Response
	 */
	if (msg->getLength() != Message::HEADER_SIZE) {
		return handleInvalidRequest(msg);
	}
	//-----------------------------------------------------------------
	//This function can be extended to include more data
	//-----------------------------------------------------------------
	//ID(8)->MTU(2)->MAX_CONN(4)->CONN(4)->MAX_MSGS(4)->MSGS(4)->UPTIME(8)
	unsigned int index = 0;
	msg->setData64(index, getUid()); //KEY
	index += sizeof(uint64_t);
	msg->setData16(index, Message::MTU);
	index += sizeof(uint16_t);
	msg->setData32(index, Socket::poolSize());
	index += sizeof(uint32_t);
	msg->setData32(index, Socket::allocated());
	index += sizeof(uint32_t);
	msg->setData32(index, Message::poolSize());
	index += sizeof(uint32_t);
	msg->setData32(index, Message::allocated());
	index += sizeof(uint32_t);
	msg->setDouble(index, getUptime());
	index += sizeof(uint64_t);
	//-----------------------------------------------------------------
	//IN_PACKETS(8)->IN_BYTES(8)->DROPPED_PACKETS(8)->DROPPED_BYTES(8)
	msg->setData64(index, messagesReceived());
	index += sizeof(uint64_t);
	msg->setData64(index, bytesReceived());
	index += sizeof(uint64_t);
	msg->setData64(index, messagesDropped());
	index += sizeof(uint64_t);
	msg->setData64(index, bytesDropped());
	index += sizeof(uint64_t);
	//-----------------------------------------------------------------
	//Predecessor(8)->Successor(8)->STABLE_FLAG(1)->TABLE_SIZE(1)
	msg->setData64(index, getPredecessor()); //Predecessor
	index += sizeof(uint64_t);
	msg->setData64(index, getSuccessor()); //Successor
	index += sizeof(uint64_t);
	msg->setData8(index, (isStable() ? 1 : 0)); //Routing table atatus
	index += sizeof(uint8_t);
	msg->setData8(index, (uint8_t) TABLESIZE); //Size of routing table
	index += sizeof(uint8_t);
	//-----------------------------------------------------------------
	for (unsigned int i = 0; i < TABLESIZE; i++) {
		//START(8)->ID(8)->OLD_ID(8)->CONNECTED(1)
		auto f = getFinger(i);
		msg->setData64(index, f->getStart());
		index += sizeof(uint64_t);
		msg->setData64(index, f->getId());
		index += sizeof(uint64_t);
		msg->setData64(index, f->getOldId());
		index += sizeof(uint64_t);
		msg->setData8(index, (f->isConnected() ? 1 : 0));
		index += sizeof(uint8_t);
	}
	//-----------------------------------------------------------------
	buildDirectResponse(msg, Message::HEADER_SIZE + index);
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	return true;
}

bool OverlayHub::handleRegistrationRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=<REQUESTED ID>, DEST=IGN, ....CMD=1, QLF=0, AQLF=0/1/127
	 * BODY: 64-byte CHALLANGE CODE in Request (optional); nothing in Response
	 * TOTAL: 32+64=96 bytes in Request; 32 bytes in Response
	 */

	//Get the UID of the connection object from which this message was received
	auto origin = msg->getOrigin();
	//Get the Requested UID
	auto requestedUid = msg->getSource();
	//Trap this message before publishing to the remote host
	msg->setFlags(MSG_TRAP);
	//-----------------------------------------------------------------
	/*
	 * [PROXY ESTABLISHMENT]
	 * In case a Proxy Connection with the remote node is being established by the
	 * local node, we might receive a <ACCEPTED> or a <REJECTED> message over the
	 * proxy connection. In such case no further processing is needed
	 */
	if (msg->isType(SOCKET_PROXY) && msg->getStatus() != WH_DHT_AQLF_REQUEST) {
		msg->setDestination(origin);
		//Set the source to this message's origin (Server performs source check)
		msg->setSource(origin);
		return true;
	}
	//-----------------------------------------------------------------
	/*
	 * Treat all the other cases as a registration request
	 */
	//Do this before the message is modified
	auto success = isValidRegistrationRequest(msg);
	//Set the source to this message's origin (Server performs source check)
	msg->setSource(origin);
	//-----------------------------------------------------------------
	if (success) {
		WH_LOG_DEBUG("Registration request %" PRIu64"->%" PRIu64" approved",
				origin, requestedUid);
		//Request Accepted, message will be delivered on new UID
		msg->writeSource(0);
		msg->writeDestination(0);
		msg->setDestination(requestedUid);
		msg->putLength(Message::HEADER_SIZE);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	} else {
		WH_LOG_DEBUG("Registration request %" PRIu64"->%" PRIu64" denied",
				origin, requestedUid);
		//Request denied, regret message will be sent on old ID
		msg->writeSource(0);
		msg->writeDestination(0);
		msg->setDestination(origin);
		msg->putLength(Message::HEADER_SIZE);
		msg->putStatus(WH_DHT_AQLF_REJECTED);
	}
	return true;
}

bool OverlayHub::handleGetKeyRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=1, AQLF=0/1/127
	 * BODY: 512/8=64 Bytes in Request (optional), (512/8)*2=128 Bytes in Response
	 * TOTAL: 32+64=96 bytes in Request; 32+128=160 bytes in Response
	 */
	auto origin = msg->getOrigin();
	//-----------------------------------------------------------------
	/*
	 * [PROXY ESTABLISHMENT]
	 * In case a Proxy Connection with the remote node is being established by
	 * the local node, we might receive a nonce from the remote node if such a
	 * request was made. Send a registration request to complete the process.
	 * [GetKey request -> receive nonce -> Registration request -> activate]
	 */
	if (msg->isType(SOCKET_PROXY) && msg->getStatus() != WH_DHT_AQLF_REQUEST) {
		//Check message integrity
		if (msg->getStatus() != WH_DHT_AQLF_ACCEPTED) {
			return handleInvalidRequest(msg);
		} else if (!((msg->getPayloadLength() == 2 * Hash::SIZE)
				|| (msg->getPayloadLength()
						== 2 * Hash::SIZE + PKI::SIGNATURE_LENGTH))) {
			return handleInvalidRequest(msg);
		} else if (!msg->verify(verifyHost() ? getPKI() : nullptr)) {
			return handleInvalidRequest(msg);
		} else if (nonceToId((Digest*) msg->getBytes(0)) != origin) {
			return handleInvalidRequest(msg);
		} else {
			//Convert the message into a Registration Request
			Digest hc;
			memcpy(&hc, msg->getBytes(Hash::SIZE), Hash::SIZE);
			Protocol::createRegisterRequest( { getUid(), origin }, &hc, msg);
			msg->sign(getPKI());
			//We are sending a registration request to the remote Node.
			msg->setDestination(origin);
			return true;
		}
	}
	//-----------------------------------------------------------------
	/*
	 * This call succeeds if the caller is a temporary connection and the
	 * message is of proper size, otherwise a failure message is sent back.
	 */
	if (Socket::isEphemeralId(origin)
			&& msg->getPayloadLength() <= Hash::SIZE) {
		Digest hc;	//Challenge Key
		memset(&hc, 0, sizeof(hc));
		generateNonce(hash, origin, getUid(), &hc);
		msg->appendBytes((const unsigned char*) &hc, Hash::SIZE);
		msg->writeSource(0);
		msg->writeDestination(0);
		msg->setDestination(origin);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	} else if (Socket::isEphemeralId(origin)
			&& msg->getPayloadLength() == PKI::ENCRYPTED_LENGTH && verifyHost()
			&& getPKI()) {
		//Extract the challenge key
		unsigned char challenge[PKI::ENCODING_LENGTH]; //Challenge
		memset(&challenge, 0, sizeof(challenge));
		getPKI()->decrypt((const PKIEncryptedData*) msg->getBytes(0),
				&challenge);
		msg->setBytes(0, (const unsigned char*) &challenge, Hash::SIZE);
		//Build and return the session key
		Digest hc; //Response
		memset(&hc, 0, sizeof(hc));
		generateNonce(hash, origin, getUid(), &hc);
		msg->setBytes(Hash::SIZE, (const unsigned char*) &hc, Hash::SIZE);
		msg->writeSource(0);
		msg->writeDestination(0);
		msg->setDestination(origin);
		msg->putLength(Message::HEADER_SIZE + 2 * Hash::SIZE);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		msg->sign(getPKI());
	} else {
		msg->writeSource(0);
		msg->writeDestination(0);
		msg->setDestination(origin);
		msg->putLength(Message::HEADER_SIZE);
		msg->putStatus(WH_DHT_AQLF_REJECTED);
	}
	return true;
}

bool OverlayHub::handleFindRootRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=2, AQLF=0/1/127
	 * BODY: 8 bytes as <id> in Request; 8 bytes as <id> and 8 bytes
	 * as <successor> in Response
	 * TOTAL: 32+8=40 bytes in Request; 32+8+8=48 bytes in Response
	 */
	//At this point we know that this hub was the intended destination
	auto origin = msg->getOrigin();
	auto source = msg->getSource();
	//-----------------------------------------------------------------
	//We have received the result, perform a clean-up and deliver
	if (msg->getStatus() == WH_DHT_AQLF_ACCEPTED) {
		if (isInternalNode(origin)
				&& msg->getPayloadLength() == (4 * sizeof(uint64_t))) {
			msg->putLength(Message::HEADER_SIZE + 2 * sizeof(uint64_t));
			msg->setDestination(msg->getData64(2 * sizeof(uint64_t)));
			msg->writeSource(0);
			if (isController(msg->getDestination())) {
				msg->writeDestination(msg->getData64(3 * sizeof(uint64_t)));
			} else {
				msg->writeDestination(0);
			}
			return true;
		} else {
			return handleInvalidRequest(msg);
		}
	}
	//-----------------------------------------------------------------
	auto queryId = msg->getData64(0); //Get the query
	auto localSuccessor = Node::localSuccessor(mapKey(queryId));
	//-----------------------------------------------------------------
	if (localSuccessor || isController(getUid())) {
		//Found the successor, save it into the message
		msg->setData64(sizeof(uint64_t), localSuccessor);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		if (isExternalNode(origin) || isController(getUid())
				|| isController(origin)) {
			//Request was initiated locally, send direct response
			msg->putLength(Message::HEADER_SIZE + 2 * sizeof(uint64_t));
			msg->setDestination(origin);
			msg->writeSource(0);
			msg->writeDestination(isController(origin) ? source : 0);
			return true;
		} else {
			//Request was initiated remotely, route towards the originator
			msg->putDestination(source);
			return createRoute(msg);
		}
	} else {
		if (isExternalNode(origin) || isController(origin)) {
			//Received a fresh request
			if (msg->getPayloadLength() == sizeof(uint64_t)
					&& msg->getStatus() == WH_DHT_AQLF_REQUEST) {
				msg->putLength(Message::HEADER_SIZE + 4 * sizeof(uint64_t));
				msg->setData64(2 * sizeof(uint64_t), origin); //Record the origin
				msg->setData64(3 * sizeof(uint64_t), source); //Final destination
				msg->writeSource(getUid()); //Result will be looped back here
			} else {
				return handleInvalidRequest(msg);
			}
		}
		//Forward the ongoing request to the closest predecessor
		auto precedingNode = closestPredecessor(mapKey(queryId), true);
		msg->putDestination(precedingNode);
		return true;
	}
}

bool OverlayHub::handleBootstrapRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=3, AQLF=0/1/127
	 * BODY: 0 in Request; 4 bytes as count + 8*NODECACHE_SIZE bytes
	 * as IDs in Response
	 * TOTAL: 32 bytes in Request; 32+4+8*NODECACHE_SIZE bytes in Response
	 */
	auto origin = msg->getOrigin();
	auto source = msg->getSource();
	//-----------------------------------------------------------------
	if (msg->getLength() != Message::HEADER_SIZE) {
		return handleInvalidRequest(msg);
	} else {
		//Direct requests only
		msg->writeSource(0);
		msg->writeDestination(isExternalNode(origin) ? 0 : source);
	}
	//-----------------------------------------------------------------
	msg->setDestination(origin);
	msg->putLength(
			Message::HEADER_SIZE + sizeof(uint32_t)
					+ (sizeof(uint64_t) * NODECACHE_SIZE));
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	//-----------------------------------------------------------------
	//Number of IDs returned, same as the size of the cache
	msg->setData32(0, NODECACHE_SIZE);
	unsigned int offset = sizeof(uint32_t);
	for (unsigned int i = 0; i < NODECACHE_SIZE; i++) {
		msg->setData64(offset, nodes.cache[i]);
		offset += sizeof(uint64_t);
	}
	return true;
}

bool OverlayHub::handlePublishRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=0, AQLF=0/1/127
	 * BODY: variable in Request; no Response
	 * TOTAL: at least 32 bytes in Request; no Response
	 */
	Watcher *sub = nullptr;
	unsigned int index = 0;
	auto topic = msg->getSession();
	while ((sub = topics.get(topic, index))) {
		if (sub->getUid() != msg->getOrigin()
				&& checkMask(msg->getOrigin(), sub->getUid())
				&& !sub->testGroup(msg->getGroup()) && sub->publish(msg)
				&& sub->isReady()) {
			retain(sub);
		}
		++index;
	}

	msg->writeLabel(0); //Clean up internal information
	msg->writeDestination(0); //There are multiple destinations
	msg->writeStatus(WH_DHT_AQLF_ACCEPTED); //Prevent rebound
	msg->addReferenceCount(); //Account for Hub::publish
	return true;
}

bool OverlayHub::handleSubscribeRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=1, AQLF=0/1/127
	 * BODY: 0 in Request; 0 in Response
	 * TOTAL: 32 bytes in Request; 32 bytes in Response
	 */
	if (msg->getLength() != Message::HEADER_SIZE) {
		return handleInvalidRequest(msg);
	}

	auto topic = msg->getSession();
	Socket *conn = (Socket*) getWatcher(msg->getOrigin());
	buildDirectResponse(msg, Message::HEADER_SIZE);
	msg->writeSource(0); //Obfuscate the source (this hub)

	if (!conn) {
		return handleInvalidRequest(msg);
	} else if (conn->testTopic(topic)) {
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	} else if (topics.put(topic, conn)) {
		conn->setTopic(topic);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	} else {
		msg->putStatus(WH_DHT_AQLF_REJECTED);
	}
	return true;
}

bool OverlayHub::handleUnsubscribeRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=2, QLF=2, AQLF=0/1/127
	 * BODY: 0 in Request; 0 in Response
	 * TOTAL: 32 bytes in Request; 32 bytes in Response
	 */
	if (msg->getLength() != Message::HEADER_SIZE) {
		return handleInvalidRequest(msg);
	}

	auto topic = msg->getSession();
	Socket *conn = (Socket*) getWatcher(msg->getOrigin());

	if (conn && conn->testTopic(topic)) {
		conn->clearTopic(topic);
		topics.remove(topic, conn);
	}

	buildDirectResponse(msg, Message::HEADER_SIZE);
	msg->writeSource(0); //Obfuscate the source (this hub)
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	return true;
}

bool OverlayHub::handleGetPredecessorRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=0, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 8 bytes as <predecessor> in Response
	 * TOTAL: 32 bytes in Request; 32+8=40 bytes in Response
	 */
	if (msg->getLength() != Message::HEADER_SIZE) {
		return handleInvalidRequest(msg);
	}

	buildDirectResponse(msg, Message::HEADER_SIZE + sizeof(uint64_t));
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	msg->setData64(0, getPredecessor());
	return true;
}

bool OverlayHub::handleSetPredecessorRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=1, AQLF=0/1/127
	 * BODY: 8 bytes as <predecessor> in Request; 8 bytes as <predecessor> in Response
	 * TOTAL: 32+8=40 bytes
	 */
	if (msg->getPayloadLength() != sizeof(uint64_t)) {
		return handleInvalidRequest(msg);
	}

	buildDirectResponse(msg);
	//Get the new Predecessor ID and run update
	if (setPredecessor(msg->getData64(0))) {
		//Success
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		msg->setData64(0, getPredecessor()); //confirm
	} else {
		//Failure
		msg->putStatus(WH_DHT_AQLF_REJECTED);
		msg->setData64(0, 0);
	}
	return true;
}

bool OverlayHub::handleGetSuccessorRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=2, AQLF=0/1/127
	 * BODY: 0 bytes in Request; 8 bytes as <successor> in Response
	 * TOTAL: 32 bytes in Request; 32+8=40 bytes in Response
	 */
	if (msg->getLength() != Message::HEADER_SIZE) {
		return handleInvalidRequest(msg);
	}

	buildDirectResponse(msg, Message::HEADER_SIZE + sizeof(uint64_t));
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	msg->setData64(0, getSuccessor());
	return true;
}

bool OverlayHub::handleSetSuccessorRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=3, AQLF=0/1/127
	 * BODY: 8 bytes as <successor> in Request; 8 bytes as <successor> in response
	 * TOTAL: 32+8=40 bytes
	 */
	if (msg->getPayloadLength() != sizeof(uint64_t)) {
		return handleInvalidRequest(msg);
	}

	buildDirectResponse(msg);
	//Get the new Successor ID and run update
	if (setSuccessor(msg->getData64(0))) {
		//Success
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		msg->setData64(0, getSuccessor()); //confirm
	} else {
		//Failure
		msg->putStatus(WH_DHT_AQLF_REJECTED);
		msg->setData64(0, 0);
	}
	return true;
}

bool OverlayHub::handleGetFingerRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=4, AQLF=0/1/127
	 * BODY: 4 bytes in Request as <index>; 4 bytes as <index> + 8 bytes
	 * as <finger> in Response
	 * TOTAL: 32+4=36 bytes in Request; 32+4+8=44 bytes in Response
	 */
	if (msg->getPayloadLength() != sizeof(uint32_t)) {
		return handleInvalidRequest(msg);
	}

	buildDirectResponse(msg,
			Message::HEADER_SIZE + sizeof(uint32_t) + sizeof(uint64_t));
	auto index = msg->getData32(0);
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	msg->setData64(sizeof(uint32_t), get(index));
	return true;
}

bool OverlayHub::handleSetFingerRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=5, AQLF=0/1/127
	 * BODY: 4 bytes as <index> and 8 bytes as <finger> in Request and in Response
	 * TOTAL: 32+4+8=44 bytes
	 */
	if (msg->getPayloadLength() != sizeof(uint32_t) + sizeof(uint64_t)) {
		return handleInvalidRequest(msg);
	}

	buildDirectResponse(msg);
	auto index = msg->getData32(0);
	auto newNode = msg->getData64(sizeof(uint32_t));

	if (set(index, newNode)) {
		//Success
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		msg->setData64(sizeof(uint32_t), get(index)); //confirm
	} else {
		//Failure
		msg->putStatus(WH_DHT_AQLF_REJECTED);
		msg->setData64(sizeof(uint32_t), 0);
	}
	return true;
}

bool OverlayHub::handleGetNeighboursRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=6, AQLF=0/1/127
	 * BODY: 0 bytes in REQ; 8 bytes as <predecessor> + 8 bytes
	 * as <successor> in Response
	 * TOTAL: 32 bytes in Request; 32+8+8=48 bytes in Response
	 */
	if (msg->getLength() != Message::HEADER_SIZE) {
		return handleInvalidRequest(msg);
	}

	buildDirectResponse(msg, Message::HEADER_SIZE + 2 * sizeof(uint64_t));
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	msg->setData64(0, getPredecessor());
	msg->setData64(sizeof(uint64_t), getSuccessor());
	return true;
}

bool OverlayHub::handleNotifyRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=3, QLF=7, AQLF=0/1/127
	 * BODY: 8 bytes as <predecessor> in Request; 0 bytes in Response
	 * TOTAL: 32+8=40 bytes in Request; 32 bytes in Response
	 */
	if (msg->getPayloadLength() != sizeof(uint64_t)) {
		return handleInvalidRequest(msg);
	}
	buildDirectResponse(msg, Message::HEADER_SIZE);
	msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	//Notify self about the probable predecessor
	notify(msg->getData64(0));
	return true;
}

bool OverlayHub::handleFindSuccesssorRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=0, AQLF=0/1/127
	 * BODY: 8 bytes as <id> in Request; 8 bytes as <id> and 8 bytes
	 * as <successor> in Response
	 * TOTAL: 32+8=40 bytes in Request; 32+8+8=48 bytes in Response
	 */
	auto origin = msg->getOrigin();
	//-----------------------------------------------------------------
	if (!(isController(origin) || isWorkerId(origin))) {
		return handleInvalidRequest(msg);
	}

	if (msg->getPayloadLength() != sizeof(uint64_t)) {
		return handleInvalidRequest(msg);
	}
	//-----------------------------------------------------------------
	auto id = msg->getData64(0); //Get parameter
	auto localSuccessor = Node::localSuccessor(mapKey(id));
	if (localSuccessor) {
		//Found the successor
		buildDirectResponse(msg, Message::HEADER_SIZE + 2 * sizeof(uint64_t));
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		msg->setData64(sizeof(uint64_t), localSuccessor);
	} else {
		//Forward the query to the closest preceding node via the controller
		auto precedingNode = closestPredecessor(mapKey(id));
		//Remove recursion, no point in forwarding to self
		if (precedingNode != getUid()) {
			msg->setDestination(CONTROLLER);
			msg->writeDestination(precedingNode);
		} else {
			return handleInvalidRequest(msg);
		}
	}
	return true;
}

bool OverlayHub::handlePingNodeRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=1, AQLF=0/1/127
	 * BODY: 0 bytes in Request and Response
	 * TOTAL: 32 bytes in Request and Response
	 */
	auto origin = msg->getOrigin();
	//-----------------------------------------------------------------
	if (isWorkerId(origin)) {
		//Allows the worker to ask for maintenance
		Node::setStable(false);
		buildDirectResponse(msg);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		return true;
	} else if (isController(origin) || isController(getUid())) {
		buildDirectResponse(msg);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
		return true;
	} else {
		return handleInvalidRequest(msg);
	}
}

bool OverlayHub::handleMapRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=4, QLF=2, AQLF=0/1/127
	 * BODY: variable in Request; variable in Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	auto origin = msg->getOrigin();
	auto source = msg->getSource();
	//-----------------------------------------------------------------
	/*
	 * Message insertion
	 * At this point we are sure that this node was the intended recipient.
	 */
	if (isExternalNode(origin)) {
		return handleInvalidRequest(msg);
	} else if (isController(origin)) {
		//Cache the source ID
		if (msg->appendData64(source)) {
			//Record the entry point
			msg->putSource(getUid());
		} else {
			//Invalid message length
			return handleInvalidRequest(msg);
		}
	} else if (isExternalNode(source) || origin != getPredecessor()
			|| msg->getPayloadLength() < sizeof(uint64_t)) {
		//Routing around the ring: invalid message
		return handleInvalidRequest(msg);
	}
	//-----------------------------------------------------------------
	/*
	 * Execute and forward
	 */
	auto result = mapFunction(msg);
	auto successor = getSuccessor();
	if (result == 0
			&& (isHostId(source) || !isInRange(source, getUid(), successor))
			&& getUid() != successor) {
		/*
		 * Forward the Message around the DHT ring until it reaches
		 * the predecessor of the node where the message was originally
		 * inserted (or the hole).
		 */
		msg->putDestination(successor);
	} else {
		//Return back to the originator, final step
		auto index = msg->getLength()
				- (sizeof(uint64_t) + Message::HEADER_SIZE);
		auto destination = msg->getData64(index);

		msg->setDestination(0);
		msg->writeDestination(destination);
		msg->putSource(getUid());
		msg->putLength(Message::HEADER_SIZE);
		msg->putStatus(
				(result == 0 || result == 1) ?
						WH_DHT_AQLF_ACCEPTED : WH_DHT_AQLF_REJECTED);
	}
	return true;
}

int OverlayHub::mapFunction(Message *msg) noexcept {
	WH_LOG_ALERT("~~Received a Map Request~~");
	return 0;
}

void OverlayHub::buildDirectResponse(Message *msg, unsigned int length) noexcept {
	auto origin = msg->getOrigin(); //Actual source
	auto source = msg->getSource(); //Declared source
	//-----------------------------------------------------------------
	//Update the routing information
	msg->setDestination(origin); //Forward
	msg->writeDestination(source); //Destination

	msg->putSource(getUid());

	if (length != 0) {
		msg->putLength(length);
	}
}

unsigned int OverlayHub::mapKey(unsigned long long key) noexcept {
	if (key > (MAX_ID + MAX_NODES)) {
		//Take the higher bits into account
		return static_cast<unsigned int>(Twiddler::mix(key) & MAX_ID);
	} else {
		return static_cast<unsigned int>(key & MAX_ID);
	}
}

unsigned long long OverlayHub::nonceToId(const Digest *nonce) const noexcept {
	unsigned int i = 0;
	for (; i <= TABLESIZE; i++) {
		if (memcmp(nonce, &sessions[i], sizeof(Digest)) == 0)
			break;
	}

	if (i < TABLESIZE) {
		return get(i);
	} else if (i == TABLESIZE) {
		return CONTROLLER;
	} else {
		return getUid();
	}
}

unsigned long long OverlayHub::getWorkerId() const noexcept {
	return worker.id;
}

bool OverlayHub::isWorkerId(unsigned long long uid) const noexcept {
	return (uid == worker.id) && !isHostId(uid);
}

bool OverlayHub::isPrivileged(unsigned long long uid) const noexcept {
	return isInternalNode(uid) || isWorkerId(uid);
}

bool OverlayHub::isSupernode() const noexcept {
	return (ctx.connectToOverlay && !isController(getUid()));
}

bool OverlayHub::isHostId(unsigned long long uid) const noexcept {
	return uid == getUid();
}

bool OverlayHub::isController(unsigned long long uid) noexcept {
	return uid == CONTROLLER;
}

bool OverlayHub::isInternalNode(unsigned long long uid) noexcept {
	return uid <= MAX_ID;
}

bool OverlayHub::isExternalNode(unsigned long long uid) noexcept {
	return uid > MAX_ID;
}

Watcher* OverlayHub::connect(int &sfd, bool blocking, int timeout) {
	Socket *local = nullptr;
	int socket = -1;
	try {
		local = Socket::createSocketPair(socket);
		if (blocking) {
			Network::setBlocking(socket, true);
			Network::setSocketTimeout(socket, timeout, timeout);
		}
		putWatcher(local, IO_WR, WATCHER_ACTIVE);
		sfd = socket;
		return local;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		Network::close(socket);
		delete local;
		throw;
	}
}

Watcher* OverlayHub::connect(unsigned long long id, Digest *hc) {
	Watcher *conn = nullptr;
	if (!(conn = getWatcher(id))) {
		WH_LOG_DEBUG("Connecting to %llu", id);
		createProxyConnection(id, hc);
		//Not registered yet
		return nullptr;
	} else if (conn->testFlags(WATCHER_ACTIVE)) {
		//Registration completed
		return conn;
	} else if (((Socket*) conn)->hasTimedOut(ctx.requestTimeout)) {
		disable(conn);
		return nullptr;
	} else {
		//wait for registration or time-out
		return nullptr;
	}
}

Watcher* OverlayHub::createProxyConnection(unsigned long long id, Digest *hc) {
	Socket *conn = nullptr;
	try {
		if (getUid() == id || !hc) {
			throw Exception(EX_INVALIDPARAM);
		}

		NameInfo ni;
		Identity::getAddress(id, ni);
		conn = new Socket(ni);
		//-----------------------------------------------------------------
		//A getKey request is automatically sent out
		generateNonce(hash, conn->getUid(), getUid(), hc);
		auto msg = Protocol::createGetKeyRequest( { 0, id },
				{ verifyHost() ? getPKI() : nullptr, hc }, nullptr);
		if (!msg) {
			throw Exception(EX_ALLOCFAILED);
		}
		conn->publish(msg);
		conn->setUid(id);
		putWatcher(conn, IO_WR, 0);
		return conn;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete conn;
		throw;
	}
}

unsigned int OverlayHub::purgeConnections(int mode, unsigned target) noexcept {
	purge.target = target;
	purge.count = 0;
	switch (mode) {
	case 0:
		return purgeTemporaryConnections(target);
	case 1:
		iterateWatchers(removeIfInvalid, this);
		return purge.count;
	case 2:
		iterateWatchers(removeIfClient, this);
		return purge.count;
	default:
		purge.count = purgeTemporaryConnections(target, true);
		if (!target || purge.count < target) {
			iterateWatchers(removeIfClient, this);
		}
		return purge.count;
	}
}

int OverlayHub::removeIfInvalid(Watcher *w, void *arg) noexcept {
	auto uid = w->getUid();
	auto hub = static_cast<OverlayHub*>(arg);
	if (hub->purge.target && hub->purge.count >= hub->purge.target) {
		return -1;
	} else if (!(Socket::isEphemeralId(uid) || hub->isLocal(mapKey(uid))
			|| hub->isInternalNode(uid) || hub->isWorkerId(uid))) {
		hub->disable(w);
		hub->purge.count++;
		//No need to remove from the lookup table
		return 0;
	} else {
		return 0;
	}
}

int OverlayHub::removeIfClient(Watcher *w, void *arg) noexcept {
	auto uid = w->getUid();
	auto hub = static_cast<OverlayHub*>(arg);
	if (hub->purge.target && hub->purge.count >= hub->purge.target) {
		return -1;
	} else if (isExternalNode(uid) && !hub->isWorkerId(uid)
			&& !(Socket::isEphemeralId(uid) && w->testFlags(WATCHER_ACTIVE))) {
		hub->disable(w);
		hub->purge.count++;
		//No need to remove from the lookup table
		return 0;
	} else {
		return 0;
	}
}

void OverlayHub::clear() noexcept {
	worker.header.clear();
	worker.id = getUid();

	memset(&ctx, 0, sizeof(ctx));
	memset(&nodes, 0, sizeof(nodes));
	memset(sessions, 0, sizeof(sessions));
	memset(&purge, 0, sizeof(purge));

	for (unsigned int i = 0; i < WATCHLIST_SIZE; ++i) {
		watchlist[i].identifier = -1;
		watchlist[i].events = 0;
	}

	topics.clear();
}

} /* namespace wanhive */
