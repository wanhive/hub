/*
 * State.cpp
 *
 * State machine base
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "State.h"
#include "../../base/ds/Twiddler.h"

namespace wanhive {
State::State() noexcept {
	clear();
	marked = false;
}

State::~State() {

}

void State::clear() noexcept {
	reference = nullptr;
	type = 0;
	flags = 0;
	events = 0;
	group = 0;
}

void* State::getReference() const noexcept {
	return reference;
}

void State::setReference(void *reference) noexcept {
	this->reference = reference;
}

int State::getType() const noexcept {
	return type;
}

bool State::isType(int type) const noexcept {
	return this->type == type;
}

void State::setType(int type) noexcept {
	this->type = type;
}

uint32_t State::getEvents() const noexcept {
	return events;
}

bool State::testEvents(uint32_t events) const noexcept {
	return Twiddler::test(this->events, events);
}

void State::putEvents(uint32_t events) noexcept {
	this->events = events;
}

void State::setEvents(uint32_t events) noexcept {
	this->events = Twiddler::set(this->events, events);
}

void State::clearEvents(uint32_t events) noexcept {
	this->events = Twiddler::clear(this->events, events);
}

void State::maskEvents(uint32_t events, bool set) noexcept {
	this->events = Twiddler::mask(this->events, events, set);
}

uint32_t State::getFlags() const noexcept {
	return flags;
}

bool State::testFlags(uint32_t flags) const noexcept {
	return Twiddler::test(this->flags, flags);
}

void State::putFlags(uint32_t flags) noexcept {
	this->flags = flags;
}

void State::setFlags(uint32_t flags) noexcept {
	this->flags = Twiddler::set(this->flags, flags);
}

void State::clearFlags(uint32_t flags) noexcept {
	this->flags = Twiddler::clear(this->flags, flags);
}

void State::maskFlags(uint32_t flags, bool set) noexcept {
	this->flags = Twiddler::mask(this->flags, flags, set);
}

unsigned char State::getGroup() const noexcept {
	return group;
}

bool State::testGroup(unsigned char group) const noexcept {
	return (this->group & group);
}

void State::setGroup(unsigned char group) noexcept {
	this->group = group;
}

bool State::isMarked() const noexcept {
	return marked;
}

void State::setMarked() noexcept {
	this->marked = true;
}

} /* namespace wanhive */
