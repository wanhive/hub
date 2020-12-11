/*
 * Host.h
 *
 * Database of Wanhive hosts
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_HOST_H_
#define WH_UTIL_HOST_H_
#include "../base/Network.h"
#include "../base/Storage.h"
#include <sqlite3.h>

namespace wanhive {
/**
 * Database of Wanhive hosts (uses SQLite 3)
 * Thread safe at class level
 */
class Host {
public:
	Host() noexcept;
	//if <readOnly> is true then the database is loaded in read-only mode
	Host(const char *path, bool readOnly = false);
	~Host();

	//if <readOnly> is true then the database is loaded in read-only mode
	void load(const char *path, bool readOnly = false);
	//Load addresses into database from a TAB-delimited text file
	void batchUpdate(const char *path);
	//Dump database content into a TAB-delimited text file
	void batchDump(const char *path);
	/*
	 * Retrieves network address of the host <uid>. Returns 0 on success; 1 if
	 * no record found; -1 on error.
	 */
	int getHost(unsigned long long uid, NameInfo &ni) noexcept;
	/*
	 * Adds the host <uid> to the database if doesn't exist and associates
	 * it with the network address <ni>. Returns 0 on success, -1 on error.
	 */
	int addHost(unsigned long long uid, const NameInfo &ni) noexcept;
	/*
	 * Removes host <uid> from the database. Returns 0 on success, -1 on error.
	 */
	int removeHost(unsigned long long uid) noexcept;
	/*
	 * Randomly copies list of host identifiers of <type> into <uids>.
	 * At most <count> identifiers are copied and the actual number of elements
	 * transferred is returned via <count>. Returns 0 on success, -1 on error.
	 */
	int list(unsigned long long uids[], unsigned int &count, int type) noexcept;
private:
	void clear() noexcept;
	void openConnection(const char *path, bool readOnly);
	void closeConnection(sqlite3 *db) noexcept;
	void createTable();
	void prepareStatements();
	void reset(sqlite3_stmt *stmt) noexcept;
	void finalize(sqlite3_stmt *stmt) noexcept;
	void beginTransaction();
	void endTransaction();
	void deferTransaction();
private:
	sqlite3 *db;
	sqlite3_stmt *iStmt; //Insert
	sqlite3_stmt *qStmt; //Query
	sqlite3_stmt *dStmt; //Delete
	sqlite3_stmt *lStmt; //List
};

} /* namespace wanhive */

#endif /* WH_UTIL_HOST_H_ */
