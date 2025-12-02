/*
 * Things.cpp
 *
 * Repository of things
 *
 *
 * Copyright (C) 2025 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Things.h"
#include "../../base/common/Exception.h"
#include <arpa/inet.h>
#include <cstring>

namespace {

constexpr const char *DEF_QUERY =
		"select uid,salt,verifier,type from wh_thing where uid=$1 and domainuid in (select wh_domain.uid from wh_domain,wh_user where wh_user.uid=wh_domain.useruid and wh_user.status=1)";

}  // namespace

namespace wanhive {

Things::Things() noexcept {

}

Things::Things(const DBInfo &info) :
		DataStore { info } {
}

Things::~Things() {

}

void Things::setCommand(const char *command) noexcept {
	this->command = command;
}

void Things::get(unsigned long long identity, const Data &nonce,
		Verifier *verifier) {
	if (!verifier || !nonce.base || !nonce.length || !command) {
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

	auto conn = DataStore::getHandle();
	auto query = command ? command : DEF_QUERY;
	//Request binary result
	auto res = PQexecParams(conn, query, 1, nullptr, paramValues, nullptr,
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

} /* namespace wanhive */
