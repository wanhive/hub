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
#include "../../util/Endpoint.h"
#include <postgresql/libpq-fe.h>
#include <new>

namespace wanhive {

AuthenticationHub::AuthenticationHub(unsigned long long uid,
		const char *path) noexcept :
		Hub { uid, path }, fake { true } {
	memset(&ctx, 0, sizeof(ctx));
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
		auto &conf = Identity::getConfiguration();
		ctx.db.name = conf.getString("AUTH", "database");
		ctx.db.query = conf.getString("AUTH", "query");
		conf.map("RDBMS", loadDatabaseParams, &ctx.db);

		ctx.salt = (const unsigned char*) conf.getString("AUTH", "salt");
		if (ctx.salt) {
			ctx.saltLength = strlen((const char*) ctx.salt);
		} else {
			ctx.saltLength = 0;
		}

		auto mask = Hub::redact();
		WH_LOG_DEBUG(
				"Authentication hub settings:\nDATABASE= '%s'\nQUERY= '%s'\nSALT= '%s'\n",
				WH_MASK_STR(mask, ctx.db.name), WH_MASK_STR(mask, ctx.db.query),
				WH_MASK_STR(mask, (const char *)ctx.salt));
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void AuthenticationHub::cleanup() noexcept {
	waitlist.iterate(deleteVerifiers, this);
	closeDatabaseConnection();
	memset(&ctx, 0, sizeof(ctx));
	//Clean up the base class object
	Hub::cleanup();
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

		if (ctx.salt && ctx.saltLength) {
			/*
			 * Obfuscate the failed identification request. Salt associated
			 * with an identity should not tend to change on new request.
			 * Nonce should look like it was randomly generated.
			 */
			Data salt { ctx.salt, ctx.saltLength };
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
	if (!verifier || !nonce.base || !nonce.length || !ctx.db.query) {
		return false;
	}

	//-----------------------------------------------------------------
	auto conn = static_cast<PGconn*>(getDatabaseConnection());
	if (!conn) {
		return false;
	}

	char identityString[64];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "%llu", identity);

	const char *paramValues[1];
	paramValues[0] = identityString;
	//Request binary result
	auto res = PQexecParams(conn, ctx.db.query, 1, nullptr, paramValues,
			nullptr, nullptr, 1);
	if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
		WH_LOG_DEBUG("%s", PQerrorMessage(conn));
		PQclear(res);
		return false;
	}
	//-----------------------------------------------------------------
	auto salt = PQgetvalue(res, 0, 1);
	auto secret = PQgetvalue(res, 0, 2);

	if (PQgetlength(res, 0, 3) == sizeof(uint32_t)) {
		verifier->setGroup(ntohl(*((uint32_t*) PQgetvalue(res, 0, 3))));
	} else {
		verifier->setGroup(0xff);
	}

	auto status = verifier->identify(identity, secret, salt, nonce);

	PQclear(res);
	return status;
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

void* AuthenticationHub::getDatabaseConnection() noexcept {
	if (ctx.db.conn) {
		//Already connected
	} else if (ctx.db.name) {
		ctx.db.conn = PQconnectdb(ctx.db.name);
	} else {
		ctx.db.conn = PQconnectdbParams(ctx.db.params.keys,
				ctx.db.params.values, 0);
	}

	//Check for completion
	if (PQstatus(static_cast<PGconn*>(ctx.db.conn)) != CONNECTION_OK) {
		WH_LOG_DEBUG("%s", PQerrorMessage(static_cast<PGconn*>(ctx.db.conn)));
		closeDatabaseConnection();
	}

	return ctx.db.conn;
}

void AuthenticationHub::closeDatabaseConnection() noexcept {
	if (ctx.db.conn) {
		PQfinish(static_cast<PGconn*>(ctx.db.conn));
		ctx.db.conn = nullptr;
	}
}

int AuthenticationHub::loadDatabaseParams(const char *option, const char *value,
		void *arg) noexcept {
	auto &params = (static_cast<DbConnection*>(arg))->params;
	if (params.index < 63) {
		params.keys[params.index] = option;
		params.values[params.index] = value;
		params.index += 1;
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
