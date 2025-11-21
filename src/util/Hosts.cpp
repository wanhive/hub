/*
 * Hosts.cpp
 *
 * Hosts database
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Hosts.h"
#include "../base/common/Exception.h"
#include <cstring>

namespace {
/*
 * Helper functions
 */
static void writeTuple(FILE *f, unsigned long long uid, const char *host,
		const char *service) noexcept {
	fprintf(f, "%llu\t%s\t%s\n", uid, host, service);
}

static void writeTuple(FILE *f, unsigned long long uid, const char *host,
		const char *service, int type) noexcept {
	fprintf(f, "%llu\t%s\t%s\t%d\n", uid, host, service, type);
}

static void writeHeading(FILE *f, int version) noexcept {
	if (version == 1) {
		fprintf(f, "# Revision: %d\n", version);
		fprintf(f, "# UID\tHOSTNAME\tSERVICE\tTYPE\n");
	}
}

}  // namespace

namespace wanhive {

Hosts::Hosts() noexcept {

}

Hosts::Hosts(const char *path, bool readOnly) {
	open(path, readOnly);
}

Hosts::~Hosts() {
	close();
}

void Hosts::open(const char *path, bool readOnly) {
	try {
		auto flags = readOnly ? SQLite::READ_ONLY : SQLite::RW_CREATE;
		SQLite::open(path, flags);
		closeStatements();
		if (!readOnly) {
			createTable();
		}
		prepareStatements();
	} catch (const BaseException &e) {
		close();
		throw;
	}
}

void Hosts::close() noexcept {
	closeStatements();
	SQLite::close();
}

void Hosts::load(const char *path) {
	if (Storage::testFile(path) != 1) {
		throw Exception(EX_RESOURCE);
	}

	auto f = Storage::openStream(path, "r");
	if (!f) {
		throw Exception(EX_ARGUMENT);
	}

	try {
		SQLite::transact(SQLiteStage::BEGIN);
		//-----------------------------------------------------------------
		char line[2048];
		char format[64];
		NameInfo ni;
		snprintf(format, sizeof(format), " %%llu %%%zus %%%zus %%d ",
				(sizeof(ni.host) - 1), (sizeof(ni.service) - 1));
		while (fgets(line, sizeof(line), f)) {
			unsigned long long id = 0;
			ni.type = 0;
			if (sscanf(line, format, &id, ni.host, ni.service, &ni.type) < 3) {
				continue;
			} else if (put(id, ni) == 0) {
				continue;
			} else {
				break;
			}
		}
		Storage::closeStream(f);
		//-----------------------------------------------------------------
		SQLite::transact(SQLiteStage::COMMIT);
	} catch (const BaseException &e) {
		Storage::closeStream(f);
		throw;
	}
}

void Hosts::dump(const char *path, int version) {
	auto query = "SELECT uid, name, service, type FROM hosts ORDER BY uid ASC";
	auto stmt = SQLite::prepare(query);

	auto f = Storage::openStream(path, "w");
	if (!f) {
		SQLite::finalize(stmt);
		throw Exception(EX_ARGUMENT);
	}

	writeHeading(f, version);
	while (SQLite::step(stmt) == SQLITE_ROW) {
		auto id = (unsigned long long) SQLite::columnLongInteger(stmt, 0);
		auto host = (const char*) SQLite::columnText(stmt, 1);
		auto service = (const char*) SQLite::columnText(stmt, 2);
		auto type = SQLite::columnInteger(stmt, 3);
		if (version == 1) {
			writeTuple(f, id, host, service, type);
		} else {
			writeTuple(f, id, host, service);
		}
	}
	Storage::closeStream(f);
	SQLite::finalize(stmt);
}

int Hosts::get(unsigned long long uid, NameInfo &ni) noexcept {
	if (!stmt.pq) {
		return -1;
	}

	int ret = 0;
	int z;
	memset(&ni, 0, sizeof(ni));
	SQLite::bindLongInteger(stmt.pq, 1, uid);
	if ((z = SQLite::step(stmt.pq)) == SQLITE_ROW) {
		strncpy(ni.host, (const char*) SQLite::columnText(stmt.pq, 0),
				sizeof(ni.host) - 1);
		strncpy(ni.service, (const char*) SQLite::columnText(stmt.pq, 1),
				sizeof(ni.service) - 1);
		ni.type = SQLite::columnInteger(stmt.pq, 2);
	} else if (z == SQLITE_DONE) {
		//No record found
		ret = 1;
	} else {
		ret = -1;
	}
	SQLite::reset(stmt.pq);
	SQLite::unbind(stmt.pq);
	return ret;
}

int Hosts::put(unsigned long long uid, const NameInfo &ni) noexcept {
	if (!stmt.pi) {
		return -1;
	}

	int ret = 0;
	SQLite::bindLongInteger(stmt.pi, 1, uid);
	SQLite::bindText(stmt.pi, 2, ni.host, strlen(ni.host), SQLiteScope::STATIC);
	SQLite::bindText(stmt.pi, 3, ni.service, strlen(ni.service),
			SQLiteScope::STATIC);
	SQLite::bindInteger(stmt.pi, 4, ni.type);
	if (SQLite::step(stmt.pi) != SQLITE_DONE) {
		ret = -1;
	}
	SQLite::reset(stmt.pi);
	SQLite::unbind(stmt.pi);
	return ret;
}

int Hosts::remove(unsigned long long uid) noexcept {
	if (!stmt.pd) {
		return -1;
	}

	int ret = 0;
	SQLite::bindLongInteger(stmt.pd, 1, uid);
	if (SQLite::step(stmt.pd) != SQLITE_DONE) {
		ret = -1;
	}
	SQLite::reset(stmt.pd);
	SQLite::unbind(stmt.pd);
	return ret;
}

int Hosts::list(unsigned long long uids[], unsigned int &count,
		int type) noexcept {
	if (count == 0) {
		return 0;
	}

	if (!uids || !stmt.pl) {
		count = 0;
		return -1;
	}

	SQLite::bindInteger(stmt.pl, 1, type);
	SQLite::bindInteger(stmt.pl, 2, count);
	unsigned int x = 0;
	int z = 0;
	while ((x < count) && ((z = SQLite::step(stmt.pl)) == SQLITE_ROW)) {
		uids[x++] = SQLite::columnLongInteger(stmt.pl, 0);
	}
	SQLite::reset(stmt.pl);
	SQLite::unbind(stmt.pl);
	count = x;
	if (z == SQLITE_ROW || z == SQLITE_DONE) {
		return 0;
	} else {
		return -1;
	}
}

void Hosts::dummy(const char *path, int version) {
	auto f = Storage::openStream(path, "w");
	if (f) {
		auto host = "127.0.0.1";
		char service[32];
		writeHeading(f, version);
		for (unsigned long long id = 0; id < 256; id++) {
			unsigned int port = (9001 + id);
			snprintf(service, sizeof(service), "%d", port);
			if (version == 1) {
				writeTuple(f, id, host, service, 0);
			} else {
				writeTuple(f, id, host, service);
			}
		}
		Storage::closeStream(f);
	} else {
		throw Exception(EX_ARGUMENT);
	}
}

void Hosts::createTable() {
	auto tq = "CREATE TABLE IF NOT EXISTS hosts ("
			"uid INTEGER NOT NULL UNIQUE ON CONFLICT REPLACE,"
			"name TEXT NOT NULL DEFAULT '127.0.0.1',"
			"service TEXT NOT NULL DEFAULT '9000',"
			"type INTEGER NOT NULL DEFAULT 0)";
	SQLite::execute(tq);
}

void Hosts::prepareStatements() {
	auto iq = "INSERT INTO hosts (uid, name, service, type) VALUES (?,?,?,?)";
	auto sq = "SELECT name, service, type FROM hosts WHERE uid=?";
	auto dq = "DELETE FROM hosts WHERE uid=?";
	auto lq = "SELECT uid FROM hosts WHERE type=? ORDER BY RANDOM() LIMIT ?";

	try {
		closeStatements();
		stmt.pi = SQLite::prepare(iq);
		stmt.pq = SQLite::prepare(sq);
		stmt.pd = SQLite::prepare(dq);
		stmt.pl = SQLite::prepare(lq);
		resetStatements();
	} catch (...) {
		closeStatements();
		throw Exception(EX_OPERATION);
	}
}

void Hosts::resetStatements() noexcept {
	SQLite::reset(stmt.pi);
	SQLite::unbind(stmt.pi);
	SQLite::reset(stmt.pq);
	SQLite::unbind(stmt.pq);
	SQLite::reset(stmt.pd);
	SQLite::unbind(stmt.pd);
	SQLite::reset(stmt.pl);
	SQLite::unbind(stmt.pl);
}

void Hosts::closeStatements() noexcept {
	SQLite::finalize(stmt.pi);
	SQLite::finalize(stmt.pq);
	SQLite::finalize(stmt.pd);
	SQLite::finalize(stmt.pl);
	stmt = { nullptr, nullptr, nullptr, nullptr };
}

} /* namespace wanhive */
