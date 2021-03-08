/*
 * ClientHub.cpp
 *
 * The base class for the Wanhive clients
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "ClientHub.h"
#include "Protocol.h"
#include "../base/Logger.h"
#include "../util/commands.h"
#include "../util/Random.h"

namespace {

enum {
	WHC_IDENTIFY,
	WHC_AUTHENTICATE,
	WHC_BOOTSTRAP,
	WHC_ROOT,
	WHC_GETKEY,
	WHC_AUTHORIZE,
	WHC_ERROR,
	WHC_REGISTERED,
	WHC_FATAL
};

}  // namespace

namespace wanhive {

ClientHub::ClientHub(unsigned long long uid, const char *path) noexcept :
		Hub(uid, path) {
	clear();
}

ClientHub::~ClientHub() {

}

bool ClientHub::isConnected() const noexcept {
	return connected;
}

void ClientHub::stop(Watcher *w) noexcept {
	if (w == bs.auth) {
		bs.auth = nullptr;
	} else if (w == bs.node) {
		bs.node = nullptr;
		connected = false;
	}

	Hub::stop(w);
}

void ClientHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		decltype(auto) conf = Identity::getConfiguration();
		ctx.password = (const unsigned char*) conf.getString("CLIENT",
				"password");
		if (ctx.password) {
			ctx.passwordLength = strlen((const char*) ctx.password);
		} else {
			ctx.passwordLength = 0;
		}
		ctx.passwordHashRounds = conf.getNumber("CLIENT", "passwordHashRounds");
		ctx.timeOut = conf.getNumber("CLIENT", "timeOut", 2000);
		ctx.retryInterval = conf.getNumber("CLIENT", "retryInterval", 5000);

		WH_LOG_DEBUG(
				"Client hub settings:\nPASSWORD=\"%s\", HASHROUNDS=%u, TIMEOUT=%ums, RETRYINTERVAL=%ums\n",
				ctx.password, ctx.passwordHashRounds, ctx.timeOut,
				ctx.retryInterval);
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::cleanup() noexcept {
	clear();
	Hub::cleanup();
}

void ClientHub::route(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto source = message->getSource();
	auto command = message->getCommand();
	auto qualifier = message->getQualifier();
	//Prevents replay (UID is the sink)
	message->setDestination(getUid());

	switch (command) {
	case WH_CMD_NULL:
		if (isStage(WHC_ERROR) || isStage(WHC_REGISTERED) || isStage(WHC_FATAL)
				|| !bs.auth) {
			//Bad message
		} else if (origin != bs.auth->getUid()) {
			//Bad message
		} else if (!(source == 0 || source == getUid())) {
			//Bad message
		} else if (qualifier == WH_QLF_IDENTIFY) {
			processIdentificationResponse(message);
		} else if (qualifier == WH_QLF_AUTHENTICATE) {
			processAuthenticationResponse(message);
		} else {
			//Unsupported message
		}
		break;
	case WH_CMD_BASIC:
		if (isStage(WHC_ERROR) || isStage(WHC_REGISTERED) || isStage(WHC_FATAL)
				|| !bs.node || (ctx.password && !bs.auth)) {
			//Bad message
		} else if (!(origin == bs.node->getUid()
				|| (bs.auth && origin == bs.auth->getUid()))) {
			//Bad message
		} else if (!(source == 0 || source == getUid())) {
			//Bad message
		} else if (qualifier == WH_QLF_REGISTER) {
			processRegistrationResponse(message);
		} else if (qualifier == WH_QLF_GETKEY) {
			processGetKeyResponse(message);
		} else if (qualifier == WH_QLF_FINDROOT) {
			processFindRootResponse(message);
		} else {
			//Unsupported message
		}
		break;
	default:
		//Unsupported message
		break;
	}
}

void ClientHub::maintain() noexcept {
	switch (getStage()) {
	case WHC_IDENTIFY:
		connectToAuthenticator();
		break;
	case WHC_AUTHENTICATE:
		if (checkStageTimeout(ctx.timeOut)) {
			setStage(WHC_ERROR);
		}
		break;
	case WHC_BOOTSTRAP:
		connectToOverlay();
		break;
	case WHC_ROOT:
	case WHC_GETKEY:
	case WHC_AUTHORIZE:
		if (checkStageTimeout(ctx.timeOut)) {
			setStage(WHC_ERROR);
		}
		break;
	case WHC_ERROR:
		if (checkStageTimeout(ctx.retryInterval)) {
			setStage(WHC_IDENTIFY);
		}
		break;
	case WHC_REGISTERED:
		if (!bs.node) {
			setStage(WHC_ERROR);
		}
		break;
	default:
		Hub::cancel();
		break;
	}
}

void ClientHub::setPassword(const unsigned char *password, unsigned int length,
		unsigned int rounds) noexcept {
	ctx.password = password;
	ctx.passwordLength = length;
	ctx.passwordHashRounds = rounds;
}

unsigned short ClientHub::nextSequenceNumber() noexcept {
	if (!bs.sn) {
		bs.sn = 1;
	}
	return bs.sn++;
}

void ClientHub::connectToAuthenticator() noexcept {
	Socket *s = nullptr;
	try {
		//-----------------------------------------------------------------
		//Check for consistency
		if (!isStage(WHC_IDENTIFY)) {
			throw Exception(EX_INVALIDSTATE);
		}

		if (!ctx.password) {
			WH_LOG_DEBUG("Using PKI for authentication");
			setStage(WHC_BOOTSTRAP);
			return;
		}
		//-----------------------------------------------------------------
		//Check for timeout on the existing authentication node
		if (bs.auth) {
			if (bs.auth->hasTimedOut(ctx.timeOut)) {
				WH_LOG_DEBUG("Connection timed out");
				disable(bs.auth);
			}
			return;
		}
		//-----------------------------------------------------------------
		//Load the list of identifiers if necessary
		if (!bs.identifiers.hasSpace()) {
			loadIdentifiers(true);
		}
		//-----------------------------------------------------------------
		//Get the next identifier to probe
		unsigned long long id;
		if (!bs.identifiers.get(id)) {
			WH_LOG_DEBUG("Identifiers list is empty");
			setStage(WHC_ERROR);
			throw Exception(EX_RESOURCE);
		}
		//-----------------------------------------------------------------
		//Establish new connection
		NameInfo ni;
		Identity::getAddress(id, ni);
		s = new Socket(ni);
		s->setUid(id);
		s->publish(createIdentificationRequest());
		putWatcher(s, IO_WR, WATCHER_ACTIVE);
		bs.auth = s;
		WH_LOG_DEBUG("Contacting authentication node %llu", bs.auth->getUid());
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete s;
	}
}

void ClientHub::connectToOverlay() noexcept {
	Socket *s = nullptr;
	try {
		//-----------------------------------------------------------------
		//Check for consistency
		if (!isStage(WHC_BOOTSTRAP)) {
			throw Exception(EX_INVALIDSTATE);
		}
		//-----------------------------------------------------------------
		//Check for timeout on the existing authentication node
		if (bs.node) {
			if (bs.node->hasTimedOut(ctx.timeOut)) {
				WH_LOG_DEBUG("Connection timed out");
				disable(bs.node);
			}
			return;
		}
		//-----------------------------------------------------------------
		//Load the list of identifiers if necessary
		if (!bs.identifiers.hasSpace()) {
			loadIdentifiers(false);
		}
		//-----------------------------------------------------------------
		//Get the next identifier to probe
		unsigned long long id;
		if (!bs.identifiers.get(id)) {
			WH_LOG_DEBUG("Identifiers list is empty");
			setStage(WHC_ERROR);
			throw Exception(EX_RESOURCE);
		}
		//-----------------------------------------------------------------
		//Establish new connection
		NameInfo ni;
		Identity::getAddress(id, ni);
		s = new Socket(ni);
		s->setUid(id);
		s->publish(createFindRootRequest());
		putWatcher(s, IO_WR, WATCHER_ACTIVE);
		bs.node = s;
		WH_LOG_DEBUG("Contacting bootstrap node %llu", bs.node->getUid());
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete s;
	}
}

bool ClientHub::checkStageTimeout(unsigned int milliseconds) const noexcept {
	return bs.timer.hasTimedOut(milliseconds);
}

void ClientHub::initAuthentication() noexcept {
	try {
		if (!isStage(WHC_AUTHENTICATE) || !bs.auth) {
			throw Exception(EX_INVALIDSTATE);
		} else {
			//Message destination is correctly set
			Hub::sendMessage(createAuthenticationRequest());
			WH_LOG_DEBUG("Authentication initiated");
		}

	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		setStage(WHC_ERROR);
	}
}

void ClientHub::findRoot() noexcept {
	Socket *s = nullptr;
	bool fresh = false;
	try {
		if (!isStage(WHC_ROOT) || !bs.root || !bs.node) {
			throw Exception(EX_INVALIDSTATE);
		} else if (bs.root == bs.node->getUid()) {
			fresh = false;
			WH_LOG_DEBUG("Found the root node [%llu]", bs.root);
		} else {
			fresh = true;
			NameInfo ni;
			Identity::getAddress(bs.root, ni);
			s = new Socket(ni);
			s->setUid(bs.root);
			WH_LOG_DEBUG("Connecting with the root node [%llu]", bs.root);
		}
		//-----------------------------------------------------------------
		uint64_t rnd[2];
		Random prng;
		prng.bytes(rnd, sizeof(rnd));
		generateNonce(bs.hashFn, rnd[0], rnd[1], &bs.nonce);
		auto msg = Protocol::createGetKeyRequest(0,
				{ verifyHost() ? getPKI() : nullptr, &bs.nonce }, nullptr);
		//-----------------------------------------------------------------
		if (!msg) {
			throw Exception(EX_ALLOCFAILED);
		} else if (fresh) {
			s->publish(msg);
			putWatcher(s, IO_WR, WATCHER_ACTIVE);
			//Swap and disable
			auto w = bs.node;
			bs.node = s;
			disable(w);
		} else {
			msg->setDestination(bs.node->getUid());
			Hub::sendMessage(msg);
		}
		//-----------------------------------------------------------------
		setStage(WHC_GETKEY);
		WH_LOG_DEBUG("Requesting session key");
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete s;
		setStage(WHC_ERROR);
	}
}

void ClientHub::initAuthorization() noexcept {
	try {
		if (!isStage(WHC_AUTHORIZE) || !bs.node || (ctx.password && !bs.auth)) {
			throw Exception(EX_INVALIDSTATE);
		} else if (bs.auth) {
			auto msg = createRegistrationMessage(false);
			msg->setDestination(bs.auth->getUid());
			Hub::sendMessage(msg);
			WH_LOG_DEBUG("Initiating authorization");
		} else {
			auto msg = createRegistrationMessage(true);
			msg->setDestination(bs.node->getUid());
			Hub::sendMessage(msg);
			WH_LOG_DEBUG("Initiating registration");
		}
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		setStage(WHC_ERROR);
	}
}

Message* ClientHub::createIdentificationRequest() {
	try {
		const unsigned char *binary = nullptr;
		unsigned int bytes = 0;
		Message *msg = nullptr;
		if (!bs.authenticator.generateNonce(binary, bytes)) {
			throw Exception(EX_SECURITY);
		} else if (!(msg = Protocol::createIdentificationRequest(0, getUid(), 0,
				binary, bytes))) {
			throw Exception(EX_ALLOCFAILED);
		} else {
			return msg;
		}
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::processIdentificationResponse(Message *msg) noexcept {
	unsigned int saltLength = 0;
	unsigned int nonceLength = 0;
	const unsigned char *salt = nullptr;
	const unsigned char *nonce = nullptr;
	if (!isStage(WHC_IDENTIFY)) {
		setStage(WHC_ERROR);
	} else if (!bs.auth || msg->getOrigin() != bs.auth->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processIdentificationResponse(msg, saltLength, salt,
			nonceLength, nonce)) {
		setStage(WHC_ERROR);
	} else {
		auto f = bs.authenticator.createIdentity(getUid(), ctx.password,
				ctx.passwordLength, salt, saltLength, nonce, nonceLength,
				ctx.passwordHashRounds);
		if (!f) {
			setStage(WHC_ERROR);
		} else {
			WH_LOG_DEBUG("Identification succeeded");
			setStage(WHC_AUTHENTICATE);
			initAuthentication();
		}
	}
}

Message* ClientHub::createAuthenticationRequest() {
	try {
		const unsigned char *binary = nullptr;
		unsigned int bytes = 0;
		Message *msg = nullptr;
		if (!bs.auth) {
			throw Exception(EX_INVALIDOPERATION);
		} else if (!bs.authenticator.generateUserProof(binary, bytes)) {
			throw Exception(EX_INVALIDSTATE);
		} else if (!(msg = Protocol::createAuthenticationRequest(
				bs.auth->getUid(), 0, binary, bytes))) {
			throw Exception(EX_ALLOCFAILED);
		} else {
			return msg;
		}
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::processAuthenticationResponse(Message *msg) noexcept {
	const unsigned char *binary = nullptr;
	unsigned int bytes = 0;
	if (!isStage(WHC_AUTHENTICATE)) {
		setStage(WHC_ERROR);
	} else if (!bs.auth || msg->getOrigin() != bs.auth->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processAuthenticationResponse(msg, bytes, binary)) {
		setStage(WHC_ERROR);
	} else if (!bs.authenticator.authenticateHost(binary, bytes)) {
		setStage(WHC_ERROR);
	} else {
		WH_LOG_DEBUG("Authentication succeeded");
		setStage(WHC_BOOTSTRAP);
	}
}

Message* ClientHub::createFindRootRequest() {
	try {
		auto msg = Protocol::createFindRootRequest(0, 0, getUid(), 0);
		if (msg) {
			return msg;
		} else {
			throw Exception(EX_ALLOCFAILED);
		}
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::processFindRootResponse(Message *msg) noexcept {
	uint64_t root = 0;
	if (!isStage(WHC_BOOTSTRAP)) {
		setStage(WHC_ERROR);
	} else if (!bs.node || msg->getOrigin() != bs.node->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processFindRootResponse(msg, getUid(), root)) {
		setStage(WHC_ERROR);
	} else {
		bs.root = root;
		setStage(WHC_ROOT);
		findRoot();
	}
}

void ClientHub::processGetKeyResponse(Message *msg) noexcept {
	if (!isStage(WHC_GETKEY)) {
		setStage(WHC_ERROR);
	} else if (!bs.node || msg->getOrigin() != bs.node->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::verify(msg, (verifyHost() ? getPKI() : nullptr))) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processGetKeyResponse(msg, &bs.nonce)) {
		setStage(WHC_ERROR);
	} else {
		WH_LOG_DEBUG("Session key received");
		setStage(WHC_AUTHORIZE);
		initAuthorization();
	}
}

Message* ClientHub::createRegistrationMessage(bool sign) {
	try {
		auto msg = Protocol::createRegisterRequest(0, getUid(), &bs.nonce,
				nullptr);
		if (msg) {
			if (sign) {
				Protocol::sign(msg, getPKI());
			}
			return msg;
		} else {
			throw Exception(EX_ALLOCFAILED);
		}
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::processRegistrationResponse(Message *msg) noexcept {
	auto origin = msg->getOrigin();
	auto status = msg->getStatus();
	if (!isStage(WHC_AUTHORIZE)) {
		setStage(WHC_ERROR);
	} else if (!bs.node || (ctx.password && !bs.auth)
			|| status == WH_AQLF_REJECTED) {
		setStage(WHC_ERROR);
	} else if (origin == bs.node->getUid() && status == WH_AQLF_ACCEPTED) {
		if (Hub::registerWatcher(bs.node->getUid(), 0, true)) {
			WH_LOG_INFO("Registration succeeded");
			setStage(WHC_REGISTERED);
		} else {
			setStage(WHC_ERROR);
		}
	} else if (bs.auth && origin == bs.auth->getUid()
			&& status == WH_AQLF_REQUEST) {
		msg->setDestination(bs.node->getUid());
	} else {
		setStage(WHC_ERROR);
	}
}

void ClientHub::setStage(int stage) noexcept {
	if (stage != bs.stage) {
		connected = (stage == WHC_REGISTERED);
		bs.stage = stage;
		bs.timer.now();

		if (stage == WHC_IDENTIFY || stage == WHC_BOOTSTRAP) {
			clearIdentifiers();
		} else if (stage == WHC_ERROR) {
			disable(bs.auth);
			disable(bs.node);
		} else if (stage == WHC_REGISTERED) {
			disable(bs.auth);
		} else {
			return;
		}
	}
}

int ClientHub::getStage() const noexcept {
	return bs.stage;
}

bool ClientHub::isStage(int stage) const noexcept {
	return (bs.stage == stage);
}

void ClientHub::loadIdentifiers(bool auth) {
	try {
		if (bs.identifiers.getStatus()) {
			throw Exception(EX_INVALIDSTATE);
		}

		bs.identifiers.setStatus(1);
		bs.identifiers.clear();
		unsigned long long buffer[128];
		auto n = Identity::loadIdentifiers("BOOTSTRAP",
				(auth ? "auths" : "nodes"), buffer, 128);
		if (n && (n = bs.identifiers.put(buffer, n))) {
			bs.identifiers.rewind();
			return;
		} else {
			WH_LOG_DEBUG("Identifiers list is empty");
			throw Exception(EX_RESOURCE);
		}
	} catch (BaseException &e) {
		WH_LOG_EXCEPTION(e);
		clearIdentifiers();
		setStage(WHC_ERROR);
		throw;
	}
}

void ClientHub::clearIdentifiers() noexcept {
	bs.identifiers.setIndex(0);
	bs.identifiers.setLimit(0);
	bs.identifiers.setStatus(0);
	bs.root = 0;
}

void ClientHub::clear() noexcept {
	connected = false;
	memset(&ctx, 0, sizeof(ctx));

	clearIdentifiers();
	bs.auth = nullptr;
	bs.node = nullptr;
	memset(&bs.nonce, 0, sizeof(bs.nonce));
	bs.stage = WHC_IDENTIFY;
	bs.sn = 1;
}

} /* namespace wanhive */
