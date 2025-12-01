/*
 * DataStore.cpp
 *
 * Structured data repository
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "DataStore.h"
#include "../common/Exception.h"

namespace wanhive {

DataStore::DataStore() noexcept {

}

DataStore::DataStore(const DBInfo &info) {
	open(info);
}

DataStore::~DataStore() {
	close();
}

void DataStore::open(const DBInfo &info) {
	close();
	db.conn = Postgres::connect(info);
	if (!db.conn) {
		throw Exception(EX_RESOURCE);
	}
}

void DataStore::reset(bool blocking) {
	db.poll = PGPoll::RESET;
	if (!Postgres::reconnect(db.conn, blocking)) {
		throw Exception(EX_RESOURCE);
	}
}

void DataStore::close() noexcept {
	Postgres::disconnect(db.conn);
	db = { PGPoll::CONNECT, nullptr };
}

bool DataStore::poll() {
	auto trace = health();
	if (trace == DBHealth::OK) {
		trace = Postgres::poll(db.conn, db.poll);
		switch (trace) {
		case PGHealth::READY:
			return true;
		case PGHealth::OK:
			return false;
		default:
			throw Exception(EX_RESOURCE);
		}
	} else if (trace == DBHealth::READY) {
		return true;
	} else {
		throw Exception(EX_RESOURCE);
	}
}

DBHealth DataStore::health() const noexcept {
	return Postgres::health(db.conn);
}

bool DataStore::ping(const DBInfo &info) noexcept {
	return Postgres::ping(info);
}

PGconn* DataStore::getHandle() const noexcept {
	return db.conn;
}

} /* namespace wanhive */
