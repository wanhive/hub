/*
 * AuthenticationHub.cpp
 *
 * Authentication hub
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "AuthenticationHub.h"
#include "../../base/common/Logger.h"
#include "../../util/commands.h"
#include <new>

namespace wanhive {

AuthenticationHub::AuthenticationHub(unsigned long long uid,
		const char *path) noexcept :
		Hub { uid, path } {
	clear();
}

AuthenticationHub::~AuthenticationHub() {

}

void AuthenticationHub::expel(Watcher *w) noexcept {
	Verifier *verifier { };
	auto index = waitlist.get(w->getUid());
	if (index != waitlist.end()) {
		waitlist.getValue(index, verifier);
		waitlist.remove(index);
	}
	delete verifier;
	Hub::expel(w);
}

void AuthenticationHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		auto &conf = Identity::getOptions();
		dbi.info.name = conf.getString("AUTH", "database");
		dbi.command = conf.getString("AUTH", "query");
		conf.map("RDBMS", loadDatabaseParams, &dbi);
		dbi.seed.base = (const unsigned char*) conf.getString("AUTH", "seed");
		if (dbi.seed.base) {
			dbi.seed.length = strlen((const char*) dbi.seed.base);
		} else {
			dbi.seed.length = 0;
		}

		auto mask = Hub::redact();
		WH_LOG_DEBUG("\nDATABASE= '%s'\nQUERY= '%s'\nSEED= '%s'\n",
				WH_MASK_STR(mask, dbi.info.name),
				WH_MASK_STR(mask, dbi.command),
				WH_MASK_STR(mask, (const char *)dbi.seed.base));
		setup();
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void AuthenticationHub::cleanup() noexcept {
	waitlist.iterate(deleteVerifiers, this);
	things.close();
	clear();
	Hub::cleanup();
}

void AuthenticationHub::maintain() noexcept {
	try {
		auto status = things.health();
		switch (status) {
		case DBHealth::READY:
			break;
		default:
			things.reset(true);
			break;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	} catch (...) {
		WH_LOG_EXCEPTION_U();
	}
}

void AuthenticationHub::route(Message *message) noexcept {
	if (message->getCommand() == WH_CMD_NULL
			&& message->getQualifier() == WH_QLF_IDENTIFY
			&& message->getStatus() == WH_AQLF_REQUEST) {
		handleIdentificationRequest(message);
	} else if (message->getCommand() == WH_CMD_NULL
			&& message->getQualifier() == WH_QLF_AUTHENTICATE
			&& message->getStatus() == WH_AQLF_REQUEST) {
		handleAuthenticationRequest(message);
	} else if (message->getCommand() == WH_CMD_BASIC
			&& message->getQualifier() == WH_QLF_REGISTER
			&& message->getStatus() == WH_AQLF_REQUEST) {
		handleAuthorizationRequest(message);
	} else {
		//UID is the sink
		message->setDestination(getUid());
	}
}

int AuthenticationHub::handleIdentificationRequest(Message *message) noexcept {
	/*
	 * HEADER: SRC=<identity>, DEST=X, ....CMD=0, QLF=1, AQLF=0/1/127
	 * BODY: variable in Request and Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	auto origin = message->getOrigin();
	auto identity = message->getSource();
	Data nonce { message->getBytes(0), message->getPayloadLength() };
	//-----------------------------------------------------------------
	if (!nonce.length || waitlist.contains(origin)) {
		return handleInvalidRequest(message);
	}

	Verifier *verifier { };
	bool success = !isBanned(identity) && (verifier =
			new (std::nothrow) Verifier(true))
			&& loadIdentity(verifier, identity, nonce)
			&& waitlist.hmPut(origin, verifier);
	//-----------------------------------------------------------------
	if (success) {
		Data salt { nullptr, 0 };
		Data hostNonce { nullptr, 0 };

		verifier->salt(salt);
		verifier->nonce(hostNonce);
		return generateIdentificationResponse(message, salt, hostNonce);
	} else {
		//Free up the memory and stop the <origin> from making further requests
		delete verifier;
		waitlist.hmPut(origin, nullptr);

		if (dbi.seed.base && dbi.seed.length) {
			/*
			 * Obfuscate the failed identification request. Salt associated
			 * with an identity should not tend to change on new request.
			 * Nonce should look like it was randomly generated.
			 */
			Data salt { dbi.seed };
			Data hostNonce { nullptr, 0 };

			fake.fakeSalt(identity, salt);
			fake.fakeNonce(hostNonce);
			salt.length = Twiddler::min(salt.length, 16);
			return generateIdentificationResponse(message, salt, hostNonce);
		} else {
			return handleInvalidRequest(message);
		}
	}
}

int AuthenticationHub::handleAuthenticationRequest(Message *message) noexcept {
	/*
	 * HEADER: SRC=0, DEST=X, ....CMD=0, QLF=2, AQLF=0/1/127
	 * BODY: variable in Request and Response
	 * TOTAL: at least 32 bytes in Request and Response
	 */
	Verifier *verifier { };
	if (!waitlist.hmGet(message->getOrigin(), verifier) || !verifier) {
		return handleInvalidRequest(message);
	}

	Data proof { message->getBytes(0), message->getPayloadLength() };
	bool success = verifier->verify(proof) && verifier->hostProof(proof)
			&& (proof.length && (proof.length < Message::PAYLOAD_SIZE));
	if (success) {
		message->setBytes(0, proof.base, proof.length);
		message->putLength(Message::HEADER_SIZE + proof.length);
		message->putStatus(WH_AQLF_ACCEPTED);
		message->writeSource(0);
		message->writeDestination(0);
		message->setDestination(message->getOrigin());
		return 0;
	} else {
		//Free up the memory and stop the <source> from making further requests
		delete verifier;
		waitlist.hmReplace(message->getOrigin(), nullptr, verifier);
		return handleInvalidRequest(message);
	}
}

int AuthenticationHub::handleAuthorizationRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	Verifier *verifier { };
	waitlist.hmGet(origin, verifier);

	if (!verifier || !verifier->verified()) {
		return handleInvalidRequest(message);
	}

	//Message is signed on behalf of the authenticated client
	message->writeSource(verifier->identity());
	message->writeSession(verifier->getGroup());
	if (message->sign(getPKI())) {
		message->setDestination(message->getOrigin());
		return 0;
	} else {
		return handleInvalidRequest(message);
	}
}

int AuthenticationHub::handleInvalidRequest(Message *message) noexcept {
	message->writeSource(0);
	message->writeDestination(0);
	message->putLength(Message::HEADER_SIZE);
	message->putStatus(WH_AQLF_REJECTED);
	message->setDestination(message->getOrigin());
	return 0;
}

bool AuthenticationHub::isBanned(unsigned long long identity) const noexcept {
	return false;
}

bool AuthenticationHub::loadIdentity(Verifier *verifier,
		unsigned long long identity, const Data &nonce) noexcept {
	try {
		things.get(identity, nonce, verifier);
		return true;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return false;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		return false;
	}
}

int AuthenticationHub::generateIdentificationResponse(Message *message,
		const Data &salt, const Data &nonce) noexcept {
	if (message) {
		if (!salt.length || !nonce.length || !salt.base || !nonce.base
				|| (salt.length + nonce.length + 2 * sizeof(uint16_t)
						> Message::PAYLOAD_SIZE)) {
			return handleInvalidRequest(message);
		}

		message->setData16(0, salt.length);
		message->setBytes(2 * sizeof(uint16_t), salt.base, salt.length);

		message->setData16(sizeof(uint16_t), nonce.length);
		message->setBytes(2 * sizeof(uint16_t) + salt.length, nonce.base,
				nonce.length);
		message->putLength(
				Message::HEADER_SIZE + 2 * sizeof(uint64_t) + salt.length
						+ nonce.length);
		message->putStatus(WH_AQLF_ACCEPTED);
		message->writeSource(0);
		message->writeDestination(0);
		message->setDestination(message->getOrigin());
	}
	return 0;
}

void AuthenticationHub::setup() {
	if (dbi.index < ArraySize(dbi.info.ctx.keys)) {
		dbi.info.ctx.keys[dbi.index] = nullptr;
		dbi.info.ctx.values[dbi.index] = nullptr;
		things.setCommand(dbi.command);
		things.open(dbi.info);
	} else {
		throw Exception(EX_OPERATION);
	}
}

void AuthenticationHub::clear() noexcept {
	dbi.info = DBInfo { };
	dbi.index = 0;
	dbi.command = nullptr;
	dbi.seed = { nullptr, 0 };
}

int AuthenticationHub::loadDatabaseParams(const char *option, const char *value,
		void *arg) noexcept {
	auto &ctx = (static_cast<DBConnection*>(arg))->info.ctx;
	auto &index = (static_cast<DBConnection*>(arg))->index;
	auto limit = ArraySize(ctx.keys) - 1;
	if (index < limit) {
		ctx.keys[index] = option;
		ctx.values[index] = value;
		index += 1;
		return 0;
	} else {
		return 1;
	}
}

int AuthenticationHub::deleteVerifiers(unsigned int index, void *arg) noexcept {
	Verifier *verifier { };
	((AuthenticationHub*) arg)->waitlist.getValue(index, verifier);
	delete verifier;
	return 1;
}

} /* namespace wanhive */
