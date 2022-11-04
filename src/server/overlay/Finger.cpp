/*
 * Finger.cpp
 *
 * Finger implementation for the Chord (distributed hash table)
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Finger.h"

namespace wanhive {

Finger::Finger() noexcept {
}

Finger::~Finger() {

}

unsigned int Finger::getStart() const noexcept {
	return start;
}

void Finger::setStart(unsigned int start) noexcept {
	this->start = start;
}

unsigned int Finger::getId() const noexcept {
	return id;
}

void Finger::setId(unsigned int id) noexcept {
	this->id = id;
}

unsigned int Finger::getOldId() const noexcept {
	return oldId;
}

void Finger::setOldId(unsigned int oldId) noexcept {
	this->oldId = oldId;
}

bool Finger::isConnected() const noexcept {
	return connected;
}

void Finger::setConnected(bool connected) noexcept {
	this->connected = connected;
}

bool Finger::isConsistent() const noexcept {
	return getId() == getOldId();
}

unsigned int Finger::commit() noexcept {
	auto old = getOldId();
	setOldId(getId());
	return old;
}

} /* namespace wanhive */
