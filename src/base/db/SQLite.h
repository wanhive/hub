/*
 * SQLite.h
 *
 * SQLite database manager
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DB_SQLITE_H_
#define WH_BASE_DB_SQLITE_H_
#include "../common/NonCopyable.h"
#include <sqlite3.h>
#include <cstddef>

namespace wanhive {

/**
 * Transaction stages.
 */
enum class SQLiteStage {
	BEGIN, /**< Start a transaction */
	COMMIT, /**< Finalize a transaction */
	ROLLBACK/**< Undo a transaction */
};

/**
 * Life cycle scopes.
 */
enum class SQLiteScope {
	STATIC, /**< STATIC */
	TRANSIENT/**< TRANSIENT */
};

/**
 * Callback function for the one-step query execution interface.
 * @ref https://sqlite.org/c3ref/exec.html
 */
using SQLiteHandler = int (*)(void*,int,char**,char**) noexcept;

/**
 * SQLite database manager
 * @ref https://sqlite.org/c3ref/intro.html
 */
class SQLite: private NonCopyable {
public:
	/**
	 * Constructor: creates a database manager.
	 */
	SQLite() noexcept;
	/**
	 * Constructor: opens a database.
	 * @param path database file path
	 * @param flags access mode flags
	 */
	SQLite(const char *path, int flags);
	/**
	 * Destructor: closes the database.
	 */
	~SQLite();
	//-----------------------------------------------------------------
	/**
	 * Opens a database, closing any current one.
	 * @param path database file path
	 * @param flags access mode flags
	 */
	void open(const char *path, int flags);
	/**
	 * Closes a database.
	 */
	void close() noexcept;
	/**
	 * Constructs a prepared statement.
	 * @param sql statement to compile
	 * @param bytes number of bytes in the input string including the
	 * nul-terminator
	 * @return compiled prepared statement
	 */
	sqlite3_stmt* prepare(const char *sql, int bytes = -1);
	//-----------------------------------------------------------------
	/**
	 * Evaluates a prepared statement.
	 * @param stmt valid prepared statement
	 * @return error code
	 */
	int step(sqlite3_stmt *stmt) noexcept;
	/**
	 * Readies a prepared statement for re-execution.
	 * @param stmt valid prepared statement
	 * @return error code
	 */
	int reset(sqlite3_stmt *stmt) noexcept;
	/**
	 * Resets the bindings on a prepared statement.
	 * @param stmt valid prepared statement
	 * @return error code
	 */
	int unbind(sqlite3_stmt *stmt) noexcept;
	/**
	 * Closes a prepared statement and frees its resources. A closed statement
	 * should never be reused. This function is nullptr-safe.
	 * @param stmt prepared statement
	 * @return error code
	 */
	int finalize(sqlite3_stmt *stmt) noexcept;
	/**
	 * Executes a transaction.
	 * @param stage transaction stage
	 */
	void transact(SQLiteStage stage);
	/**
	 * One-step query executor.
	 * @param sql semicolon-separated sql statements
	 * @param handler callback function
	 * @param arg additional argument for the callback function
	 * @return true on success, false if callback routine requested an abort
	 */
	bool execute(const char *sql, SQLiteHandler handler, void *arg);
	/**
	 * One-step query executor.
	 * @param sql semicolon-separated sql statements
	 * @return always true
	 */
	bool execute(const char *sql);
protected:
	//-----------------------------------------------------------------
	/**
	 * Returns the database connection handle.
	 * @return opaque database connection object
	 */
	sqlite3* getHandle() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Binds a blob to the prepared statement.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @param blob value to bind
	 * @param bytes number of bytes
	 * @param scope life cycle scope
	 * @return status code
	 */
	static int bindBlob(sqlite3_stmt *stmt, int index, const void *blob,
			size_t bytes, SQLiteScope scope) noexcept;
	/**
	 * Binds a UTF-8 encoded text to the prepared statement.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @param text value to bind
	 * @param bytes string length in bytes
	 * @param scope life cycle scope
	 * @return status code
	 */
	static int bindText(sqlite3_stmt *stmt, int index, const char *text,
			size_t bytes, SQLiteScope scope) noexcept;
	/**
	 * Binds an integer to the prepared statement.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @param value value to bind
	 * @return status code
	 */
	static int bindInteger(sqlite3_stmt *stmt, int index, int value) noexcept;
	/**
	 * Binds a 64-bit integer to the prepared statement.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @param value value to bind
	 * @return status code
	 */
	static int bindLongInteger(sqlite3_stmt *stmt, int index,
			long long value) noexcept;
	/**
	 * Binds a double value to the prepared statement.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @param value value to bind
	 * @return status code
	 */
	static int bindDouble(sqlite3_stmt *stmt, int index, double value) noexcept;
	/**
	 * Binds null value to the prepared statement.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @return status code
	 */
	static int bindNull(sqlite3_stmt *stmt, int index) noexcept;
	/**
	 * Binds a zero-filled blob to the prepared statement.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @param count number of zeroes
	 * @return status code
	 */
	static int bindZeroes(sqlite3_stmt *stmt, int index, size_t count) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a query result's data type.
	 * @param stmt valid prepared statement
	 * @param index valid column index
	 * @return data type code
	 */
	static int columnType(sqlite3_stmt *stmt, int index) noexcept;
	/**
	 * Returns a blob or a text result's size in bytes.
	 * @param stmt valid prepared statement
	 * @param index valid index
	 * @return result size in bytes
	 */
	static int columnBytes(sqlite3_stmt *stmt, int index) noexcept;
	/**
	 * Returns the blob from a query's result column.
	 * @param stmt valid prepared statement
	 * @param index valid column index
	 * @return blob's pointer
	 */
	static const void* columnBlob(sqlite3_stmt *stmt, int index) noexcept;
	/**
	 * Returns the UTF-8 encoded text from a query's result column.
	 * @param stmt valid prepared statement
	 * @param index valid column index
	 * @return text's pointer
	 */
	static const unsigned char* columnText(sqlite3_stmt *stmt,
			int index) noexcept;
	/**
	 * Returns the integer value from a query's result column.
	 * @param stmt valid prepared statement
	 * @param index valid column index
	 * @return integer value
	 */
	static int columnInteger(sqlite3_stmt *stmt, int index) noexcept;
	/**
	 * Returns the 64-bit integer value from a query's result column.
	 * @param stmt valid prepared statement
	 * @param index valid column index
	 * @return 64-bit integer value
	 */
	static long long columnLongInteger(sqlite3_stmt *stmt, int index) noexcept;
	/**
	 * Returns the double value from a query's result column.
	 * @param stmt valid prepared statement
	 * @param index valid column index
	 * @return double value
	 */
	static double columnDouble(sqlite3_stmt *stmt, int index) noexcept;
public:
	/**
	 * Common access control flags
	 */
	enum AccessFlags {
		/*! Open in read-only mode */
		READ_ONLY = SQLITE_OPEN_READONLY,
		/*! Open for reading and writing if possible */
		RW_ONLY = SQLITE_OPEN_READWRITE,
		/*! Open for reading and writing; Create if doesn't exist */
		RW_CREATE = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)
	};

	/*! Special pathname for in-memory database */
	static constexpr const char *IN_MEMORY = ":memory:";
private:
	sqlite3 *db { nullptr };
};

} /* namespace wanhive */

#endif /* WH_BASE_DB_SQLITE_H_ */
