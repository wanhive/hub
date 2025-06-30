/*
 * Logic.cpp
 *
 * Digital logic watcher
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Logic.h"
#include "Hub.h"

namespace wanhive {

Logic::Logic() noexcept {

}

Logic::~Logic() {

}

void Logic::start() {

}

void Logic::stop() noexcept {

}

bool Logic::callback(void *arg) noexcept {
	if (getReference() != nullptr) {
		Handler<Logic> *h = static_cast<Hub*>(getReference());
		return h->handle(this);
	} else {
		return false;
	}
}

bool Logic::publish(void *arg) noexcept {
	return false;
}

ssize_t Logic::update(LogicEvent &event) {
	return -1;
}

} /* namespace wanhive */
