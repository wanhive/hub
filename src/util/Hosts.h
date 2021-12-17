/*
 * Hosts.h
 *
 * Database of Wanhive hosts
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
 * Database of Wanhive hosts (uses SQLite 3)
 * Thread safe at class level
 */
class Hosts {
public:
	Hosts() noexcept;
	//Opens the hosts database at <path>
	Hosts(const char *path, bool readOnly = false);
	~Hosts();

	//Opens the hosts database at <path>
	void open(const char *path, bool readOnly = false);
	//Imports the hosts database from a TAB-delimited text file
	void batchUpdate(const char *path);
	//Exports the hosts database to a TAB-delimited text file
	void batchDump(const char *path, int version = 1);
	/*
	 * Retrieves network address of the host <uid>.
	 * Returns 0 on success;1 if no record found; -1 on error.
	 */
	int get(unsigned long long uid, NameInfo &ni) noexcept;
	/*
	 * Adds the host <uid> to the database if doesn't exist and associates
	 * it with the network address <ni>. Returns 0 on success, -1 on error.
	 */
	int put(unsigned long long uid, const NameInfo &ni) noexcept;
	/*
	 * Removes the details of the host <uid> from the database.
	 * Returns 0 on success, -1 on error.
	 */
	int remove(unsigned long long uid) noexcept;
	/*
	 * Copies a random list of host identifiers of a given <type> into <uids>.
	 * At most <count> identifiers are copied and the actual number of elements
	 * transferred is returned via <count>. Returns 0 on success, -1 on error.
	 */
	int list(unsigned long long uids[], unsigned int &count, int type) noexcept;
	/*
	 * Generates a dummy hosts file
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
	//Enumeration of the special host types
	enum : int {
		BOOTSTRAP = 1, AUTHENTICATOR = 2
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
