/*
 * Hosts.h
 *
 * The hosts database
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_HOSTS_H_
#define WH_UTIL_HOSTS_H_
#include "../base/Network.h"
#include "../base/Storage.h"
#include <sqlite3.h>

namespace wanhive {
/**
 * The hosts database
 * Thread safe at class level
 */
class Hosts {
public:
	/**
	 * Default constructor: doesn't create a database connection. Call
	 * Hosts::open() explicitly to open a database connection.
	 */
	Hosts() noexcept;
	/**
	 * Constructor: opens a new database connection
	 * @param path pathname of the database file
	 * @param readOnly true for opening the database in read-only mode,
	 * false otherwise.
	 */
	Hosts(const char *path, bool readOnly = false);
	/**
	 * Destructor: releases the resources
	 */
	~Hosts();

	/**
	 * Creates a new database connection after closing any existing one.
	 * @param path pathname of the database file
	 * @param readOnly true for opening the database in read-only mode,
	 * false otherwise.
	 */
	void open(const char *path, bool readOnly = false);
	//-----------------------------------------------------------------
	/**
	 * Imports hosts data from a TAB-delimited text file (hosts file).
	 * @param path pathname of the text file containing the hosts data
	 */
	void batchUpdate(const char *path);

	/**
	 * Exports the hosts database to a tab-delimited text file.
	 * @param path pathname of the text file, if a file with the given name
	 * doesn't exist then a new file will be created.
	 * @param version the output format specifier
	 */
	void batchDump(const char *path, int version = 1);
	//-----------------------------------------------------------------
	/**
	 * Retrieves the network address associated with the given host identifier.
	 * @param uid the host identifier
	 * @param ni object for storing the network address
	 * @return 0 on success, 1 if no record found, -1 on error
	 */
	int get(unsigned long long uid, NameInfo &ni) noexcept;
	/**
	 * Associates a network address to the given host identifier.
	 * @param uid the host identifier
	 * @param ni the network address
	 * @return 0 on success, -1 on error
	 */
	int put(unsigned long long uid, const NameInfo &ni) noexcept;
	/**
	 * Removes any record associated with the given host identifier.
	 * @param uid the host identifier
	 * @return 0 on success, -1 on error
	 */
	int remove(unsigned long long uid) noexcept;
	/**
	 * Returns a randomized list of host identifiers of the given type.
	 * @param uids the output array for storing the host identifiers
	 * @param count before the call it's value should be set to the maximum
	 * capacity of the output array. The actual number of elements transferred
	 * into the output array is written to this argument.
	 * @param type the desired host type
	 * @return 0 on success, -1 on error
	 */
	int list(unsigned long long uids[], unsigned int &count, int type) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates a dummy hosts file.
	 * @param path pathname of the hosts file, if a file with the given name
	 * doesn't exist then it will be created.
	 * @param version the output format specifier
	 */
	static void createDummy(const char *path, int version = 1);
private:
	void clear() noexcept;
	void openConnection(const char *path, bool readOnly);
	void closeConnection() noexcept;
	void createTable();
	void prepareStatements();
	void resetStatements() noexcept;
	void closeStatements() noexcept;
	void reset(sqlite3_stmt *stmt) noexcept;
	void finalize(sqlite3_stmt *stmt) noexcept;
	void beginTransaction();
	void endTransaction();
	void cancelTransaction();
public:
	/**
	 * Host type enumeration
	 */
	enum : int {
		BOOTSTRAP = 1, /**< Bootstrapping host */
		AUTHENTICATOR = 2/**< Authentication host */
	};
private:
	struct {
		sqlite3 *conn; //The database connection
		sqlite3_stmt *iStmt; //Insert
		sqlite3_stmt *qStmt; //Get
		sqlite3_stmt *dStmt; //Delete
		sqlite3_stmt *lStmt; //List
	} db;
};

} /* namespace wanhive */

#endif /* WH_UTIL_HOSTS_H_ */
