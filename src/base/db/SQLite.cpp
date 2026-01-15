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

namespace {

auto disposal(wanhive::SQLiteScope scope) noexcept {
	switch (scope) {
	case wanhive::SQLiteScope::STATIC:
		return SQLITE_STATIC;
	default:
		return SQLITE_TRANSIENT;
	}
}

}  // namespace

namespace wanhive {

SQLite::SQLite() noexcept {

}

SQLite::SQLite(const char *path, int flags) {
	open(path, flags);
}

SQLite::~SQLite() {
	close();
}

void SQLite::open(const char *path, int flags) {
	close();
	sqlite3 *conn { };
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
	if (!db || !sql) {
		throw Exception(EX_NULL);
	}

	sqlite3_stmt *stmt { nullptr };
	if (sqlite3_prepare_v2(db, sql, bytes, &stmt, nullptr) == SQLITE_OK) {
		return stmt;
	} else {
		finalize(stmt);
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

void SQLite::transact(SQLiteStage stage) {
	const char *ts { nullptr };
	switch (stage) {
	case SQLiteStage::BEGIN:
		ts = "BEGIN";
		break;
	case SQLiteStage::COMMIT:
		ts = "COMMIT";
		break;
	case SQLiteStage::ROLLBACK:
		ts = "ROLLBACK";
		break;
	default:
		break;
	}

	execute(ts);
}

bool SQLite::execute(const char *sql, SQLiteHandler handler, void *arg) {
	if (!db || !sql) {
		throw Exception(EX_NULL);
	}

	auto status = sqlite3_exec(db, sql, handler, arg, nullptr);
	if (status == SQLITE_OK) {
		return true;
	} else if (status == SQLITE_ABORT) {
		return false;
	} else {
		throw Exception(EX_OPERATION);
	}
}

bool SQLite::execute(const char *sql) {
	return execute(sql, nullptr, nullptr);
}

sqlite3* SQLite::database() const noexcept {
	return db;
}

int SQLite::bindBlob(sqlite3_stmt *stmt, int index, const void *blob,
		size_t bytes, SQLiteScope scope) noexcept {
	return sqlite3_bind_blob64(stmt, index, blob, bytes, disposal(scope));
}

int SQLite::bindText(sqlite3_stmt *stmt, int index, const char *text,
		size_t bytes, SQLiteScope scope) noexcept {
	return sqlite3_bind_text64(stmt, index, text, bytes, disposal(scope),
	SQLITE_UTF8);
}

int SQLite::bindInteger(sqlite3_stmt *stmt, int index, int value) noexcept {
	return sqlite3_bind_int(stmt, index, value);
}

int SQLite::bindLongInteger(sqlite3_stmt *stmt, int index,
		long long value) noexcept {
	return sqlite3_bind_int64(stmt, index, value);
}

int SQLite::bindDouble(sqlite3_stmt *stmt, int index, double value) noexcept {
	return sqlite3_bind_double(stmt, index, value);
}

int SQLite::bindNull(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_bind_null(stmt, index);
}

int SQLite::bindZeros(sqlite3_stmt *stmt, int index, size_t count) noexcept {
	return sqlite3_bind_zeroblob64(stmt, index, count);
}

int SQLite::getType(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_column_type(stmt, index);
}

int SQLite::getSize(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_column_bytes(stmt, index);
}

const void* SQLite::getBlob(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_column_blob(stmt, index);
}

const unsigned char* SQLite::getText(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_column_text(stmt, index);
}

int SQLite::getInteger(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_column_int(stmt, index);
}

long long SQLite::getLongInteger(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_column_int64(stmt, index);
}

double SQLite::getDouble(sqlite3_stmt *stmt, int index) noexcept {
	return sqlite3_column_double(stmt, index);
}

} /* namespace wanhive */
