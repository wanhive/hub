/*
 * ClientHub.cpp
 *
 * Base class for wanhive clients
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "ClientHub.h"
#include "Protocol.h"
#include "../base/common/Logger.h"
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

void ClientHub::expel(Watcher *w) noexcept {
	if (w == bs.auth) {
		bs.auth = nullptr;
	} else if (w == bs.node) {
		bs.node = nullptr;
		bs.connected = false;
	}

	Hub::expel(w);
}

void ClientHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		auto &conf = Identity::getOptions();

		auto password = conf.getString("CLIENT", "password", "");
		auto rounds = conf.getNumber("CLIENT", "rounds");
		setPassword((const unsigned char*) password, strlen(password), rounds);

		ctx.timeout = conf.getNumber("CLIENT", "timeout", 5000);
		ctx.pause = conf.getNumber("CLIENT", "pause", 10000);

		auto mask = Hub::redact();
		WH_LOG_DEBUG(
				"\nPASSWORD='%s', HASH_ROUNDS=%u,\n" "IO_TIMEOUT=%ums, RETRY_INTERVAL=%ums\n",
				WH_MASK_STR(mask, (const char *)ctx.password),
				WH_MASK_NUM(mask, ctx.rounds), ctx.timeout, ctx.pause);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::cleanup() noexcept {
	clear();
	Hub::cleanup();
}

void ClientHub::maintain() noexcept {
	switch (getStage()) {
	case WHC_IDENTIFY:
		connectToAuthenticator();
		break;
	case WHC_AUTHENTICATE:
		if (checkStageTimeout(ctx.timeout)) {
			setStage(WHC_ERROR);
		}
		break;
	case WHC_BOOTSTRAP:
		connectToOverlay();
		break;
	case WHC_ROOT:
	case WHC_GETKEY:
	case WHC_AUTHORIZE:
		if (checkStageTimeout(ctx.timeout)) {
			setStage(WHC_ERROR);
		}
		break;
	case WHC_ERROR:
		if (checkStageTimeout(ctx.pause)) {
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
				|| !bs.node || (ctx.passwordLength && !bs.auth)) {
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

bool ClientHub::isConnected() const noexcept {
	return bs.connected;
}

void ClientHub::setPassword(const unsigned char *password, unsigned int length,
		unsigned int rounds) noexcept {
	if (password && length) {
		length = Twiddler::min(length, sizeof(ctx.password)); //trim
		::memcpy(ctx.password, password, length);
		ctx.passwordLength = length;
		ctx.rounds = rounds;
	} else {
		::memset(ctx.password, 0, sizeof(password));
		ctx.passwordLength = 0;
		ctx.rounds = 0;
	}
}

void ClientHub::connectToAuthenticator() noexcept {
	Socket *s = nullptr;
	try {
		//-----------------------------------------------------------------
		//Check for consistency
		if (!isStage(WHC_IDENTIFY)) {
			throw Exception(EX_STATE);
		}

		if (!ctx.passwordLength) {
			WH_LOG_DEBUG("Using PKI for authentication");
			setStage(WHC_BOOTSTRAP);
			return;
		}
		//-----------------------------------------------------------------
		//Check for timed-out connection
		if (bs.auth) {
			if (bs.auth->hasTimedOut(ctx.timeout)) {
				WH_LOG_DEBUG("Connection timed out");
				disable(bs.auth);
			}
			return;
		}
		//-----------------------------------------------------------------
		//Load the identifiers list if necessary
		if (!bs.identifiers.hasSpace()) {
			loadIdentifiers(true);
		}
		//-----------------------------------------------------------------
		//Get the next identifier to probe
		unsigned long long id;
		if (!bs.identifiers.get(id)) {
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
		attach(s, IO_WR, WATCHER_ACTIVE);
		bs.auth = s;
		WH_LOG_DEBUG("Contacting authentication node %llu", bs.auth->getUid());
	} catch (const BaseException &e) {
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
			throw Exception(EX_STATE);
		}
		//-----------------------------------------------------------------
		//Check for timed-out connection
		if (bs.node) {
			if (bs.node->hasTimedOut(ctx.timeout)) {
				WH_LOG_DEBUG("Connection timed out");
				disable(bs.node);
			}
			return;
		}
		//-----------------------------------------------------------------
		//Load the identifiers list if necessary
		if (!bs.identifiers.hasSpace()) {
			loadIdentifiers(false);
		}
		//-----------------------------------------------------------------
		//Get the next identifier to probe
		unsigned long long id;
		if (!bs.identifiers.get(id)) {
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
		attach(s, IO_WR, WATCHER_ACTIVE);
		bs.node = s;
		WH_LOG_DEBUG("Contacting bootstrap node %llu", bs.node->getUid());
	} catch (const BaseException &e) {
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
			throw Exception(EX_STATE);
		} else {
			//Message destination is correctly set
			Hub::forward(createAuthenticationRequest());
			WH_LOG_DEBUG("Authentication initiated");
		}

	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		setStage(WHC_ERROR);
	}
}

void ClientHub::findRoot() noexcept {
	Socket *s = nullptr;
	auto fresh = false;
	try {
		if (!isStage(WHC_ROOT) || !bs.node) {
			throw Exception(EX_STATE);
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
		auto msg = Protocol::createGetKeyRequest( { 0, 0 },
				{ verifyHost() ? getPKI() : nullptr, &bs.nonce }, nullptr);
		//-----------------------------------------------------------------
		if (!msg) {
			throw Exception(EX_MEMORY);
		} else if (fresh) {
			s->publish(msg);
			attach(s, IO_WR, WATCHER_ACTIVE);
			//Swap and disable
			auto w = bs.node;
			bs.node = s;
			disable(w);
		} else {
			msg->setDestination(bs.node->getUid());
			Hub::forward(msg);
		}
		//-----------------------------------------------------------------
		setStage(WHC_GETKEY);
		WH_LOG_DEBUG("Requesting session key");
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete s;
		setStage(WHC_ERROR);
	}
}

void ClientHub::initAuthorization() noexcept {
	try {
		if (!isStage(WHC_AUTHORIZE) || !bs.node
				|| (ctx.passwordLength && !bs.auth)) {
			throw Exception(EX_STATE);
		} else if (bs.auth) {
			auto msg = createRegistrationRequest(false);
			msg->setDestination(bs.auth->getUid());
			Hub::forward(msg);
			WH_LOG_DEBUG("Initiating authorization");
		} else {
			auto msg = createRegistrationRequest(true);
			msg->setDestination(bs.node->getUid());
			Hub::forward(msg);
			WH_LOG_DEBUG("Initiating registration");
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		setStage(WHC_ERROR);
	}
}

Message* ClientHub::createIdentificationRequest() {
	try {
		Message *msg = nullptr;
		Data nonce;
		if (!bs.verifier.nonce(nonce)) {
			throw Exception(EX_SECURITY);
		} else if (!(msg = Protocol::createIdentificationRequest(
				{ getUid(), 0 }, nonce, 0))) {
			throw Exception(EX_MEMORY);
		} else {
			return msg;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::processIdentificationResponse(Message *msg) noexcept {
	Data salt;
	Data nonce;
	if (!isStage(WHC_IDENTIFY)) {
		setStage(WHC_ERROR);
	} else if (!bs.auth || msg->getOrigin() != bs.auth->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processIdentificationResponse(msg, salt, nonce)) {
		setStage(WHC_ERROR);
	} else {
		Data password { ctx.password, ctx.passwordLength };
		auto f = bs.verifier.scramble(getUid(), password, salt, nonce,
				ctx.rounds);
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
		Message *msg = nullptr;
		Data proof;
		if (!bs.auth) {
			throw Exception(EX_OPERATION);
		} else if (!bs.verifier.userProof(proof)) {
			throw Exception(EX_STATE);
		} else if (!(msg = Protocol::createAuthenticationRequest(
				{ 0, bs.auth->getUid() }, proof, 0))) {
			throw Exception(EX_MEMORY);
		} else {
			return msg;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::processAuthenticationResponse(Message *msg) noexcept {
	Data proof;
	if (!isStage(WHC_AUTHENTICATE)) {
		setStage(WHC_ERROR);
	} else if (!bs.auth || msg->getOrigin() != bs.auth->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processAuthenticationResponse(msg, proof)) {
		setStage(WHC_ERROR);
	} else if (!bs.verifier.verify(proof)) {
		setStage(WHC_ERROR);
	} else {
		WH_LOG_DEBUG("Authentication succeeded");
		setStage(WHC_BOOTSTRAP);
	}
}

Message* ClientHub::createFindRootRequest() {
	try {
		auto msg = Protocol::createFindRootRequest( { 0, 0 }, getUid(), 0);
		if (msg) {
			return msg;
		} else {
			throw Exception(EX_MEMORY);
		}
	} catch (const BaseException &e) {
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
	} else if (!msg->verify((verifyHost() ? getPKI() : nullptr))) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processGetKeyResponse(msg, &bs.nonce)) {
		setStage(WHC_ERROR);
	} else {
		WH_LOG_DEBUG("Session key received");
		setStage(WHC_AUTHORIZE);
		initAuthorization();
	}
}

Message* ClientHub::createRegistrationRequest(bool sign) {
	try {
		auto msg = Protocol::createRegisterRequest( { getUid(), 0 }, &bs.nonce,
				nullptr);
		if (msg) {
			if (sign) {
				msg->sign(getPKI());
			}
			return msg;
		} else {
			throw Exception(EX_MEMORY);
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void ClientHub::processRegistrationResponse(Message *msg) noexcept {
	auto origin = msg->getOrigin();
	auto status = msg->getStatus();
	if (!isStage(WHC_AUTHORIZE)) {
		setStage(WHC_ERROR);
	} else if (!bs.node || (ctx.passwordLength && !bs.auth)
			|| status == WH_AQLF_REJECTED) {
		setStage(WHC_ERROR);
	} else if (origin == bs.node->getUid() && status == WH_AQLF_ACCEPTED) {
		if (move(bs.node->getUid(), 0, true)) {
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
		bs.timer.now();
		bs.stage = stage;
		bs.connected = (stage == WHC_REGISTERED);

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
			throw Exception(EX_STATE);
		}

		bs.identifiers.setStatus(1);
		bs.identifiers.clear();
		unsigned long long buffer[128];
		//-----------------------------------------------------------------
		auto n = Identity::getIdentifiers("BOOTSTRAP",
				(auth ? "auths" : "nodes"), buffer, ArraySize(buffer));
		if (!n) {
			n = Identity::getIdentifiers(buffer, ArraySize(buffer),
					(auth ? Hosts::AUTHENTICATOR : Hosts::BOOTSTRAP));
		}
		//-----------------------------------------------------------------
		if (n && (n = bs.identifiers.put(buffer, n))) {
			bs.identifiers.rewind();
			return;
		} else {
			throw Exception(EX_RESOURCE);
		}
	} catch (const BaseException &e) {
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
	memset(&ctx, 0, sizeof(ctx));

	clearIdentifiers();
	bs.auth = nullptr;
	bs.node = nullptr;
	memset(&bs.nonce, 0, sizeof(bs.nonce));
	bs.stage = WHC_IDENTIFY;
	bs.connected = false;
}

} /* namespace wanhive */
