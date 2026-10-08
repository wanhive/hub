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
#include "../../base/common/Exception.h"
#include "../../base/common/Logger.h"
#include "../../util/commands.h"
#include "../../util/PKI.h"
#include <cstring>
#include <new>

namespace {

/* Message trace types */
enum MessageTrace : uint32_t {
	TRACE_TOKEN = 1U, TRACE_PAKE = 2U
};

constexpr const char *DEF_QUERY =
		"select uid,salt,verifier,type from wh_thing where uid=$1 and domainuid in (select wh_domain.uid from wh_domain,wh_user where wh_user.uid=wh_domain.useruid and wh_user.status=1)";

}  // namespace

namespace wanhive {

AuthenticationHub::AuthenticationHub(unsigned long long uid,
		const char *path) noexcept :
		Hub { uid, path } {
	clear();
}

AuthenticationHub::~AuthenticationHub() {

}

void AuthenticationHub::expel(Watcher *w) noexcept {
	auto index = pake.get(w->getUid());
	if (index != pake.end()) {
		Verifier *verifier { };
		pake.getValue(index, verifier);
		pake.remove(index);
		delete verifier;
	}
	pkey.removeKey(w->getUid());
	Hub::expel(w);
}

void AuthenticationHub::configure(void *arg) {
	try {
		Hub::configure(arg);
		auto &conf = Identity::getOptions();
		dbi.info.name = conf.getString("AUTH", "database");
		dbi.command = conf.getString("AUTH", "query", DEF_QUERY);
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
	pake.iterate(deleteVerifiers, this);
	pkey.clear();
	DataStore::close();
	clear();
	Hub::cleanup();
}

void AuthenticationHub::maintain() noexcept {
	try {
		auto status = DataStore::health();
		switch (status) {
		case DBHealth::READY:
			break;
		default:
			DataStore::reset(true);
			break;
		}
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
	} catch (...) {
		WH_LOG_EXCEPTION_U();
	}
}

void AuthenticationHub::route(Message *message) noexcept {
	if (message->getStatus() != WH_AQLF_REQUEST) {
		handleInvalidRequest(message);
		return;
	}

	switch (message->getCommand()) {
	case WH_CMD_NULL:
		handlePakeRequest(message);
		break;
	case WH_CMD_BASIC:
		handlePassKeyRequest(message);
		break;
	default:
		handleInvalidRequest(message);
	}
}

int AuthenticationHub::handlePakeRequest(Message *message) noexcept {
	//-----------------------------------------------------------------
	if (message->testTrace(TRACE_TOKEN)) {
		return handleInvalidRequest(message);
	}

	message->setTrace(TRACE_PAKE);
	//-----------------------------------------------------------------
	switch (message->getQualifier()) {
	case WH_QLF_IDENTIFY:
		return handleIdentificationRequest(message);
	case WH_QLF_AUTHENTICATE:
		return handleAuthenticationRequest(message);
	default:
		return handleInvalidRequest(message);
	}
}

int AuthenticationHub::handlePassKeyRequest(Message *message) noexcept {
	switch (message->getQualifier()) {
	case WH_QLF_REGISTER:
		if (message->testTrace(TRACE_PAKE)) {
			return handleAuthorizationRequest(message);
		} else if (message->testTrace(TRACE_TOKEN)) {
			return handleResolutionRequest(message);
		} else {
			return handleInvalidRequest(message);
		}
	case WH_QLF_TOKEN:
		return handleTokenRequest(message);
	default:
		return handleInvalidRequest(message);
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
	if (!nonce.length || pake.contains(origin)) {
		return handleInvalidRequest(message);
	}

	Verifier *verifier { };
	auto success = !isBanned(identity) && (verifier =
			new (std::nothrow) Verifier(true))
			&& loadIdentity(verifier, identity, nonce)
			&& pake.hmPut(origin, verifier);
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
		pake.hmPut(origin, nullptr);

		if (dbi.seed.base && dbi.seed.length) {
			/*
			 * Obfuscate the failed identification request. Salt associated
			 * with an identity should not tend to change on new request.
			 * Nonce should look like it was randomly generated.
			 */
			Data salt { dbi.seed };
			Data hostNonce { nullptr, 0 };

			dummy.fakeSalt(identity, salt);
			dummy.fakeNonce(hostNonce);
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
	if (!pake.hmGet(message->getOrigin(), verifier) || !verifier) {
		return handleInvalidRequest(message);
	}

	Data proof { message->getBytes(0), message->getPayloadLength() };
	bool success = verifier->verify(proof) && verifier->hostProof(proof)
			&& (proof.length && (proof.length < Message::MPS));
	if (success) {
		message->setBytes(0, proof.base, proof.length);
		message->putLength(Message::HLEN + proof.length);
		message->putStatus(WH_AQLF_ACCEPTED);
		message->writeSource(0);
		message->writeDestination(0);
		message->setDestination(message->getOrigin());
		return 0;
	} else {
		//Free up the memory and stop the <source> from making further requests
		delete verifier;
		pake.hmReplace(message->getOrigin(), nullptr, verifier);
		return handleInvalidRequest(message);
	}
}

int AuthenticationHub::handleAuthorizationRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	Verifier *verifier { };
	// Stop the <origin> from making further requests
	if (pake.hmReplace(origin, nullptr, verifier) && verifier
			&& verifier->verified()) {
		return generateAuthorizationResponse(message, verifier->identity(),
				verifier->getGroup());
	} else {
		return handleInvalidRequest(message);
	}
}

int AuthenticationHub::handleResolutionRequest(Message *message) noexcept {
	auto origin = message->getOrigin();
	auto identity = message->getSource();
	Resolved resolved { };
	if (message->getPayloadLength() > Hash::SIZE) {
		unsigned int group { 0xff };
		auto success = !isBanned(identity) && !pkey.contains(origin)
				&& verifyNonce(hash, origin, getUid(), getDigest(message))
				&& loadIdentity(message, group) && pkey.hmPut(origin, {
						identity, (unsigned char) group });
		if (success) {
			message->putLength(Message::HLEN);
			message->putStatus(WH_AQLF_ACCEPTED);
			message->writeSource(0);
			message->writeDestination(0);
			message->setDestination(message->getOrigin());
			return 0;
		} else {
			// Stop the <origin> from making further requests
			pkey.hmReplace(origin, { 0, 0 }, resolved);
			return handleInvalidRequest(message);
		}
	} else if (pkey.hmReplace(origin, { 0, 0 }, resolved)
			&& (resolved.identity != 0)) {
		return generateAuthorizationResponse(message, resolved.identity,
				resolved.group);
	} else {
		return handleInvalidRequest(message);
	}
	return 0;
}

int AuthenticationHub::handleTokenRequest(Message *message) noexcept {
	//-----------------------------------------------------------------
	if (message->testTrace(TRACE_PAKE | TRACE_TOKEN)) {  //Prevent misuse
		return handleInvalidRequest(message);
	}

	message->setTrace(TRACE_TOKEN);
	//-----------------------------------------------------------------
	auto origin = message->getOrigin();
	auto plen = message->getPayloadLength();
	if (plen <= Hash::SIZE) {
		Digest hc { };	//Challenge Key
		generateNonce(hash, origin, getUid(), &hc);
		message->appendBytes(Hash::bytes(hc), Hash::SIZE);
		message->writeSource(0);
		message->writeDestination(0);
		message->setDestination(origin);
		message->putStatus(WH_AQLF_ACCEPTED);
	} else if (plen > Hash::SIZE && verifyHost() && getPKI()) {
		//Extract the challenge key
		unsigned char pt[Message::MPS] { }; //Challenge
		Cache challenge { pt, sizeof(pt) };
		getPKI()->decrypt( { message->getBytes(0), plen }, challenge);
		message->setBytes(0, challenge.base, Hash::SIZE);
		//Build and return the session key
		Digest hc { }; //Response
		generateNonce(hash, origin, getUid(), &hc);
		message->setBytes(Hash::SIZE, Hash::bytes(hc), Hash::SIZE);
		message->writeSource(0);
		message->writeDestination(0);
		message->setDestination(origin);
		message->putLength(Message::HLEN + 2 * Hash::SIZE);
		message->putStatus(WH_AQLF_ACCEPTED);
		message->sign(getPKI());
	} else {
		return handleInvalidRequest(message);
	}
	return 0;
}

int AuthenticationHub::handleInvalidRequest(Message *message) noexcept {
	message->writeSource(0);
	message->writeDestination(0);
	message->setDestination(message->getOrigin());
	message->putLength(Message::HLEN);
	message->putStatus(WH_AQLF_REJECTED);
	return 0;
}

bool AuthenticationHub::isBanned(unsigned long long identity) const noexcept {
	return false;
}

bool AuthenticationHub::loadIdentity(Verifier *verifier,
		unsigned long long identity, const Data &nonce) noexcept {
	try {
		resolve(identity, nonce, verifier);
		return true;
	} catch (const BaseException &e) {
		WH_LOG_EXCEPTION(e);
		return false;
	} catch (...) {
		WH_LOG_EXCEPTION_U();
		return false;
	}
}

bool AuthenticationHub::loadIdentity(Message *message,
		unsigned int &group) noexcept {
	try {
		group = resolve(message);
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
						> Message::MPS)) {
			return handleInvalidRequest(message);
		}

		message->setData16(0, salt.length);
		message->setBytes(2 * sizeof(uint16_t), salt.base, salt.length);

		message->setData16(sizeof(uint16_t), nonce.length);
		message->setBytes(2 * sizeof(uint16_t) + salt.length, nonce.base,
				nonce.length);
		message->putLength(
				Message::HLEN + 2 * sizeof(uint64_t) + salt.length
						+ nonce.length);
		message->putStatus(WH_AQLF_ACCEPTED);
		message->writeSource(0);
		message->writeDestination(0);
		message->setDestination(message->getOrigin());
	}
	return 0;
}

int AuthenticationHub::generateAuthorizationResponse(Message *message,
		unsigned long long identity, unsigned int group) noexcept {
	if (message && message->getPayloadLength() == Hash::SIZE) {
		//Message is signed on behalf of the authenticated client
		message->writeSource(identity);
		message->writeSession(group);
		if (message->sign(getPKI())) {
			message->setDestination(message->getOrigin());
			return 0;
		} else {
			return handleInvalidRequest(message);
		}
	}

	return 0;
}

void AuthenticationHub::resolve(unsigned long long identity, const Data &nonce,
		Verifier *verifier) {
	if (!verifier || !nonce.base || !nonce.length) {
		throw Exception(EX_ARGUMENT);
	}

	//-----------------------------------------------------------------
	if (DataStore::health() != DBHealth::READY) {
		throw Exception(EX_RESOURCE);
	}

	char identityString[64];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "%llu", identity);

	const char *paramValues[1];
	paramValues[0] = identityString;

	auto conn = DataStore::connection();
	auto res = PQexecParams(conn, dbi.command, 1, nullptr, paramValues, nullptr,
			nullptr, 1);
	if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
		PQclear(res);
		throw Exception(EX_OPERATION);
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

	if (!status) {
		throw Exception(EX_SECURITY);
	}
}

unsigned int AuthenticationHub::resolve(Message *message) {
	if (!message) {
		throw Exception(EX_ARGUMENT);
	}

	//-----------------------------------------------------------------
	if (DataStore::health() != DBHealth::READY) {
		throw Exception(EX_RESOURCE);
	}

	unsigned long long identity = message->getSource();
	char identityString[64];
	memset(identityString, 0, sizeof(identityString));
	snprintf(identityString, sizeof(identityString), "%llu", identity);

	const char *paramValues[1];
	paramValues[0] = identityString;

	auto conn = DataStore::connection();
	auto res = PQexecParams(conn, dbi.command, 1, nullptr, paramValues, nullptr,
			nullptr, 1);
	if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
		PQclear(res);
		throw Exception(EX_OPERATION);
	}
	//-----------------------------------------------------------------
	auto key = PQgetvalue(res, 0, 2);

	unsigned int group = 0xff;
	if (PQgetlength(res, 0, 3) == sizeof(uint32_t)) {
		group = (ntohl(*((uint32_t*) PQgetvalue(res, 0, 3))));
	}

	PKI pki;
	auto status = pki.loadPublicKey(key, true) && message->verify(&pki);
	PQclear(res);

	if (!status) {
		throw Exception(EX_SECURITY);
	}

	return group;
}

void AuthenticationHub::setup() {
	if (dbi.index < ArraySize(dbi.info.ctx.keys)) {
		dbi.info.ctx.keys[dbi.index] = nullptr;
		dbi.info.ctx.values[dbi.index] = nullptr;
		DataStore::open(dbi.info);
	} else {
		throw Exception(EX_INDEX);
	}
}

void AuthenticationHub::clear() noexcept {
	dbi.info = DBInfo { };
	dbi.index = 0;
	dbi.command = nullptr;
	dbi.seed = { nullptr, 0 };
}

const Digest* AuthenticationHub::getDigest(const Message *message,
		unsigned int index) noexcept {
	return reinterpret_cast<const Digest*>(message->getBytes(index, Hash::SIZE));
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
	((AuthenticationHub*) arg)->pake.getValue(index, verifier);
	delete verifier;
	return 1;
}

} /* namespace wanhive */
