/*
 * DataStore.h
 *
 * Structured data repository
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DB_DATASTORE_H_
#define WH_BASE_DB_DATASTORE_H_
#include "Postgres.h"

namespace wanhive {
/**
 * Database connection parameters.
 */
using DBInfo = PGInfo;
/**
 * Database connection health.
 */
using DBHealth = PGHealth;

/**
 * PostgreSQL-based structured data repository
 */
class DataStore: private NonCopyable {
public:
	/**
	 * Constructor: creates an empty repository.
	 */
	DataStore() noexcept;
	/**
	 * Constructor: connects to a database.
	 * @param info connection parameters
	 */
	DataStore(const DBInfo &info);
	/**
	 * Destructor: closes the database.
	 */
	~DataStore();
	/**
	 * Opens a database connection.
	 * @param info connection parameters
	 */
	void open(const DBInfo &info);
	/**
	 * Re-establishes a database connection.
	 * @param blocking true for blocking operation, false otherwise
	 */
	void reset(bool blocking = true);
	/**
	 * Closes the database connection.
	 */
	void close() noexcept;
	/**
	 * Polls database connection's status.
	 * @param timeout wait period in milliseconds. Set to -1 (default) to block
	 * indefinitely, set to zero (0) for non-blocking operation.
	 * @return
	 */
	bool poll(int timeout = -1);
	/**
	 * Probes database connection's health.
	 * @return health code
	 */
	DBHealth health() const noexcept;
	/**
	 * Checks connection parameters and database server's status.
	 * @param info connection parameters
	 * @return true on success, false on error
	 */
	static bool ping(const DBInfo &info) noexcept;
protected:
	/**
	 * Returns the database connection handle.
	 * @return database connection
	 */
	PGconn* connection() const noexcept;
private:
	struct {
		PGPoll poll { PGPoll::CONNECT };
		PGconn *conn { };
	} db;
};

} /* namespace wanhive */

#endif /* WH_BASE_DB_DATASTORE_H_ */
