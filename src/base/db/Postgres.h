/*
 * Postgres.h
 *
 * PostgreSQL connection manager
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DB_POSTGRES_H_
#define WH_BASE_DB_POSTGRES_H_
#include "../common/NonCopyable.h"
#include <postgresql/libpq-fe.h>

namespace wanhive {
/**
 * Connection parameters.
 */
struct PGInfo {
	/*! Connection parameters as nul-terminated string */
	const char *name { nullptr };
	/*! Connection parameters as keywords and values */
	struct {
		/*! NULL-terminated keywords */
		const char *keys[64] { };
		/*! NULL-terminated values */
		const char *values[64] { };
		/*! Controls 'dbname' parameter's expansion */
		bool expand { false };
	} ctx;
	/*! Controls blocking/non-blocking operation */
	bool blocking { true };
};

/**
 * Connection health.
 */
enum class PGHealth {
	BAD, /**< Failed or invalid connection */
	OK, /**< Connection in progress */
	READY, /**< Ready to use */
};

/**
 * Polling type.
 */
enum class PGPoll {
	CONNECT,/**< New connection attempt */
	RESET /**< Reset attempt */
};

/**
 * PostgreSQL connection manager
 * @ref https://www.postgresql.org/docs/current/libpq.html
 */
class Postgres: private NonCopyable {
public:
	/**
	 * Constructor
	 */
	Postgres() noexcept;
	/**
	 * Destructor
	 */
	~Postgres();
	/**
	 * Opens a new database connection.
	 * @param info connection parameters
	 * @return database connection handle (nullptr on error)
	 */
	static PGconn* connect(const PGInfo &info) noexcept;
	/**
	 * Opens a new database connection.
	 * @param name connection parameters string
	 * @param blocking true for blocking operation, false otherwise
	 * @return database connection handle (nullptr on error)
	 */
	static PGconn* connect(const char *name, bool blocking) noexcept;
	/**
	 * Opens a new database connection.
	 * @param keys null terminated array of keywords
	 * @param values null terminated array of values
	 * @param expand true to expand the 'dbname' keyword, false otherwise
	 * @param blocking true for blocking operation, false otherwise
	 * @return database connection handle (nullptr on error)
	 */
	static PGconn* connect(const char *const keys[], const char *const values[],
			bool expand, bool blocking) noexcept;
	/**
	 * Reads database connection's health.
	 * @param conn database connection handle
	 * @return health code
	 */
	static PGHealth health(const PGconn *conn) noexcept;
	/**
	 * Closes a database connection and frees its resources. A closed connection
	 * should never be reused. This function is nullptr-safe.
	 * @param conn database connection handle
	 */
	static void disconnect(PGconn *conn) noexcept;
	/**
	 * Attempts to re-establish a database connection.
	 * @param conn database connection handle
	 * @param blocking true for blocking operation, false otherwise
	 * @return true on success, false on error
	 */
	static bool reconnect(PGconn *conn, bool blocking) noexcept;
	/**
	 * Checks connection parameters and database server's status.
	 * @param info connection parameters
	 * @return true if connection accepted, false otherwise
	 */
	static bool ping(const PGInfo &info) noexcept;
	/**
	 * Checks connection parameters and database server's status.
	 * @param name connection parameters string
	 * @return true if connection accepted, false otherwise
	 */
	static bool ping(const char *name) noexcept;
	/**
	 * Checks connection parameters and database server's status.
	 * @param keys null terminated array of keywords
	 * @param values null terminated array of values
	 * @param expand true to expand the 'dbname' keyword, false otherwise
	 * @return true if connection accepted, false otherwise
	 */
	static bool ping(const char *const keys[], const char *const values[],
			bool expand) noexcept;
	/**
	 * Polls database connection's status.
	 * @param conn database connection handle
	 * @param type polling type
	 * @return health code
	 */
	static PGHealth poll(PGconn *conn, PGPoll type) noexcept;
};

} /* namespace wanhive */

#endif /* WH_BASE_DB_POSTGRES_H_ */
