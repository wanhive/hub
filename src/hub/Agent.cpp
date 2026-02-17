/*
 * Agent.cpp
 *
 * Wanhive Agent
 *
 *
 * Copyright (C) 2019 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Agent.h"
#include "Protocol.h"
#include "../base/common/Logger.h"
#include "../base/unix/Time.h"
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

Agent::Agent(unsigned long long uid, const char *path) noexcept :
		Hub { uid, path } {
	clear();
}

Agent::~Agent() {

}

void Agent::expel(Watcher *w) noexcept {
	if (w == boot.auth) {
		boot.auth = nullptr;
	} else if (w == boot.node) {
		boot.node = nullptr;
		boot.connected = false;
	}

	Hub::expel(w);
}

void Agent::configure(void *arg) {
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

void Agent::cleanup() noexcept {
	clear();
	Hub::cleanup();
}

void Agent::maintain() noexcept {
	switch (getStage()) {
	case WHC_IDENTIFY:
		connectToAuthenticator();
		break;
	case WHC_AUTHENTICATE:
		if (overdue(ctx.timeout)) {
			setStage(WHC_ERROR);
		}
		break;
	case WHC_BOOTSTRAP:
		connectToOverlay();
		break;
	case WHC_ROOT:
	case WHC_GETKEY:
	case WHC_AUTHORIZE:
		if (overdue(ctx.timeout)) {
			setStage(WHC_ERROR);
		}
		break;
	case WHC_ERROR:
		if (overdue(ctx.pause)) {
			setStage(WHC_IDENTIFY);
		}
		break;
	case WHC_REGISTERED:
		if (!boot.node) {
			setStage(WHC_ERROR);
		}
		break;
	default:
		Hub::cancel();
		break;
	}
}

void Agent::route(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto source = message->getSource();
	auto command = message->getCommand();
	auto qualifier = message->getQualifier();
	//Prevents replay (UID is the sink)
	message->setDestination(getUid());

	switch (command) {
	case WH_CMD_NULL:
		if (isStage(WHC_ERROR) || isStage(WHC_REGISTERED) || isStage(WHC_FATAL)
				|| !boot.auth) {
			//Bad message
		} else if (origin != boot.auth->getUid()) {
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
				|| !boot.node || (ctx.passwordLength && !boot.auth)) {
			//Bad message
		} else if (!(origin == boot.node->getUid()
				|| (boot.auth && origin == boot.auth->getUid()))) {
			//Bad message
		} else if (!(source == 0 || source == getUid())) {
			//Bad message
		} else if (qualifier == WH_QLF_REGISTER) {
			processRegistrationResponse(message);
		} else if (qualifier == WH_QLF_TOKEN) {
			processTokenResponse(message);
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

bool Agent::connected() const noexcept {
	return boot.connected;
}

void Agent::setPassword(const unsigned char *password, unsigned int length,
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

unsigned int Agent::cycle() const noexcept {
	Period p;
	Hub::period(p);
	if (p.once && p.interval) {
		return p.interval;
	} else {
		return 0;
	}
}

double Agent::timestamp() noexcept {
	double seconds { };
	Time::now(CLOCK_REALTIME, seconds);
	return seconds;
}

void Agent::connectToAuthenticator() noexcept {
	Socket *s { };
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
		if (boot.auth) {
			if (boot.auth->aged(ctx.timeout)) {
				WH_LOG_DEBUG("Connection timed out");
				disable(boot.auth);
			}
			return;
		}
		//-----------------------------------------------------------------
		//Load the identifiers list if necessary
		if (!boot.ids.hasSpace()) {
			loadIdentifiers(true);
		}
		//-----------------------------------------------------------------
		//Get the next identifier to probe
		unsigned long long id;
		if (!boot.ids.get(id)) {
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
		boot.auth = s;
		WH_LOG_DEBUG("Contacting authentication node %llu",
				boot.auth->getUid());
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete s;
	}
}

void Agent::connectToOverlay() noexcept {
	Socket *s { };
	try {
		//-----------------------------------------------------------------
		//Check for consistency
		if (!isStage(WHC_BOOTSTRAP)) {
			throw Exception(EX_STATE);
		}
		//-----------------------------------------------------------------
		//Check for timed-out connection
		if (boot.node) {
			if (boot.node->aged(ctx.timeout)) {
				WH_LOG_DEBUG("Connection timed out");
				disable(boot.node);
			}
			return;
		}
		//-----------------------------------------------------------------
		//Load the identifiers list if necessary
		if (!boot.ids.hasSpace()) {
			loadIdentifiers(false);
		}
		//-----------------------------------------------------------------
		//Get the next identifier to probe
		unsigned long long id;
		if (!boot.ids.get(id)) {
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
		boot.node = s;
		WH_LOG_DEBUG("Contacting bootstrap node %llu", boot.node->getUid());
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		delete s;
	}
}

bool Agent::overdue(unsigned int milliseconds) const noexcept {
	return boot.timer.elapsed(milliseconds);
}

void Agent::initAuthentication() noexcept {
	try {
		if (!isStage(WHC_AUTHENTICATE) || !boot.auth) {
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

void Agent::findRoot() noexcept {
	Socket *s { };
	bool fresh { };
	try {
		if (!isStage(WHC_ROOT) || !boot.node) {
			throw Exception(EX_STATE);
		} else if (boot.root == boot.node->getUid()) {
			fresh = false;
			WH_LOG_DEBUG("Found the root node [%llu]", boot.root);
		} else {
			fresh = true;
			NameInfo ni;
			Identity::getAddress(boot.root, ni);
			s = new Socket(ni);
			s->setUid(boot.root);
			WH_LOG_DEBUG("Connecting with the root node [%llu]", boot.root);
		}
		//-----------------------------------------------------------------
		uint64_t rnd[2];
		Random prng;
		prng.bytes(rnd, sizeof(rnd));
		generateNonce(boot.hf, rnd[0], rnd[1], &boot.nonce);
		auto msg = Protocol::createTokenRequest( { 0, 0 },
				{ verifyHost() ? getPKI() : nullptr, &boot.nonce }, nullptr);
		//-----------------------------------------------------------------
		if (!msg) {
			throw Exception(EX_MEMORY);
		} else if (fresh) {
			s->publish(msg);
			attach(s, IO_WR, WATCHER_ACTIVE);
			//Swap and disable
			auto w = boot.node;
			boot.node = s;
			disable(w);
		} else {
			msg->setDestination(boot.node->getUid());
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

void Agent::initAuthorization() noexcept {
	try {
		if (!isStage(WHC_AUTHORIZE) || !boot.node
				|| (ctx.passwordLength && !boot.auth)) {
			throw Exception(EX_STATE);
		} else if (boot.auth) {
			auto msg = createRegistrationRequest(false);
			msg->setDestination(boot.auth->getUid());
			Hub::forward(msg);
			WH_LOG_DEBUG("Initiating authorization");
		} else {
			auto msg = createRegistrationRequest(true);
			msg->setDestination(boot.node->getUid());
			Hub::forward(msg);
			WH_LOG_DEBUG("Initiating registration");
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		setStage(WHC_ERROR);
	}
}

Message* Agent::createIdentificationRequest() {
	try {
		Message *msg { };
		Data nonce;
		if (!boot.verifier.nonce(nonce)) {
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

void Agent::processIdentificationResponse(const Message *msg) noexcept {
	Data salt;
	Data nonce;
	if (!isStage(WHC_IDENTIFY)) {
		setStage(WHC_ERROR);
	} else if (!boot.auth || msg->getOrigin() != boot.auth->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processIdentificationResponse(msg, salt, nonce)) {
		setStage(WHC_ERROR);
	} else {
		Data password { ctx.password, ctx.passwordLength };
		auto f = boot.verifier.scramble(getUid(), password, salt, nonce,
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

Message* Agent::createAuthenticationRequest() {
	try {
		Message *msg { };
		Data proof;
		if (!boot.auth) {
			throw Exception(EX_OPERATION);
		} else if (!boot.verifier.userProof(proof)) {
			throw Exception(EX_STATE);
		} else if (!(msg = Protocol::createAuthenticationRequest(
				{ 0, boot.auth->getUid() }, proof, 0))) {
			throw Exception(EX_MEMORY);
		} else {
			return msg;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void Agent::processAuthenticationResponse(const Message *msg) noexcept {
	Data proof;
	if (!isStage(WHC_AUTHENTICATE)) {
		setStage(WHC_ERROR);
	} else if (!boot.auth || msg->getOrigin() != boot.auth->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processAuthenticationResponse(msg, proof)) {
		setStage(WHC_ERROR);
	} else if (!boot.verifier.verify(proof)) {
		setStage(WHC_ERROR);
	} else {
		WH_LOG_DEBUG("Authentication succeeded");
		setStage(WHC_BOOTSTRAP);
	}
}

Message* Agent::createFindRootRequest() {
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

void Agent::processFindRootResponse(const Message *msg) noexcept {
	uint64_t root { };
	if (!isStage(WHC_BOOTSTRAP)) {
		setStage(WHC_ERROR);
	} else if (!boot.node || msg->getOrigin() != boot.node->getUid()) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processFindRootResponse(msg, getUid(), root)) {
		setStage(WHC_ERROR);
	} else {
		boot.root = root;
		setStage(WHC_ROOT);
		findRoot();
	}
}

void Agent::processTokenResponse(const Message *msg) noexcept {
	if (!isStage(WHC_GETKEY)) {
		setStage(WHC_ERROR);
	} else if (!boot.node || msg->getOrigin() != boot.node->getUid()) {
		setStage(WHC_ERROR);
	} else if (!msg->verify((verifyHost() ? getPKI() : nullptr))) {
		setStage(WHC_ERROR);
	} else if (!Protocol::processTokenResponse(msg, &boot.nonce)) {
		setStage(WHC_ERROR);
	} else {
		WH_LOG_DEBUG("Session key received");
		setStage(WHC_AUTHORIZE);
		initAuthorization();
	}
}

Message* Agent::createRegistrationRequest(bool sign) {
	try {
		auto msg = Protocol::createRegisterRequest( { getUid(), 0 },
				&boot.nonce, nullptr);
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

void Agent::processRegistrationResponse(Message *msg) noexcept {
	auto origin = msg->getOrigin();
	auto status = msg->getStatus();
	if (!isStage(WHC_AUTHORIZE)) {
		setStage(WHC_ERROR);
	} else if (!boot.node || (ctx.passwordLength && !boot.auth)
			|| status == WH_AQLF_REJECTED) {
		setStage(WHC_ERROR);
	} else if (origin == boot.node->getUid() && status == WH_AQLF_ACCEPTED) {
		if (move(boot.node->getUid(), 0, true)) {
			WH_LOG_INFO("Registration succeeded");
			setStage(WHC_REGISTERED);
		} else {
			setStage(WHC_ERROR);
		}
	} else if (boot.auth && origin == boot.auth->getUid()
			&& status == WH_AQLF_REQUEST) {
		msg->setDestination(boot.node->getUid());
	} else {
		setStage(WHC_ERROR);
	}
}

void Agent::setStage(int stage) noexcept {
	if (stage != boot.stage) {
		boot.timer.now();
		boot.stage = stage;
		boot.connected = (stage == WHC_REGISTERED);

		if (stage == WHC_IDENTIFY || stage == WHC_BOOTSTRAP) {
			clearIdentifiers();
		} else if (stage == WHC_ERROR) {
			disable(boot.auth);
			disable(boot.node);
		} else if (stage == WHC_REGISTERED) {
			disable(boot.auth);
		} else {
			return;
		}
	}
}

int Agent::getStage() const noexcept {
	return boot.stage;
}

bool Agent::isStage(int stage) const noexcept {
	return (boot.stage == stage);
}

void Agent::loadIdentifiers(bool auth) {
	try {
		if (boot.ids.getStatus()) {
			throw Exception(EX_STATE);
		}

		boot.ids.setStatus(1);
		boot.ids.clear();
		unsigned long long buffer[128];
		//-----------------------------------------------------------------
		auto n = Identity::getIdentifiers("BOOTSTRAP",
				(auth ? "auths" : "nodes"), buffer, ArraySize(buffer));
		if (!n) {
			n = Identity::getIdentifiers(buffer, ArraySize(buffer),
					(auth ? Hosts::AUTHENTICATOR : Hosts::BOOTSTRAP));
		}
		//-----------------------------------------------------------------
		if (n && (n = boot.ids.put(buffer, n))) {
			boot.ids.rewind();
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

void Agent::clearIdentifiers() noexcept {
	boot.ids.setIndex(0);
	boot.ids.setLimit(0);
	boot.ids.setStatus(0);
	boot.root = 0;
}

void Agent::clear() noexcept {
	memset(&ctx, 0, sizeof(ctx));

	clearIdentifiers();
	boot.auth = nullptr;
	boot.node = nullptr;
	memset(&boot.nonce, 0, sizeof(boot.nonce));
	boot.stage = WHC_IDENTIFY;
	boot.connected = false;
}

} /* namespace wanhive */
