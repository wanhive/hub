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
#include <cerrno>
#include <sys/select.h>

namespace wanhive {

Postgres::Postgres() noexcept {

}

Postgres::~Postgres() {

}

PGconn* Postgres::connect(const PGInfo &info) noexcept {
	if (info.name) {
		return connect(info.name, info.blocking);
	} else {
		return connect(info.ctx.keys, info.ctx.values, info.ctx.expand,
				info.blocking);
	}
}

PGconn* Postgres::connect(const char *name, bool blocking) noexcept {
	if (!name) {
		return nullptr;
	}

	PGconn *conn { };
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

	PGconn *conn { };
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

bool Postgres::ping(const PGInfo &info) noexcept {
	if (info.name) {
		return ping(info.name);
	} else {
		return ping(info.ctx.keys, info.ctx.values, info.ctx.expand);
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

PGHealth Postgres::poll(PGconn *conn, PGPoll type, int timeout) noexcept {
	if (!conn) {
		return PGHealth::BAD;
	}

	PostgresPollingStatusType status;
	if (type == PGPoll::CONNECT) {
		status = PQconnectPoll(conn);
	} else {
		status = PQresetPoll(conn);
	}

	if (status == PGRES_POLLING_FAILED) {
		return PGHealth::BAD;
	}

	if (status == PGRES_POLLING_OK) {
		return PGHealth::READY;
	}

	auto fd = PQsocket(conn);
	if (fd < 0) {
		return PGHealth::BAD;
	}

	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	timeval tv { }, *to { };
	int ret { -1 };

	if (timeout >= 0) {
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		to = &tv;
	}

	if (status == PGRES_POLLING_READING) {
		ret = select(fd + 1, &fdset, nullptr, nullptr, to);
	} else if (status == PGRES_POLLING_WRITING) {
		ret = select(fd + 1, nullptr, &fdset, nullptr, to);
	}

	if (ret >= 0 || errno == EINTR) {
		return PGHealth::OK;
	} else {
		return PGHealth::BAD;
	}
}

} /* namespace wanhive */
