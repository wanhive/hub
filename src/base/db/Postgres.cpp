/*
 * Postgres.cpp
 *
 * PostgreSQL connection manager
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Postgres.h"
#include "../common/Exception.h"

namespace wanhive {

Postgres::Postgres() noexcept {

}

Postgres::~Postgres() {

}

PGconn* Postgres::connect(const PGConnection &info) noexcept {
	if (info.name) {
		return connect(info.name, info.blocking);
	} else {
		return connect(info.params.keys, info.params.values, info.params.expand,
				info.blocking);
	}
}

PGconn* Postgres::connect(const char *name, bool blocking) noexcept {
	if (!name) {
		return nullptr;
	}

	PGconn *conn { nullptr };
	if (blocking) {
		conn = PQconnectdb(name);
	} else {
		conn = PQconnectStart(name);
	}

	if (conn && PQstatus(conn) != CONNECTION_BAD) {
		return conn;
	} else {
		PQfinish(conn);
		return nullptr;
	}
}

PGconn* Postgres::connect(const char *const keys[], const char *const values[],
		bool expand, bool blocking) noexcept {
	if (!keys || !values) {
		return nullptr;
	}

	PGconn *conn { nullptr };
	if (blocking) {
		conn = PQconnectdbParams(keys, values, expand);
	} else {
		conn = PQconnectStartParams(keys, values, expand);
	}

	if (conn && PQstatus(conn) != CONNECTION_BAD) {
		return conn;
	} else {
		PQfinish(conn);
		return nullptr;
	}
}

PGHealth Postgres::health(const PGconn *conn) noexcept {
	if (!conn) {
		return PGHealth::BAD;
	}

	auto status = PQstatus(conn);
	switch (status) {
	case CONNECTION_OK:
		return PGHealth::READY;
	case CONNECTION_BAD:
		return PGHealth::BAD;
	default:
		return PGHealth::OK;
	}
}

void Postgres::disconnect(PGconn *conn) noexcept {
	PQfinish(conn);
}

bool Postgres::reconnect(PGconn *conn, bool blocking) noexcept {
	if (!conn) {
		return false;
	}

	if (blocking) {
		PQreset(conn);
		return (PQstatus(conn) != CONNECTION_BAD);
	} else {
		return (PQresetStart(conn) != 0);
	}
}

bool Postgres::ping(const PGConnection &info) noexcept {
	if (info.name) {
		return ping(info.name);
	} else {
		return ping(info.params.keys, info.params.values, info.params.expand);
	}
}

bool Postgres::ping(const char *name) noexcept {
	if (name) {
		return PQping(name) == PQPING_OK;
	} else {
		return false;
	}
}

bool Postgres::ping(const char *const keys[], const char *const values[],
		bool expand) noexcept {
	if (keys && values) {
		return PQpingParams(keys, values, expand) == PQPING_OK;
	} else {
		return false;
	}
}

} /* namespace wanhive */
