/*
 * Watcher.cpp
 *
 * Resource descriptor and request handler
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Watcher.h"

namespace wanhive {

Watcher::Watcher() noexcept :
		Descriptor() {
}

Watcher::Watcher(int fd) noexcept :
		Descriptor(fd) {
}

Watcher::~Watcher() {

}

void Watcher::setTopic(unsigned int index) noexcept {

}

void Watcher::clearTopic(unsigned int index) noexcept {

}

bool Watcher::testTopic(unsigned int index) const noexcept {
	return false;
}

unsigned long long Watcher::getOption(int name) const noexcept {
	return 0;
}

void Watcher::setOption(int name, unsigned long long value) noexcept {

}

bool Watcher::isReady() const noexcept {
	return Descriptor::isReady(testFlags(WATCHER_OUT));
}

WatcherHandle Watcher::get() const noexcept {
	return WatcherHandle { Descriptor::get() };
}

} /* namespace wanhive */
