/*
 * Hosts.h
 *
 * Hosts database
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
#include "../base/db/SQLite.h"

namespace wanhive {
/**
 * Hosts database
 */
class Hosts: private SQLite {
public:
	/**
	 * Constructor: creates an empty database.
	 */
	Hosts() noexcept;
	/**
	 * Constructor: opens the hosts database.
	 * @param path database file's path
	 * @param readOnly true for read-only access, false otherwise
	 */
	Hosts(const char *path, bool readOnly = false);
	/**
	 * Destructor: releases the resources
	 */
	~Hosts();
	/**
	 * Opens the hosts database.
	 * @param path database file's path
	 * @param readOnly true for read-only access, false otherwise
	 */
	void open(const char *path, bool readOnly = false);
	/**
	 * Closes the hosts database.
	 */
	void close() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Imports data from a tab-delimited text file.
	 * @param path text file's path
	 */
	void load(const char *path);
	/**
	 * Exports data to a tab-delimited text file.
	 * @param path output file's path
	 * @param version output format
	 */
	void dump(const char *path, int version = 1);
	//-----------------------------------------------------------------
	/**
	 * Reads the network address associated with a host identifier.
	 * @param uid host identifier
	 * @param ni stores the network address
	 * @return 0 on success, 1 if no record found, -1 on error
	 */
	int get(unsigned long long uid, NameInfo &ni) noexcept;
	/**
	 * Adds a new host into the database. If the host identifier already then
	 * updates the network address.
	 * @param uid host identifier
	 * @param ni host's network address
	 * @return 0 on success, -1 on error
	 */
	int put(unsigned long long uid, const NameInfo &ni) noexcept;
	/**
	 * Removes a host associated with the given identifier.
	 * @param uid host identifier
	 * @return 0 on success, -1 on error
	 */
	int remove(unsigned long long uid) noexcept;
	/**
	 * Returns random host identifiers for the given type.
	 * @param uids buffer to store the host identifiers
	 * @param count buffer capacity for input, identifiers count as output
	 * @param type host type
	 * @return 0 on success, -1 on error
	 */
	int list(unsigned long long uids[], unsigned int &count, int type) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Generates a dummy hosts file.
	 * @param path hosts file's path
	 * @param version output format specifier
	 */
	static void dummy(const char *path, int version = 1);
private:
	void createTable();
	void prepareStatements();
	void resetStatements() noexcept;
	void closeStatements() noexcept;
public:
	using SQLite::IN_MEMORY;
	/**
	 * Host type enumeration
	 */
	enum : int {
		BOOTSTRAP = 1, /**< Bootstrapping host */
		AUTHENTICATOR = 2/**< Authentication host */
	};
private:
	struct {
		sqlite3_stmt *pi { nullptr }; //Insert
		sqlite3_stmt *pq { nullptr }; //Get
		sqlite3_stmt *pd { nullptr }; //Delete
		sqlite3_stmt *pl { nullptr }; //List
	} stmt;
};

} /* namespace wanhive */

#endif /* WH_UTIL_HOSTS_H_ */
