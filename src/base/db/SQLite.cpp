/*
 * SQLite.cpp
 *
 * SQLite database manager
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "SQLite.h"
#include "../common/Exception.h"

namespace wanhive {

SQLite::SQLite() noexcept {

}

SQLite::SQLite(const char *path, bool fixed) {
	open(path, fixed);
}

SQLite::~SQLite() {
	close();
}

void SQLite::open(const char *path, bool fixed) {
	close();
	int flags =
			fixed ? SQLITE_OPEN_READONLY : (SQLITE_OPEN_READWRITE
							| SQLITE_OPEN_CREATE);
	sqlite3 *conn = nullptr;
	if (sqlite3_open_v2(path, &conn, flags, nullptr) != SQLITE_OK) {
		sqlite3_close_v2(conn);
		throw Exception(EX_OPERATION);
	} else {
		db = conn;
	}
}

void SQLite::close() noexcept {
	sqlite3_close_v2(db);
	db = nullptr;
}

sqlite3_stmt* SQLite::prepare(const char *sql, int bytes) {
	sqlite3_stmt *stmt { nullptr };
	if (db && sqlite3_prepare_v2(db, sql, bytes, &stmt, nullptr) == SQLITE_OK) {
		return stmt;
	} else {
		throw Exception(EX_OPERATION);
	}
}

int SQLite::step(sqlite3_stmt *stmt) noexcept {
	if (stmt) {
		return sqlite3_step(stmt);
	} else {
		return SQLITE_ERROR;
	}
}

int SQLite::reset(sqlite3_stmt *stmt) noexcept {
	if (stmt) {
		return sqlite3_reset(stmt);
	} else {
		return SQLITE_ERROR;
	}
}

int SQLite::unbind(sqlite3_stmt *stmt) noexcept {
	if (stmt) {
		return sqlite3_clear_bindings(stmt);
	} else {
		return SQLITE_ERROR;
	}
}

int SQLite::finalize(sqlite3_stmt *stmt) noexcept {
	return sqlite3_finalize(stmt);
}

void SQLite::transact(SQLiteUnit stage) {
	const char *ts { nullptr };
	switch (stage) {
	case SQLiteUnit::BEGIN:
		ts = "BEGIN";
		break;
	case SQLiteUnit::COMMIT:
		ts = "COMMIT";
		break;
	case SQLiteUnit::ROLLBACK:
		ts = "ROLLBACK";
		break;
	default:
		break;
	}

	transact(ts);
}

sqlite3* SQLite::getHandle() noexcept {
	return db;
}

const sqlite3* SQLite::getHandle() const noexcept {
	return db;
}

void SQLite::transact(const char *stage) {
	if (!db || !stage) {
		throw Exception(EX_NULL);
	} else if (sqlite3_exec(db, stage, nullptr, nullptr, nullptr) != SQLITE_OK) {
		throw Exception(EX_OPERATION);
	} else {
		return;
	}
}

} /* namespace wanhive */
