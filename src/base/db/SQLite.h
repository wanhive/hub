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

namespace wanhive {

/**
 * Transaction stages.
 */
enum class SQLiteUnit {
	BEGIN, /**< Start a transaction */
	COMMIT, /**< Finalize a transaction */
	ROLLBACK/**< Undo a transaction */
};

/**
 * SQLite database manager
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
	 * @param fixed true for read-only, false otherwise
	 */
	SQLite(const char *path, bool fixed);
	/**
	 * Destructor: closes the database.
	 */
	~SQLite();
	/**
	 * Opens the database, closing any current one.
	 * @param path database file path
	 * @param fixed true for read-only, false otherwise
	 */
	void open(const char *path, bool fixed);
	/**
	 * Closes the database.
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
	/**
	 * Evaluates a prepared statement.
	 * @param stmt prepared statement
	 * @return error code
	 */
	int step(sqlite3_stmt *stmt) noexcept;
	/**
	 * Resets a prepared statement to its initial state for re-execution.
	 * @param stmt prepared statement
	 * @return error code
	 */
	int reset(sqlite3_stmt *stmt) noexcept;
	/**
	 * Resets the bindings on a prepared statement.
	 * @param stmt prepared statement
	 * @return error code
	 */
	int unbind(sqlite3_stmt *stmt) noexcept;
	/**
	 * Closes a prepared statement.
	 * @param stmt prepared statement
	 * @return error code
	 */
	int finalize(sqlite3_stmt *stmt) noexcept;
	/**
	 * Executes a transaction.
	 * @param stage transaction stage
	 */
	void transact(SQLiteUnit stage);
protected:
	/**
	 * Returns the database connection handle.
	 * @return opaque database connection object
	 */
	sqlite3* getHandle() noexcept;
	/**
	 * Returns the database connection handle.
	 * @return opaque database connection object
	 */
	const sqlite3* getHandle() const noexcept;
private:
	void transact(const char *stage);
public:
	static constexpr const char *IN_MEMORY = ":memory:";
private:
	sqlite3 *db { nullptr };
};

} /* namespace wanhive */

#endif /* WH_BASE_DB_SQLITE_H_ */
