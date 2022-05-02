/*
 * Hosts.cpp
 *
 * The hosts database
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Hosts.h"
#include "../base/common/Exception.h"
#include "../base/common/Logger.h"
#include <cstring>

namespace {
/**
 * Helper functions for formatting a hosts file.
 */
static void writeTuple(FILE *f, unsigned long long uid, const char *host,
		const char *service) noexcept {
	fprintf(f, "%llu\t%s\t%s%s", uid, host, service, wanhive::Storage::NEWLINE);
}

static void writeTuple(FILE *f, unsigned long long uid, const char *host,
		const char *service, int type) noexcept {
	fprintf(f, "%llu\t%s\t%s\t%d%s", uid, host, service, type,
			wanhive::Storage::NEWLINE);
}

static void writeHeading(FILE *f, int version) noexcept {
	if (version == 1) {
		fprintf(f, "# Revision: %d%s", version, wanhive::Storage::NEWLINE);
		fprintf(f, "# UID\tHOSTNAME\tSERVICE\tTYPE%s",
				wanhive::Storage::NEWLINE);
	}
}

}  // namespace

namespace wanhive {

Hosts::Hosts() noexcept {
	memset(&db, 0, sizeof(db));
}

Hosts::Hosts(const char *path, bool readOnly) {
	memset(&db, 0, sizeof(db));
	open(path, readOnly);
}

Hosts::~Hosts() {
	clear();
}

void Hosts::open(const char *path, bool readOnly) {
	try {
		clear();
		openConnection(path, readOnly);
		if (!readOnly) {
			createTable();
		}
		prepareStatements();
	} catch (const BaseException &e) {
		//Clean up to prevent resource leak
		clear();
		throw;
	}
}

void Hosts::batchUpdate(const char *path) {
	if (!db.conn || Storage::testFile(path) != 1) {
		throw Exception(EX_RESOURCE);
	}

	auto f = Storage::openStream(path, "r", false);
	if (!f) {
		throw Exception(EX_INVALIDPARAM);
	}

	try {
		beginTransaction();
		//-----------------------------------------------------------------
		char line[2048];
		char format[64];
		NameInfo ni;
		snprintf(format, sizeof(format), " %%llu %%%zus %%%zus %%d ",
				(sizeof(ni.host) - 1), (sizeof(ni.service) - 1));
		while (fgets(line, sizeof(line), f)) {
			unsigned long long id = 0;
			ni.type = 0;
			if (sscanf(line, format, &id, ni.host, ni.service, &ni.type) < 3) {
				continue;
			} else if (put(id, ni) == 0) {
				continue;
			} else {
				break;
			}
		}
		Storage::closeStream(f);
		//-----------------------------------------------------------------
		endTransaction();
	} catch (const BaseException &e) {
		Storage::closeStream(f);
		throw;
	}
}

void Hosts::batchDump(const char *path, int version) {
	if (db.conn) {
		//-----------------------------------------------------------------
		auto query =
				"SELECT uid, name, service, type FROM hosts ORDER BY uid ASC";
		sqlite3_stmt *stmt = nullptr;
		if (sqlite3_prepare_v2(db.conn, query, strlen(query), &stmt,
				nullptr) != SQLITE_OK) {
			finalize(stmt);
			throw Exception(EX_INVALIDSTATE);
		}
		//-----------------------------------------------------------------
		auto f = Storage::openStream(path, "w", true);
		if (!f) {
			finalize(stmt);
			throw Exception(EX_INVALIDPARAM);
		}

		writeHeading(f, version);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			auto id = (unsigned long long) sqlite3_column_int64(stmt, 0);
			auto host = (const char*) sqlite3_column_text(stmt, 1);
			auto service = (const char*) sqlite3_column_text(stmt, 2);
			auto type = sqlite3_column_int(stmt, 3);
			if (version == 1) {
				writeTuple(f, id, host, service, type);
			} else {
				writeTuple(f, id, host, service);
			}
		}
		Storage::closeStream(f);
		finalize(stmt);
	} else {
		throw Exception(EX_RESOURCE);
	}
}

int Hosts::get(unsigned long long uid, NameInfo &ni) noexcept {
	if (db.conn && db.qStmt) {
		int ret = 0;
		int z;
		memset(&ni, 0, sizeof(ni));
		sqlite3_bind_int64(db.qStmt, 1, uid);
		if ((z = sqlite3_step(db.qStmt)) == SQLITE_ROW) {
			strncpy(ni.host, (const char*) sqlite3_column_text(db.qStmt, 0),
					sizeof(ni.host) - 1);
			strncpy(ni.service, (const char*) sqlite3_column_text(db.qStmt, 1),
					sizeof(ni.service) - 1);
			ni.type = sqlite3_column_int(db.qStmt, 2);
		} else if (z == SQLITE_DONE) {
			//No record found
			ret = 1;
		} else {
			ret = -1;
		}
		reset(db.qStmt);
		return ret;
	} else {
		return -1;
	}
}

int Hosts::put(unsigned long long uid, const NameInfo &ni) noexcept {
	if (db.conn && db.iStmt) {
		int ret = 0;
		sqlite3_bind_int64(db.iStmt, 1, uid);
		sqlite3_bind_text(db.iStmt, 2, ni.host, strlen(ni.host), SQLITE_STATIC);
		sqlite3_bind_text(db.iStmt, 3, ni.service, strlen(ni.service),
		SQLITE_STATIC);
		sqlite3_bind_int(db.iStmt, 4, ni.type);
		if (sqlite3_step(db.iStmt) != SQLITE_DONE) {
			ret = -1;
		}
		reset(db.iStmt);
		return ret;
	} else {
		return -1;
	}
}

int Hosts::remove(unsigned long long uid) noexcept {
	if (db.conn && db.dStmt) {
		int ret = 0;
		sqlite3_bind_int64(db.dStmt, 1, uid);
		if (sqlite3_step(db.dStmt) != SQLITE_DONE) {
			ret = -1;
		}
		reset(db.dStmt);
		return ret;
	} else {
		return -1;
	}
}

int Hosts::list(unsigned long long uids[], unsigned int &count,
		int type) noexcept {
	if (count == 0) {
		return 0;
	} else if (uids && db.conn && db.lStmt) {
		sqlite3_bind_int(db.lStmt, 1, type);
		sqlite3_bind_int(db.lStmt, 2, count);
		unsigned int x = 0;
		int z = 0;
		while ((x < count) && ((z = sqlite3_step(db.lStmt)) == SQLITE_ROW)) {
			uids[x++] = sqlite3_column_int64(db.lStmt, 0);
		}
		reset(db.lStmt);
		count = x;
		if (z == SQLITE_ROW || z == SQLITE_DONE) {
			return 0;
		} else {
			return -1;
		}
	} else {
		count = 0;
		return -1;
	}
}

void Hosts::createDummy(const char *path, int version) {
	auto f = Storage::openStream(path, "w", true);
	if (f) {
		auto host = "127.0.0.1";
		char service[32];
		writeHeading(f, version);
		for (unsigned long long id = 0; id < 256; id++) {
			unsigned int port = (9001 + id);
			snprintf(service, sizeof(service), "%d", port);
			if (version == 1) {
				writeTuple(f, id, host, service, 0);
			} else {
				writeTuple(f, id, host, service);
			}
		}
		Storage::closeStream(f);
	} else {
		throw Exception(EX_INVALIDPARAM);
	}
}

void Hosts::clear() noexcept {
	closeStatements();
	closeConnection();
}

void Hosts::openConnection(const char *path, bool readOnly) {
	closeConnection();
	int flags = readOnly ?
	SQLITE_OPEN_READONLY :
							(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
	sqlite3 *conn = nullptr;
	if (sqlite3_open_v2(path, &conn, flags, nullptr) != SQLITE_OK) {
		WH_LOG_DEBUG("%s", sqlite3_errmsg(conn));
		//sqlite3_open_v2 always returns a connection handle
		sqlite3_close(conn);
		throw Exception(EX_INVALIDOPERATION);
	} else {
		db.conn = conn;
	}
}

void Hosts::closeConnection() noexcept {
	sqlite3_close_v2(db.conn);
	db.conn = nullptr;
}

void Hosts::createTable() {
	auto tq = "CREATE TABLE IF NOT EXISTS hosts ("
			"uid INTEGER NOT NULL UNIQUE ON CONFLICT REPLACE,"
			"name TEXT NOT NULL DEFAULT '127.0.0.1',"
			"service TEXT NOT NULL DEFAULT '9000',"
			"type INTEGER NOT NULL DEFAULT 0)";
	if (!db.conn
			|| sqlite3_exec(db.conn, tq, nullptr, nullptr, nullptr) != SQLITE_OK) {
		WH_LOG_DEBUG("Could not create database tables");
		throw Exception(EX_INVALIDOPERATION);
	} else {
		//SUCCESS
	}
}

void Hosts::prepareStatements() {
	auto iq = "INSERT INTO hosts (uid, name, service, type) VALUES (?,?,?,?)";
	auto sq = "SELECT name, service, type FROM hosts WHERE uid=?";
	auto dq = "DELETE FROM hosts WHERE uid=?";
	auto lq = "SELECT uid FROM hosts WHERE type=? ORDER BY RANDOM() LIMIT ?";

	closeStatements();
	if (!db.conn
			|| (sqlite3_prepare_v2(db.conn, iq, strlen(iq), &db.iStmt, nullptr)
					!= SQLITE_OK)
			|| (sqlite3_prepare_v2(db.conn, sq, strlen(sq), &db.qStmt, nullptr)
					!= SQLITE_OK)
			|| (sqlite3_prepare_v2(db.conn, dq, strlen(dq), &db.dStmt, nullptr)
					!= SQLITE_OK)
			|| (sqlite3_prepare_v2(db.conn, lq, strlen(lq), &db.lStmt, nullptr)
					!= SQLITE_OK)) {
		closeStatements();
		WH_LOG_DEBUG("Could not create prepared statements");
		throw Exception(EX_INVALIDOPERATION);
	} else {
		resetStatements();
	}
}

void Hosts::resetStatements() noexcept {
	reset(db.iStmt);
	reset(db.qStmt);
	reset(db.dStmt);
	reset(db.lStmt);
}

void Hosts::closeStatements() noexcept {
	finalize(db.iStmt);
	finalize(db.qStmt);
	finalize(db.dStmt);
	finalize(db.lStmt);

	db.iStmt = nullptr;
	db.qStmt = nullptr;
	db.dStmt = nullptr;
	db.lStmt = nullptr;
}

void Hosts::reset(sqlite3_stmt *stmt) noexcept {
	if (stmt) {
		sqlite3_clear_bindings(stmt);
		sqlite3_reset(stmt);
	}
}

void Hosts::finalize(sqlite3_stmt *stmt) noexcept {
	reset(stmt);
	sqlite3_finalize(stmt); //Harmless no-op on nullptr
}

void Hosts::beginTransaction() {
	if (!db.conn
			|| sqlite3_exec(db.conn, "BEGIN", nullptr, nullptr, nullptr)
					!= SQLITE_OK) {
		throw Exception(EX_INVALIDOPERATION);
	}
}

void Hosts::endTransaction() {
	if (!db.conn
			|| sqlite3_exec(db.conn, "COMMIT", nullptr, nullptr, nullptr)
					!= SQLITE_OK) {
		throw Exception(EX_INVALIDOPERATION);
	}
}

void Hosts::cancelTransaction() {
	if (!db.conn
			|| sqlite3_exec(db.conn, "ROLLBACK", nullptr, nullptr, nullptr)
					!= SQLITE_OK) {
		throw Exception(EX_INVALIDOPERATION);
	}
}

} /* namespace wanhive */
