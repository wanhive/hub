/*
 * OverlayHub.cpp
 *
 * Overlay hub
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

namespace {

/* Connection purge modes */
enum PurgeType {
	PURGE_TEMPORARY, PURGE_INVALID, PURGE_CLIENT
};

/* Connections purge control structure */
struct PurgeControl {
	unsigned int target { 0 };
	unsigned int count { 0 };
	wanhive::OverlayHub *hub { nullptr };
};
//-----------------------------------------------------------------
/* Message trace types */
enum MessageTrace : uint32_t {
	SESSION_TRACE = 1U
};

/* Token bucket's default level (for the registration requests) */
constexpr unsigned long long DEF_TOKENS_COUNT = 200;

//-----------------------------------------------------------------
}// namespace

namespace wanhive {

OverlayHub::OverlayHub(unsigned long long uid, const char *path) :
		Hub(uid, path), Node(uid), stabilizer(uid) {
	clear();
}

OverlayHub::~OverlayHub() {

}

void OverlayHub::expel(Watcher *w) noexcept {
	offboard(w);
	Hub::expel(w);
}

void OverlayHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		auto &conf = Identity::getOptions();
		ctx.enroll = conf.getBoolean("OVERLAY", "enroll");
		ctx.authenticate = conf.getBoolean("OVERLAY", "authenticate");
		ctx.join = conf.getBoolean("OVERLAY", "join");
		ctx.period = conf.getNumber("OVERLAY", "period", 5000);
		ctx.timeout = conf.getNumber("OVERLAY", "timeout", 5000);
		ctx.pause = conf.getNumber("OVERLAY", "pause", 10000);
		auto hex = conf.getString("OVERLAY", "netmask", "0x0");
		sscanf(hex, "%llx", &ctx.netmask);
		ctx.group = conf.getNumber("OVERLAY", "group");

		auto n = Identity::getIdentifiers("BOOTSTRAP", "nodes", ctx.nodes,
				ArraySize(ctx.nodes) - 1);
		if (!n) {
			n = Identity::getIdentifiers(ctx.nodes, ArraySize(ctx.nodes) - 1,
					Hosts::BOOTSTRAP);
		}
		ctx.nodes[n] = 0;

		WH_LOG_DEBUG(
				"\nENABLE_REGISTRATION=%s, AUTHENTICATE_CLIENTS=%s, JOIN_OVERLAY=%s,\n" "UPDATE_CYCLE=%ums, IO_TIMEOUT=%ums, RETRY_INTERVAL=%ums,\n" "NETMASK=%#llx, GROUP_ID=%u\n",
				WH_BOOLF(ctx.enroll), WH_BOOLF(ctx.authenticate),
				WH_BOOLF(ctx.join), ctx.period, ctx.timeout, ctx.pause,
				ctx.netmask, ctx.group);
		installService();
		installTracker();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void OverlayHub::cleanup() noexcept {
	Watcher *w = nullptr;
	if (!isHost(getWorker()) && (w = find(getWorker()))) {
		//Shut down hub's end of the socket pair
		w->stop();
		stabilizer.notify();
	}

	clear();
	//Clean up the base class
	Hub::cleanup();
}

void OverlayHub::maintain() noexcept {
	converge();
}

bool OverlayHub::probe(Message *message) noexcept {
	return (bool) enroll(message);
}

void OverlayHub::route(Message *message) noexcept {
	//-----------------------------------------------------------------
	/*
	 * [REGISTRATION]: Intercept and handle registration and session-key requests
	 * because modification in the message will result in verification failure.
	 */
	if (intercept(message)) {
		message->setGroup(0); //Ignore the group ID
		return;
	}
	//-----------------------------------------------------------------
	/*
	 * [FLOW CONTROL]: apply correct source, group, and label
	 */
	annotate(message);
	//-----------------------------------------------------------------
	/*
	 * [ROUTING]: Hub's ID is the sink
	 */
	plot(message);
	//-----------------------------------------------------------------
	/*
	 * [PROCESSING]: Process the local requests
	 */
	if (isHost(message->getDestination()) && !message->testFlags(MSG_INVALID)) {
		//Maintain this order
		serve(message); //Process the local request
		message->setGroup(0); //Ignore the group ID
	}
	//-----------------------------------------------------------------
	/*
	 * [DELIVERY]: deliver to the local destination
	 */
	if (isExternal(message->getDestination())) {
		message->writeLabel(0); //Clean up the label
	}
}

void OverlayHub::onAlarm(unsigned long long uid,
		unsigned long long ticks) noexcept {
	tokens.fill(DEF_TOKENS_COUNT);
}

void OverlayHub::onInotification(unsigned long long uid,
		const InotifyEvent *event) noexcept {
	if (event->wd == -1) { //overflow notification
		return;
	}

	for (unsigned int i = 0; i < WATCHLIST_SIZE; ++i) {
		if (watchlist[i].identifier == event->wd) {
			watchlist[i].events |= event->mask;
			refresh(i);
			break;
		}
	}
}

bool OverlayHub::doable() const noexcept {
	return isSuperNode();
}

void OverlayHub::act(void *arg) noexcept {
	//Execute the stabilization loop
	stabilizer.periodic();
}

void OverlayHub::cease() noexcept {
	/*
	 * Just in case the worker thread failed to start and hence did
	 * not perform a cleanup on exit. No race condition because the
	 * worker thread has already exited.
	 */
	stabilizer.cleanup();
}

void OverlayHub::installService() {
	if (!doable()) {
		return;
	}

	int fd = -1;
	auto w = connect(fd, true, ctx.timeout);
	worker.id = w->getUid(); //set here
	onboard(w);
	stabilizer.configure(fd, ctx.nodes, ctx.period, ctx.pause);
}

void OverlayHub::installTracker() {
	try {
		//Events we are interested in: modify-> close
		const uint32_t events = IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE;
		if (auto path = getPath(Identity::CTX_OPTIONS); path) {
			watchlist[0].identifier = track(path, events);
			watchlist[0].context = Identity::CTX_OPTIONS;
		}

		if (auto path = getPath(Identity::CTX_HOSTS_DB); path) {
			watchlist[1].identifier = track(path, events);
			watchlist[1].context = Identity::CTX_HOSTS_DB;
		}

		if (auto path = getPath(Identity::CTX_HOSTS_FILE); path) {
			watchlist[2].identifier = track(path, events);
			watchlist[2].context = Identity::CTX_HOSTS_FILE;
		}

		if (auto path = getPath(Identity::CTX_PKI_PRIVATE); path) {
			watchlist[3].identifier = track(path, events);
			watchlist[3].context = Identity::CTX_PKI_PRIVATE;
		}

		if (auto path = getPath(Identity::CTX_PKI_PUBLIC); path) {
			watchlist[4].identifier = track(path, events);
			watchlist[4].context = Identity::CTX_PKI_PUBLIC;
		}

		if (auto path = getPath(Identity::CTX_SSL_ROOT); path) {
			watchlist[5].identifier = track(path, events);
			watchlist[5].context = Identity::CTX_SSL_ROOT;
		}

		if (auto path = getPath(Identity::CTX_SSL_CERT); path) {
			watchlist[6].identifier = track(path, events);
			watchlist[6].context = Identity::CTX_SSL_CERT;
		}

		if (auto path = getPath(Identity::CTX_SSL_PRIVATE); path) {
			watchlist[7].identifier = track(path, events);
			watchlist[7].context = Identity::CTX_SSL_PRIVATE;
		}

	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void OverlayHub::refresh(unsigned int context) noexcept {
	//REF: https://github.com/guard/guard/wiki/Analysis-of-inotify-events-for-different-editors
	if (watchlist[context].events & IN_IGNORED) {
		//Associated file will no longer be monitored
		watchlist[context].identifier = -1;
		watchlist[context].events = 0;
	} else if (!(watchlist[context].events & IN_CLOSE_WRITE)) { //Write-close
		return;
	} else if (!(watchlist[context].events & (IN_MODIFY | IN_ATTRIB))) { //Modification
		//Closed without modification
		watchlist[context].events = 0;
		return;
	} else {
		//Reset for next cycle
		watchlist[context].events = 0;
	}
	//-----------------------------------------------------------------
	/*
	 * Reload the settings
	 */
	try {
		switch (watchlist[context].context) {
		case Identity::CTX_OPTIONS:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG(
						"Configuration file has been modified (restart required)");
			} else {
				WH_LOG_DEBUG("Configuration file has been ignored");
			}
			break;
		case Identity::CTX_HOSTS_DB:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG("Hosts database has been modified");
			} else {
				WH_LOG_DEBUG("Hosts database has been ignored");
			}
			break;
		case Identity::CTX_HOSTS_FILE:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG("Hosts file has been modified");
				Identity::refresh(Identity::CTX_HOSTS_FILE);
			} else {
				WH_LOG_DEBUG("Hosts file has been ignored");
			}
			break;
		case Identity::CTX_PKI_PRIVATE:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG("Private key file has been modified");
				Identity::refresh(Identity::CTX_PKI_PRIVATE);
			} else {
				WH_LOG_DEBUG("Private key file has been ignored");
			}
			break;
		case Identity::CTX_PKI_PUBLIC:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG("Public key file has been modified");
				Identity::refresh(Identity::CTX_PKI_PUBLIC);
			} else {
				WH_LOG_DEBUG("Public key file has been ignored");
			}
			break;
		case Identity::CTX_SSL_ROOT:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG(
						"SSL trusted certificate has been modified (restart required)");
			} else {
				WH_LOG_DEBUG("SSL trusted certificate has been ignored");
			}
			break;
		case Identity::CTX_SSL_CERT:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG("SSL certificate has been modified");
				Identity::refresh(Identity::CTX_SSL_CERT);
			} else {
				WH_LOG_DEBUG("SSL certificate has been ignored");
			}
			break;
		case Identity::CTX_SSL_PRIVATE:
			if (watchlist[context].identifier != -1) {
				WH_LOG_DEBUG("SSL host key has been modified");
				Identity::refresh(Identity::CTX_SSL_PRIVATE);
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

bool OverlayHub::converge() noexcept {
	if (isStable()) {
		return true;
	}

	setStable(true);

	//First fix the connection to controller
	if (!bridge(CONTROLLER, &sessions[TABLESIZE])) {
		return false;
	}

	//Fall through the routing table and fix errors
	for (unsigned int i = 0; i < TABLESIZE; i++) {
		if (!isConsistent(i)) {
			auto old = commit(i);
			if (!isInRoute(old)) {
				auto conn = find(old);
				//Take care of the reference asymmetry
				if (conn && conn->isType(SOCKET_PROXY)) {
					disable(conn);
				}
			}
		}

		//Update the connection status
		setConnected(i, bridge(get(i), &sessions[i]));
	}

	//If the predecessor has changed, certain connections need to be removed
	if (predessorChanged()) {
		commitPredecessor();
		reap(PURGE_INVALID);
	}

	return true;
}

bool OverlayHub::bridge(unsigned long long id, Digest *hc) noexcept {
	try {
		return connect(id, hc);
	} catch (const BaseException &e) {
		if (!Socket::unallocated() || !Message::unallocated()) {
			reap(PURGE_CLIENT, 2);
			setStable(false);
		}
		return false;
	}
}

void OverlayHub::onboard(Watcher *w) noexcept {
	auto id = w->getUid();
	if (!isSuperNode()) {
		return;
	} else if (isController(id) || isWorker(id)) {
		w->setFlags(SOCKET_PRIORITY);
		w->setOption(WATCHER_OUTBOUND_MAX, 0); //default
	} else if (isInternal(id)) {
		w->setFlags(SOCKET_OVERLAY);
		w->setOption(WATCHER_OUTBOUND_MAX, 0); //default
		Node::update(id, true);
	} else {
		return;
	}
}

void OverlayHub::offboard(Watcher *w) noexcept {
	//If the worker connection failed then initiate shutdown
	if (isWorker(w->getUid())) {
		Hub::cancel();
	}

	//Remove from the routing table
	if (isInternal(w->getUid())) {
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

void OverlayHub::memorize(unsigned long long id) noexcept {
	if (id && isInternal(id) && !isHost(id)) {
		nodes.cache[nodes.index] = id;
		nodes.index = (nodes.index + 1) & (NODECACHE_SIZE - 1);
	}
}

int OverlayHub::enroll(const Message *request) noexcept {
	if (request->getOrigin() != request->getSource()) {
		//Origin must match the source identifier (only direct requests)
		return -1;
	}

	auto current = request->getSource();
	auto requested = request->getDestination();
	int mode = -1; //default: reject
	if (request->getStatus() == WH_DHT_AQLF_ACCEPTED) {
		mode = enroll(current, requested);
	}

	if (mode == -1) {
		detach(current);
		return -1;
	}

	auto conn = move(current, requested, (mode == 2) ? true : false);
	if (!conn) {
		return -1;
	} else {
		conn->setGroup(request->getSession());
		onboard(conn);
		return (mode == 0) ? 1 : 0;
	}
}

int OverlayHub::enroll(unsigned long long source,
		unsigned long long request) noexcept {
	if (!ctx.enroll) {
		return -1;
	} else if (isHost(request) || isWorker(request)) {
		return -1;
	} else if (source == request) {
		//Just activate
		return 0;
	} else if (isInternal(request)) {
		//Precedence rule if both sides are trying to connect
		return ((request < getUid()) ? 1 : 2);
	} else if (isLocal(mapKey(request))) {
		//Replace existing connection on conflict
		return !(isSuperNode() && (Socket::unallocated() <= TABLESIZE)) ? 2 : -1;
	} else {
		return -1;
	}
}

bool OverlayHub::authenticate(const Message *request) noexcept {
	/*
	 * 1. Confirm that the requested ID is valid
	 * 2. Analyze the security features
	 * 3. Impose rate limit
	 */
	auto origin = request->getOrigin();
	auto requested = request->getSource();

	if (!validate(origin, requested)) {
		//CASE 1
		return false;
	} else if (!ctx.authenticate && !isInternal(requested)) {
		//CASE 2
		return true;
	} else if (!getPKI()) {
		//CASE 2
		return true;
	} else if (request->getPayloadLength()
			== Hash::SIZE + PKI::SIGNATURE_LENGTH) {
		//CASE 2 & 3
		return tokens.take()
				&& verifyNonce(hash, origin, getUid(),
						(Digest*) request->getBytes(0))
				&& request->verify(getPKI());
	} else {
		return false;
	}
}

bool OverlayHub::validate(unsigned long long source,
		unsigned long long request) const noexcept {
	/*
	 * 1. Registration should be enabled
	 * 2. Only fresh request and the requested ID must be an Active ID
	 * 3. Requested ID cannot be one of the Host/Controller/Worker IDs
	 * 4. Requested client ID must be "local"
	 */
	if (!ctx.enroll) {
		//CASE 1
		return false;
	} else if (!isEphemeral(source) || isEphemeral(request)) {
		//CASE 2
		return false;
	} else if (isHost(request) || isController(request) || isWorker(request)) {
		//CASE 3
		return false;
	} else if (isExternal(request) && !isLocal(mapKey(request))) {
		//CASE 4
		return false;
	} else {
		return true;
	}
}

bool OverlayHub::intercept(Message *message) noexcept {
	if (message->getCommand() != WH_DHT_CMD_BASIC) {
		return false;
	} else if (message->getQualifier() == WH_DHT_QLF_REGISTER) {
		handleRegistrationRequest(message);
		return true;
	} else if (message->getQualifier() == WH_DHT_QLF_TOKEN) {
		handleTokenRequest(message);
		return true;
	} else {
		return false;
	}
}

void OverlayHub::annotate(Message *message) noexcept {
	if (isWorker(message->getOrigin())) {
		message->putLabel(getWorker() + getUid());
		message->getHeader(worker.header);
	} else if (isExternal(message->getOrigin())) {
		//Preserve the group ID at insertion
		message->writeLabel(message->getGroup());
		//Assign the correct source ID
		message->putSource(message->getOrigin());
	} else if (isExternal(message->getSource())) {
		//Retrieve the group ID during routing
		message->setGroup(message->getLabel());
	} else {
		memorize(message->getSource());
	}
}

bool OverlayHub::plot(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto destination = message->getDestination();
	if (isWorker(origin)) {
		if (!isHost(destination)) {
			//Stabilization request sent via controller
			message->setDestination(CONTROLLER);
		}
	} else if (isController(origin)) {
		if (corroborate(message)) {
			//Stabilization response returned via controller
			message->setDestination(getWorker());
		}
	} else if (approve(origin, destination)) {
		message->setDestination(gateway(destination));
	} else {
		//Highly likely a miscommunication
		if (!(isHost(destination) || isController(destination))) {
			message->setFlags(MSG_INVALID);
		}
		message->setDestination(getUid());
	}

	return true;
}

bool OverlayHub::corroborate(const Message *response) const noexcept {
	auto &sh = worker.header;
	return response->getStatus() != WH_DHT_AQLF_REQUEST
			&& response->getLabel() == sh.getLabel()
			&& isHost(response->getDestination())
			&& response->getSequenceNumber() == sh.getSequenceNumber()
			&& response->getSession() == sh.getSession()
			&& response->getCommand() == sh.getCommand()
			&& response->getQualifier() == sh.getQualifier();
}

unsigned long long OverlayHub::gateway(unsigned long long to) const noexcept {
	/*
	 * CASE 1: destination is "local" or "controller"
	 * In such case do nothing and allow the server take care of it
	 * That is always the case with a stand-alone server
	 *
	 * CASE 2: Destination lies somewhere else on the network
	 * FIND THE NEXT HOP
	 */
	auto k = mapKey(to);
	if (!isLocal(k) && !isController(to)) {
		//Case 2
		return nextHop(k);
	} else {
		return to;
	}
}

bool OverlayHub::approve(unsigned long long from,
		unsigned long long to) const noexcept {
	/*
	 * 1. Both Source and Destination must be active IDs
	 * 2. Destination cannot be controller or worker IDs
	 * 3. Allow client -> supernode communication only via controller
	 * 4. Apply netmask over all client -> * communications
	 */
	auto checkActive = !isEphemeral(from) && !isEphemeral(to);
	auto checkDestinations = !(isController(to) || isWorker(to));
	auto checkPrivilege = isController(getUid())
			|| !(isExternal(from) && isInternal(to));

	return checkActive && checkDestinations && checkPrivilege
			&& permit(from, to);
}

bool OverlayHub::permit(unsigned long long from,
		unsigned long long to) const noexcept {
	return isInternal(from) || ((from & ctx.netmask) == (to & ctx.netmask));
}

bool OverlayHub::serve(Message *request) noexcept {
	switch (request->getCommand()) {
	case WH_DHT_CMD_NULL:
		return serveNullRequest(request);
	case WH_DHT_CMD_BASIC:
		return serveBasicRequest(request);
	case WH_DHT_CMD_MULTICAST:
		return serveMulticastRequest(request);
	case WH_DHT_CMD_NODE:
		return serveNodeRequest(request);
	case WH_DHT_CMD_OVERLAY:
		return serveOverlayRequest(request);
	default:
		return handleInvalidRequest(request);
	}
}

bool OverlayHub::serveNullRequest(Message *request) noexcept {
	if (request->getCommand() != WH_DHT_CMD_NULL) {
		return handleInvalidRequest(request);
	}

	if (!isPrivileged(request->getOrigin())
			|| request->getStatus() != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(request);
	}

	switch (request->getQualifier()) {
	case WH_DHT_QLF_DESCRIBE:
		return handleDescribeNodeRequest(request);
	default:
		return handleInvalidRequest(request);
	}
}

bool OverlayHub::serveBasicRequest(Message *request) noexcept {
	if (request->getCommand() != WH_DHT_CMD_BASIC) {
		return handleFindRootRequest(request);
	}

	switch (request->getQualifier()) {
	case WH_DHT_QLF_FINDROOT:
		return handleFindRootRequest(request);
	case WH_DHT_QLF_BOOTSTRAP:
		if (request->getStatus() == WH_DHT_AQLF_REQUEST) {
			return handleBootstrapRequest(request);
		} else {
			return handleInvalidRequest(request);
		}
	default:
		return handleInvalidRequest(request);
	}
}

bool OverlayHub::serveMulticastRequest(Message *request) noexcept {
	if (request->getCommand() != WH_DHT_CMD_MULTICAST) {
		return handleInvalidRequest(request);
	}

	if (isSuperNode() || isInternal(request->getOrigin())
			|| isEphemeral(request->getOrigin())
			|| request->getStatus() != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(request);
	}

	switch (request->getQualifier()) {
	case WH_DHT_QLF_PUBLISH:
		return handlePublishRequest(request);
	case WH_DHT_QLF_SUBSCRIBE:
		return handleSubscribeRequest(request);
	case WH_DHT_QLF_UNSUBSCRIBE:
		return handleUnsubscribeRequest(request);
	default:
		return handleInvalidRequest(request);
	}
}

bool OverlayHub::serveNodeRequest(Message *request) noexcept {
	if (request->getCommand() != WH_DHT_CMD_NODE) {
		return handleInvalidRequest(request);
	}

	if (!(isController(request->getOrigin()) || isWorker(request->getOrigin()))
			|| request->getStatus() != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(request);
	}

	switch (request->getQualifier()) {
	case WH_DHT_QLF_GETPREDECESSOR:
		return handleGetPredecessorRequest(request);
	case WH_DHT_QLF_SETPREDECESSOR:
		return handleSetPredecessorRequest(request);
	case WH_DHT_QLF_GETSUCCESSOR:
		return handleGetSuccessorRequest(request);
	case WH_DHT_QLF_SETSUCCESSOR:
		return handleSetSuccessorRequest(request);
	case WH_DHT_QLF_GETFINGER:
		return handleGetFingerRequest(request);
	case WH_DHT_QLF_SETFINGER:
		return handleSetFingerRequest(request);
	case WH_DHT_QLF_GETNEIGHBOURS:
		return handleGetNeighboursRequest(request);
	case WH_DHT_QLF_NOTIFY:
		return handleNotifyRequest(request);
	default:
		return handleInvalidRequest(request);
	}
}

bool OverlayHub::serveOverlayRequest(Message *request) noexcept {
	if (request->getCommand() != WH_DHT_CMD_OVERLAY) {
		return handleInvalidRequest(request);
	}

	if (!isPrivileged(request->getOrigin())
			|| request->getStatus() != WH_DHT_AQLF_REQUEST) {
		return handleInvalidRequest(request);
	}

	switch (request->getQualifier()) {
	case WH_DHT_QLF_FINDSUCCESSOR:
		return handleFindSuccesssorRequest(request);
	case WH_DHT_QLF_PING:
		return handlePingNodeRequest(request);
	case WH_DHT_QLF_MAP:
		return handleMapRequest(request);
	default:
		return handleInvalidRequest(request);
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

	OverlayHubInfo info;
	metrics(info);
	auto index = info.pack(msg->payload(), Message::PAYLOAD_SIZE);
	//-----------------------------------------------------------------
	buildDirectResponse(msg, Message::HEADER_SIZE + index);
	msg->putStatus(index ? WH_DHT_AQLF_ACCEPTED : WH_DHT_AQLF_REJECTED);
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
	msg->setFlags(MSG_PROBE);
	//-----------------------------------------------------------------
	/*
	 * [PROXY ESTABLISHMENT]
	 * If a proxy connection with a remote node is in progress, we might have
	 * received an accepted/rejected message.
	 */
	if (msg->isType(SOCKET_PROXY) && msg->getStatus() != WH_DHT_AQLF_REQUEST) {
		msg->setDestination(origin);
		//Set correct source identifier
		msg->setSource(origin);
		return true;
	}
	//-----------------------------------------------------------------
	/*
	 * Treat all the other cases as a registration request
	 */
	//Do this before the message is modified
	auto success = authenticate(msg);
	//Set correct source identifier
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

bool OverlayHub::handleTokenRequest(Message *msg) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=1, QLF=1, AQLF=0/1/127
	 * BODY: 512/8=64 Bytes in Request (optional), (512/8)*2=128 Bytes in Response
	 * TOTAL: 32+64=96 bytes in Request; 32+128=160 bytes in Response
	 */
	auto origin = msg->getOrigin();
	//-----------------------------------------------------------------
	/*
	 * [PROXY ESTABLISHMENT]
	 * If a proxy connection with a remote node is in progress, we might have
	 * received a session key. Send back a registration request to complete
	 * the process.
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
	 * TODO: This is an EXPERIMENTAL FEATURE.
	 * Session key request misuse prevention.
	 */
	if (msg->testTrace(SESSION_TRACE)) {
		return handleInvalidRequest(msg);
	}
	msg->setTrace(SESSION_TRACE);
	//-----------------------------------------------------------------
	/*
	 * This call succeeds if the caller is a temporary connection and the
	 * message is of proper size, otherwise a failure message is sent back.
	 */
	if (isEphemeral(origin) && msg->getPayloadLength() <= Hash::SIZE) {
		Digest hc;	//Challenge Key
		memset(&hc, 0, sizeof(hc));
		generateNonce(hash, origin, getUid(), &hc);
		msg->appendBytes((const unsigned char*) &hc, Hash::SIZE);
		msg->writeSource(0);
		msg->writeDestination(0);
		msg->setDestination(origin);
		msg->putStatus(WH_DHT_AQLF_ACCEPTED);
	} else if (isEphemeral(origin)
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
		if (isInternal(origin)
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
		if (isExternal(origin) || isController(getUid())
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
			return plot(msg);
		}
	} else {
		if (isExternal(origin) || isController(origin)) {
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
		msg->writeDestination(isExternal(origin) ? 0 : source);
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
				&& permit(msg->getOrigin(), sub->getUid())
				&& !sub->testGroup(msg->getGroup()) && sub->publish(msg)
				&& sub->isReady()) {
			retain(sub);
		}
		++index;
	}

	msg->writeLabel(0); //Clean up internal information
	msg->writeDestination(0); //There are multiple destinations
	msg->writeStatus(WH_DHT_AQLF_ACCEPTED); //Prevent rebound
	msg->link(); //Account for Hub::publish
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

	buildDirectResponse(msg, Message::HEADER_SIZE);
	msg->writeSource(0); //Obfuscate the source (this hub)

	auto topic = msg->getSession();
	auto conn = find(msg->getOrigin());

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
	auto conn = find(msg->getOrigin());

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
	if (!(isController(origin) || isWorker(origin))) {
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
	if (isWorker(origin)) {
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
	 * Message insertion: at this point we are sure that this node is the
	 * intended recipient.
	 */
	if (isExternal(origin)) {
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
	} else if (isExternal(source) || origin != getPredecessor()
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
			&& (isHost(source) || !isInRange(source, getUid(), successor))
			&& getUid() != successor) {
		/*
		 * Forward the Message around the identifier ring until it reaches the
		 * predecessor of the node where the message was originally inserted (or
		 * a hole).
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

unsigned long long OverlayHub::getWorker() const noexcept {
	return worker.id;
}

bool OverlayHub::isWorker(unsigned long long uid) const noexcept {
	return (uid == worker.id) && !isHost(uid);
}

bool OverlayHub::isPrivileged(unsigned long long uid) const noexcept {
	return isInternal(uid) || isWorker(uid);
}

bool OverlayHub::isSuperNode() const noexcept {
	return (ctx.join && !isController(getUid()));
}

bool OverlayHub::isHost(unsigned long long uid) const noexcept {
	return uid == getUid();
}

bool OverlayHub::isController(unsigned long long uid) noexcept {
	return uid == CONTROLLER;
}

bool OverlayHub::isInternal(unsigned long long uid) noexcept {
	return uid <= MAX_ID;
}

bool OverlayHub::isExternal(unsigned long long uid) noexcept {
	return uid > MAX_ID;
}

bool OverlayHub::isEphemeral(unsigned long long uid) noexcept {
	return uid > Socket::MAX_ACTIVE_ID;
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
		attach(local, IO_WR, WATCHER_ACTIVE);
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
	auto conn = find(id);
	if (!conn) {
		WH_LOG_DEBUG("Connecting to %llu", id);
		createProxyConnection(id, hc);
		//Not registered yet
		return nullptr;
	} else if (conn->testFlags(WATCHER_ACTIVE)) {
		//Registration completed
		return conn;
	} else if (conn->hasTimedOut(ctx.timeout)) {
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
			throw Exception(EX_ARGUMENT);
		}

		NameInfo ni;
		Identity::getAddress(id, ni);
		conn = new Socket(ni);
		//-----------------------------------------------------------------
		//A session key request is automatically sent out
		generateNonce(hash, conn->getUid(), getUid(), hc);
		auto msg = Protocol::createTokenRequest( { 0, id },
				{ verifyHost() ? getPKI() : nullptr, hc }, nullptr);
		if (!msg) {
			throw Exception(EX_MEMORY);
		}
		conn->publish(msg);
		conn->setUid(id);
		attach(conn, IO_WR, 0);
		return conn;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete conn;
		throw;
	}
}

unsigned int OverlayHub::reap(int mode, unsigned int target) noexcept {
	PurgeControl pc { target, 0, this };
	switch (mode) {
	case PURGE_TEMPORARY:
		return Hub::reap(target);
	case PURGE_INVALID:
		iterate(reapInvalid, &pc);
		return pc.count;
	default:
		iterate(reapClient, &pc);
		return pc.count;
	}
}

int OverlayHub::reapInvalid(Watcher *w, void *arg) noexcept {
	auto uid = w->getUid();
	auto pc = static_cast<PurgeControl*>(arg);
	auto hub = pc->hub;
	if (pc->target && pc->count >= pc->target) {
		return -1;
	} else if (hub->isInternal(uid) || hub->isWorker(uid)) {
		return 0;
	} else if (isEphemeral(uid) || hub->isLocal(mapKey(uid))) {
		return 0;
	} else {
		hub->disable(w);
		pc->count++;
		return 0;
	}
}

int OverlayHub::reapClient(Watcher *w, void *arg) noexcept {
	auto uid = w->getUid();
	auto pc = static_cast<PurgeControl*>(arg);
	auto hub = pc->hub;
	if (pc->target && pc->count >= pc->target) {
		return -1;
	} else if (hub->isInternal(uid) || hub->isWorker(uid)) {
		return 0;
	} else if (isEphemeral(uid) && w->testFlags(WATCHER_ACTIVE)) {
		return 0;
	} else {
		hub->disable(w);
		pc->count++;
		return 0;
	}
}

void OverlayHub::clear() noexcept {
	worker.header.clear();
	worker.id = getUid();

	memset(&ctx, 0, sizeof(ctx));
	memset(&nodes, 0, sizeof(nodes));
	memset(sessions, 0, sizeof(sessions));

	for (unsigned int i = 0; i < WATCHLIST_SIZE; ++i) {
		watchlist[i].context = -1;
		watchlist[i].identifier = -1;
		watchlist[i].events = 0;
	}

	topics.clear();
}

void OverlayHub::metrics(OverlayHubInfo &info) const noexcept {
	Hub::metrics(info);
	info.setPredecessor(getPredecessor());
	info.setSuccessor(getSuccessor());
	info.setRoutes(TABLESIZE);
	info.setStable(isStable());
	for (unsigned int i = 0; i < TABLESIZE; ++i) {
		auto f = getFinger(i);
		RouteInfo ri = { f->getStart(), f->getId(), f->getOldId(),
				f->isConnected() };
		info.setRoute(ri, i);
	}
}

} /* namespace wanhive */
