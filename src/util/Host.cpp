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
#include<cstring>

namespace wanhive {

Host::Host() noexcept {
	db = nullptr;
	iStmt = nullptr;
	qStmt = nullptr;
	dStmt = nullptr;
}

Host::Host(const char *path, bool readOnly) {
	db = nullptr;
	iStmt = nullptr;
	qStmt = nullptr;
	dStmt = nullptr;
	load(path, readOnly); //cleans up in case of exception
}

Host::~Host() {
	clear();
}

void Host::load(const char *path, bool readOnly) {
	try {
		clear();
		openConnection(path, readOnly);
		if (!readOnly) {
			createTable();
		}
		//deferTransaction();
		prepareStatements();
	} catch (BaseException &e) {
		//Clean up before throw
		clear();
		throw;
	}
}

void Host::batchUpdate(const char *path) {
	if (!db || Storage::testFile(path) != 1) {
		throw Exception(EX_RESOURCE);
	}
	//-----------------------------------------------------------------
	FILE *f = Storage::openStream(path, "rt", false);
	if (!f) {
		throw Exception(EX_INVALIDPARAM);
	}
	//-----------------------------------------------------------------
	try {
		beginTransaction();
		unsigned long long id;
		nameInfo ni;

		char format[32];
		snprintf(format, sizeof(format), " %%llu %%%zus %%%zus ",
				(sizeof(ni.host) - 1), (sizeof(ni.service) - 1));
		while (fscanf(f, format, &id, ni.host, ni.service) == 3) {
			if (addHost(id, ni) == -1) { //Stop on error
				break;
			}
		}
		endTransaction();
		Storage::closeStream(f);
	} catch (BaseException &e) {
		Storage::closeStream(f);
		throw;
	}
}

void Host::batchDump(const char *path) {
	if (db) {
		//-----------------------------------------------------------------
		const char *query = "SELECT uid, name, service FROM hosts";
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

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			unsigned long long id = sqlite3_column_int64(stmt, 0);
			const unsigned char *host = sqlite3_column_text(stmt, 1);
			const unsigned char *service = sqlite3_column_text(stmt, 2);
			fprintf(f, "%llu\t%s\t%s%s", id, host, service, Storage::NEWLINE);
		}
		Storage::closeStream(f);
		finalize(stmt);
	} else {
		throw Exception(EX_RESOURCE);
	}
}

int Host::getHost(unsigned long long uid, nameInfo &ni) noexcept {
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
		} else if (z == SQLITE_DONE) {
			//No record found
			ret = 1;
		} else {
			ret = -1;
		}
		//Reset immediately to free the resources
		reset(qStmt);
		return ret;
	} else {
		return -1;
	}
}

int Host::addHost(unsigned long long uid, const nameInfo &ni) noexcept {
	if (db && iStmt) {
		int ret = 0;
		sqlite3_bind_int64(iStmt, 1, uid);
		sqlite3_bind_text(iStmt, 2, ni.host, strlen(ni.host), SQLITE_STATIC);
		sqlite3_bind_text(iStmt, 3, ni.service, strlen(ni.service),
		SQLITE_STATIC);
		if (sqlite3_step(iStmt) != SQLITE_DONE) {
			ret = -1;
		}
		//Reset immediately to free the resources
		reset(iStmt);
		return ret;
	} else {
		return -1;
	}
}

int Host::removeHost(unsigned long long uid) noexcept {
	if (db && dStmt) {
		int ret = 0;
		sqlite3_bind_int64(dStmt, 1, uid);
		if (sqlite3_step(dStmt) != SQLITE_DONE) {
			ret = -1;
		}
		//Reset immediately to free the resources
		reset(dStmt);
		return ret;
	} else {
		return -1;
	}
}

void Host::clear() noexcept {
	finalize(iStmt);
	finalize(qStmt);
	finalize(dStmt);
	closeConnection(db);
	db = nullptr;
	iStmt = qStmt = dStmt = nullptr;
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
		//hosts table has been found
	}
}

void Host::prepareStatements() {
	const char *iq = "INSERT INTO hosts (uid, name, service) VALUES (?,?,?)";
	const char *sq = "SELECT name, service FROM hosts where uid=?";
	const char *dq = "DELETE FROM hosts where uid=?";

	if ((sqlite3_prepare_v2(db, iq, strlen(iq), &iStmt, nullptr) != SQLITE_OK)
			|| (sqlite3_prepare_v2(db, sq, strlen(sq), &qStmt, nullptr)
					!= SQLITE_OK)
			|| (sqlite3_prepare_v2(db, dq, strlen(dq), &dStmt, nullptr)
					!= SQLITE_OK)) {
		finalize(iStmt);
		finalize(qStmt);
		finalize(dStmt);
		iStmt = qStmt = dStmt = nullptr;
		WH_LOG_DEBUG("Could not create Prepared Statements");
		throw Exception(EX_INVALIDSTATE);
	} else {
		//To make sure that everything is in place
		reset(iStmt);
		reset(qStmt);
		reset(dStmt);
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
