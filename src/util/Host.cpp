/*
 * Host.cpp
 *
 * Database of Wanhive hosts
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Host.h"
#include "../base/Logger.h"
#include "../base/common/Exception.h"
#include <cstring>

namespace {
/**
 * Helper functions for writing to the hosts file.
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

Host::Host() noexcept {
	db = nullptr;
	iStmt = nullptr;
	qStmt = nullptr;
	dStmt = nullptr;
	lStmt = nullptr;
}

Host::Host(const char *path, bool readOnly) {
	db = nullptr;
	iStmt = nullptr;
	qStmt = nullptr;
	dStmt = nullptr;
	lStmt = nullptr;
	open(path, readOnly);
}

Host::~Host() {
	clear();
}

void Host::open(const char *path, bool readOnly) {
	try {
		clear();
		openConnection(path, readOnly);
		if (!readOnly) {
			createTable();
		}
		prepareStatements();
	} catch (BaseException &e) {
		//Clean up to prevent resource leak
		clear();
		throw;
	}
}

void Host::batchUpdate(const char *path) {
	if (!db || Storage::testFile(path) != 1) {
		throw Exception(EX_RESOURCE);
	}

	FILE *f = Storage::openStream(path, "rt", false);
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
	} catch (BaseException &e) {
		Storage::closeStream(f);
		throw;
	}
}

void Host::batchDump(const char *path, int version) {
	if (db) {
		//-----------------------------------------------------------------
		const char *query = "SELECT uid, name, service, type FROM hosts";
		sqlite3_stmt *stmt = nullptr;
		if (sqlite3_prepare_v2(db, query, strlen(query), &stmt,
				nullptr) != SQLITE_OK) {
			finalize(stmt);
			throw Exception(EX_INVALIDSTATE);
		}
		//-----------------------------------------------------------------
		FILE *f = Storage::openStream(path, "wt", true);
		if (!f) {
			finalize(stmt);
			throw Exception(EX_INVALIDPARAM);
		}

		writeHeading(f, version);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			unsigned long long id = sqlite3_column_int64(stmt, 0);
			const char *host = (const char*) sqlite3_column_text(stmt, 1);
			const char *service = (const char*) sqlite3_column_text(stmt, 2);
			unsigned int type = sqlite3_column_int(stmt, 3);
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

int Host::get(unsigned long long uid, NameInfo &ni) noexcept {
	if (db && qStmt) {
		int ret = 0;
		int z;
		memset(&ni, 0, sizeof(ni));
		sqlite3_bind_int64(qStmt, 1, uid);
		if ((z = sqlite3_step(qStmt)) == SQLITE_ROW) {
			const unsigned char *x = sqlite3_column_text(qStmt, 0);
			strcpy(ni.host, (const char*) x);
			x = sqlite3_column_text(qStmt, 1);
			strcpy(ni.service, (const char*) x);
			ni.type = sqlite3_column_int(qStmt, 2);
		} else if (z == SQLITE_DONE) {
			//No record found
			ret = 1;
		} else {
			ret = -1;
		}
		reset(qStmt);
		return ret;
	} else {
		return -1;
	}
}

int Host::put(unsigned long long uid, const NameInfo &ni) noexcept {
	if (db && iStmt) {
		int ret = 0;
		sqlite3_bind_int64(iStmt, 1, uid);
		sqlite3_bind_text(iStmt, 2, ni.host, strlen(ni.host), SQLITE_STATIC);
		sqlite3_bind_text(iStmt, 3, ni.service, strlen(ni.service),
		SQLITE_STATIC);
		sqlite3_bind_int(iStmt, 4, ni.type);
		if (sqlite3_step(iStmt) != SQLITE_DONE) {
			ret = -1;
		}
		reset(iStmt);
		return ret;
	} else {
		return -1;
	}
}

int Host::remove(unsigned long long uid) noexcept {
	if (db && dStmt) {
		int ret = 0;
		sqlite3_bind_int64(dStmt, 1, uid);
		if (sqlite3_step(dStmt) != SQLITE_DONE) {
			ret = -1;
		}
		reset(dStmt);
		return ret;
	} else {
		return -1;
	}
}

int Host::list(unsigned long long uids[], unsigned int &count,
		int type) noexcept {
	if (uids && count && db && lStmt) {
		sqlite3_bind_int(lStmt, 1, type);
		sqlite3_bind_int(lStmt, 2, count);
		unsigned int x = 0;
		int z = 0;
		while ((x < count) && ((z = sqlite3_step(lStmt)) == SQLITE_ROW)) {
			uids[x++] = sqlite3_column_int64(lStmt, 0);
		}
		reset(lStmt);
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

void Host::createDummy(const char *path, int version) {
	FILE *f = Storage::openStream(path, "wt", true);
	if (f) {
		const char *host = "127.0.0.1";
		char service[32];
		int type = 0;
		writeHeading(f, version);
		for (unsigned long long id = 0; id < 256; id++) {
			unsigned int port = (9001 + id);
			snprintf(service, sizeof(service), "%d", port);
			if (version == 1) {
				writeTuple(f, id, host, service, type);
			} else {
				writeTuple(f, id, host, service);
			}
		}
		Storage::closeStream(f);
	} else {
		throw Exception(EX_INVALIDPARAM);
	}
}

void Host::clear() noexcept {
	finalize(iStmt);
	finalize(qStmt);
	finalize(dStmt);
	finalize(lStmt);
	closeConnection(db);
	db = nullptr;
	iStmt = qStmt = dStmt = lStmt = nullptr;
}

void Host::openConnection(const char *path, bool readOnly) {
	int flags = readOnly ?
	SQLITE_OPEN_READONLY :
							(SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
	if (sqlite3_open_v2(path, &db, flags, nullptr) != SQLITE_OK) {
		WH_LOG_DEBUG("%s", sqlite3_errmsg(db));
		//sqlite3_open_v2 always returns a connection handle
		sqlite3_close(db);
		db = nullptr;
		throw Exception(EX_INVALIDSTATE);
	}
}

void Host::closeConnection(sqlite3 *db) noexcept {
	sqlite3_close(db);
}

void Host::createTable() {
	const char *tq = "CREATE TABLE IF NOT EXISTS hosts ("
			"uid INTEGER NOT NULL UNIQUE ON CONFLICT REPLACE,"
			"name TEXT NOT NULL DEFAULT '127.0.0.1',"
			"service TEXT NOT NULL DEFAULT '9000',"
			"type INTEGER NOT NULL DEFAULT 0)";
	char *errMsg = nullptr;
	if (sqlite3_exec(db, tq, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		WH_LOG_DEBUG("%s", errMsg);
		sqlite3_free(errMsg);
		throw Exception(EX_INVALIDSTATE);
	} else {
		//SUCCESS
	}
}

void Host::prepareStatements() {
	const char *iq =
			"INSERT INTO hosts (uid, name, service, type) VALUES (?,?,?,?)";
	const char *sq = "SELECT name, service, type FROM hosts WHERE uid=?";
	const char *dq = "DELETE FROM hosts WHERE uid=?";
	const char *lq =
			"SELECT uid FROM hosts WHERE type=? ORDER BY RANDOM() LIMIT ?";

	if ((sqlite3_prepare_v2(db, iq, strlen(iq), &iStmt, nullptr) != SQLITE_OK)
			|| (sqlite3_prepare_v2(db, sq, strlen(sq), &qStmt, nullptr)
					!= SQLITE_OK)
			|| (sqlite3_prepare_v2(db, dq, strlen(dq), &dStmt, nullptr)
					!= SQLITE_OK)
			|| (sqlite3_prepare_v2(db, lq, strlen(lq), &lStmt, nullptr)
					!= SQLITE_OK)) {
		finalize(iStmt);
		finalize(qStmt);
		finalize(dStmt);
		finalize(lStmt);
		iStmt = qStmt = dStmt = lStmt = nullptr;
		WH_LOG_DEBUG("Could not create prepared statements");
		throw Exception(EX_INVALIDSTATE);
	} else {
		//Make sure that everything is good and shiny
		reset(iStmt);
		reset(qStmt);
		reset(dStmt);
		reset(lStmt);
	}
}

void Host::reset(sqlite3_stmt *stmt) noexcept {
	if (stmt) {
		sqlite3_clear_bindings(stmt);
		sqlite3_reset(stmt);
	}
}

void Host::finalize(sqlite3_stmt *stmt) noexcept {
	reset(stmt);
	sqlite3_finalize(stmt); //Harmless no-op on nullptr
}

void Host::beginTransaction() {
	char *errMsg = nullptr;
	if (sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr,
			&errMsg) != SQLITE_OK) {
		WH_LOG_DEBUG("%s", errMsg);
		throw Exception(EX_INVALIDOPERATION);
	}
}

void Host::endTransaction() {
	char *errMsg = nullptr;
	if (sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr,
			&errMsg) != SQLITE_OK) {
		WH_LOG_DEBUG("%s", errMsg);
		throw Exception(EX_INVALIDOPERATION);
	}
}

void Host::deferTransaction() {
	char *errMsg = nullptr;
	if (sqlite3_exec(db, "PRAGMA synchronous = OFF", nullptr, nullptr,
			&errMsg) != SQLITE_OK) {
		WH_LOG_DEBUG("%s", errMsg);
		throw Exception(EX_INVALIDOPERATION);
	}

	if (sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", nullptr, nullptr,
			&errMsg) != SQLITE_OK) {
		WH_LOG_DEBUG("%s", errMsg);
		throw Exception(EX_INVALIDOPERATION);
	}
}

} /* namespace wanhive */
