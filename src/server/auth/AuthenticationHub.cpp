/*
 * AuthenticationHub.cpp
 *
 * Wanhive host for distributed authentication
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
#include "../../util/Trust.h"
#include <new>
#include <postgresql/libpq-fe.h>

namespace wanhive {

AuthenticationHub::AuthenticationHub(unsigned long long uid,
		const char *path) noexcept :
		Hub(uid, path), fake(true) {
	memset(&ctx, 0, sizeof(ctx));
}

AuthenticationHub::~AuthenticationHub() {

}

void AuthenticationHub::stop(Watcher *w) noexcept {
	Authenticator *authenticator = nullptr;
	auto index = session.get(w->getUid());
	if (index != session.end()) {
		session.getValue(index, authenticator);
		session.remove(index);
	}
	delete authenticator;
	Hub::stop(w);
}

void AuthenticationHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		decltype(auto) conf = Identity::getConfiguration();
		ctx.connInfo = conf.getString("AUTH", "connInfo");
		ctx.query = conf.getString("AUTH", "query");

		ctx.salt = (const unsigned char*) conf.getString("AUTH", "salt");
		if (ctx.salt) {
			ctx.saltLength = strlen((const char*) ctx.salt);
		} else {
			ctx.saltLength = 0;
		}

		WH_LOG_DEBUG(
				"Authentication hub settings:\nCONNINFO= \"%s\"\nQUERY= \"%s\"\nSALT= \"%s\"\n",
				ctx.connInfo, ctx.query, ctx.salt);
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		throw;
	}
}

void AuthenticationHub::cleanup() noexcept {
	session.iterate(_deleteAuthenticators, this);
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
	auto nonceLength = message->getPayloadLength();
	//-----------------------------------------------------------------
	if (!nonceLength || session.contains(origin)) {
		return handleInvalidRequest(message);
	}
	//-----------------------------------------------------------------
	auto nonce = message->getBytes(0);
	auto identity = message->getSource();
	Authenticator *authenticator = nullptr;
	bool success = !isBanned(identity) && (authenticator =
			new (std::nothrow) Authenticator(true))
			&& loadIdentity(authenticator, identity, nonce, nonceLength)
			&& session.hmPut(origin, authenticator);
	//-----------------------------------------------------------------
	if (success) {
		unsigned int saltLength = 0;
		unsigned int hostNonceLength = 0;
		const unsigned char *salt = nullptr;
		const unsigned char *hostNonce = nullptr;

		authenticator->getSalt(salt, saltLength);
		authenticator->generateNonce(hostNonce, hostNonceLength);
		return generateIdentificationResponse(message, saltLength,
				hostNonceLength, salt, hostNonce);
	} else {
		//Free up the memory and stop the <source> from making further requests
		delete authenticator;
		session.hmPut(origin, nullptr);

		if (ctx.salt && ctx.saltLength) {
			/*
			 * Obfuscate the failed identification request. Salt associated
			 * with an identity should not tend to change on new request.
			 * Nonce should look like it was randomly generated.
			 */
			unsigned int saltLength = ctx.saltLength;
			unsigned int hostNonceLength = 0;
			const unsigned char *salt = ctx.salt;
			const unsigned char *hostNonce = nullptr;

			fake.generateFakeSalt(identity, salt, saltLength);
			fake.generateFakeNonce(hostNonce, hostNonceLength);
			saltLength = Twiddler::min(saltLength, 16);
			return generateIdentificationResponse(message, saltLength,
					hostNonceLength, salt, hostNonce);
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
	auto origin = message->getOrigin();
	Authenticator *authenticator = nullptr;
	//-----------------------------------------------------------------
	if (!session.hmGet(origin, authenticator) || !authenticator) {
		return handleInvalidRequest(message);
	}
	//-----------------------------------------------------------------
	auto proofLength = message->getPayloadLength();
	const unsigned char *binary = nullptr;
	unsigned int bytes = 0;
	bool success = authenticator->authenticateUser(message->getBytes(0),
			proofLength) && authenticator->generateHostProof(binary, bytes)
			&& (bytes && (bytes < Message::PAYLOAD_SIZE));
	if (success) {
		message->setBytes(0, binary, bytes);
		message->putLength(Message::HEADER_SIZE + bytes);
		message->putStatus(WH_AQLF_ACCEPTED);
		message->updateSource(0);
		message->updateDestination(0);
		message->setDestination(message->getOrigin());
		return 0;
	} else {
		//Free up the memory and stop the <source> from making further requests
		delete authenticator;
		session.hmReplace(origin, nullptr, authenticator);
		return handleInvalidRequest(message);
	}
}

int AuthenticationHub::handleAuthorizationRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	Authenticator *authenticator = nullptr;
	session.hmGet(origin, authenticator);

	if (!authenticator || !authenticator->isAuthenticated()) {
		return handleInvalidRequest(message);
	}

	//Message is signed on behalf of the authenticated client
	message->updateSource(authenticator->getIdentity());
	message->updateSession(authenticator->getGroup());
	if (message->sign(getPKI())) {
		message->setDestination(message->getOrigin());
		return 0;
	} else {
		return handleInvalidRequest(message);
	}
}

int AuthenticationHub::handleInvalidRequest(Message *message) noexcept {
	message->updateSource(0);
	message->updateDestination(0);
	message->putLength(Message::HEADER_SIZE);
	message->putStatus(WH_AQLF_REJECTED);
	message->setDestination(message->getOrigin());
	return 0;
}

bool AuthenticationHub::loadIdentity(Authenticator *authenticator,
		unsigned long long identity, const unsigned char *nonce,
		unsigned int nonceLength) noexcept {
	if (!authenticator || !nonce || !nonceLength || !ctx.connInfo
			|| !ctx.query) {
		return false;
	}
	//-----------------------------------------------------------------
	auto conn = PQconnectdb(ctx.connInfo);
	if (PQstatus(conn) == CONNECTION_BAD) {
		WH_LOG_DEBUG("%s", PQerrorMessage(conn));
		PQfinish(conn);
		return false;
	}

	char identityString[128];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "%llu", identity);

	const char *paramValues[1];
	paramValues[0] = identityString;
	//Request binary result
	auto res = PQexecParams(conn, ctx.query, 1, nullptr, paramValues, nullptr,
			nullptr, 1);
	if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
		WH_LOG_DEBUG("%s", PQerrorMessage(conn));
		PQclear(res);
		PQfinish(conn);
		return false;
	}
	//-----------------------------------------------------------------
	auto group = PQgetvalue(res, 0, 3);
	authenticator->setGroup(
			group == nullptr ? 0xff : ntohl(*((uint32_t*) group)));
	auto status = authenticator->identify(identity, nonce, nonceLength,
			PQgetvalue(res, 0, 1), PQgetvalue(res, 0, 2));
	//-----------------------------------------------------------------
	PQclear(res);
	PQfinish(conn);
	return status;
}

bool AuthenticationHub::isBanned(unsigned long long identity) const noexcept {
	return false;
}

int AuthenticationHub::generateIdentificationResponse(Message *message,
		unsigned int saltLength, unsigned int nonceLength,
		const unsigned char *salt, const unsigned char *nonce) noexcept {
	if (message) {
		if (!saltLength || !nonceLength || !salt || !nonce
				|| (saltLength + nonceLength + 2 * sizeof(uint16_t)
						> Message::PAYLOAD_SIZE)) {
			return handleInvalidRequest(message);
		}

		message->setData16(0, saltLength);
		message->setBytes(2 * sizeof(uint16_t), salt, saltLength);

		message->setData16(sizeof(uint16_t), nonceLength);
		message->setBytes(2 * sizeof(uint16_t) + saltLength, nonce,
				nonceLength);
		message->putLength(
				Message::HEADER_SIZE + 2 * sizeof(uint64_t) + saltLength
						+ nonceLength);
		message->putStatus(WH_AQLF_ACCEPTED);
		message->updateSource(0);
		message->updateDestination(0);
		message->setDestination(message->getOrigin());
	}
	return 0;
}

int AuthenticationHub::_deleteAuthenticators(unsigned int index,
		void *arg) noexcept {
	Authenticator *authenticator = nullptr;
	((AuthenticationHub*) arg)->session.getValue(index, authenticator);
	delete authenticator;
	return 1;
}

} /* namespace wanhive */
