/*
 * Reactor.cpp
 *
 * Synchronous event demultiplexer and dispatcher
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Reactor.h"
#include "../base/common/audit.h"

namespace wanhive {

Reactor::Reactor() noexcept {

}

Reactor::Reactor(unsigned int maxEvents, bool signal) {
	initialize(maxEvents, signal);
}

Reactor::~Reactor() {

}

void Reactor::initialize(unsigned int maxEvents, bool signal) {
	timeout = -1;
	selector.initialize(maxEvents, signal);
}

void Reactor::add(Watcher *w, uint32_t events) {
	if (w && !w->testFlags(WATCHER_RUNNING)) {
		adapt(w);

		events |= (IO_CLOSE | TRIGGER_EDGE);
		selector.add(w->getHandle(), events, w);

		w->setFlags(WATCHER_RUNNING);
	} else {
		throw Exception(EX_ARGUMENT);
	}
}

void Reactor::modify(Watcher *w, uint32_t events) {
	if (w && w->testFlags(WATCHER_RUNNING)) {
		events |= (IO_CLOSE | TRIGGER_EDGE);
		selector.modify(w->getHandle(), events, w);
	} else {
		throw Exception(EX_ARGUMENT);
	}
}

bool Reactor::disable(Watcher *w) noexcept {
	if (!w || !w->testFlags(WATCHER_RUNNING)) {
		//Nothing to do
		return false;
	} else if (w->testFlags(WATCHER_READY)) {
		w->setFlags(WATCHER_INVALID);
		return true;
	} else {
		remove(w);
		return false;
	}
}

void Reactor::poll(bool block) {
	auto to = (readyList.isEmpty() && block) ? timeout : 0;
	selector.select(to);

	const SelectionEvent *se;
	while ((se = selector.next())) {
		Watcher *watcher = (Watcher*) Selector::attachment(se);
		watcher->setEvents(Selector::events(se));

		//An error occurred
		if (watcher->testEvents(IO_ERROR)) {
			watcher->setFlags(WATCHER_INVALID);
		}

		//Put the watcher into the ready list only if it can do some useful work
		if (watcher->isReady()) {
			retain(watcher);
		}
	}
}

void Reactor::dispatch() noexcept {
	//Need <count> because the list is modified inside the while loop
	auto count = readyList.readSpace();
	while (count--) { //postfix --
		Watcher *watcher = release();
		if (watcher->testFlags(WATCHER_INVALID)) {
			remove(watcher);
		} else if (react(watcher)) {
			retain(watcher);
		}
	}
}

void Reactor::retain(Watcher *w) noexcept {
	if (!w->testFlags(WATCHER_READY)) {
		w->setFlags(WATCHER_READY);
		readyList.put(w);
	}
}

bool Reactor::interrupted() const noexcept {
	return selector.interrupted();
}

bool Reactor::timedOut() const noexcept {
	return selector.timedOut();
}

int Reactor::getTimeout() const noexcept {
	return timeout;
}

void Reactor::setTimeout(int milliseconds) noexcept {
	this->timeout = milliseconds;
}

Watcher* Reactor::release() noexcept {
	Watcher *w = nullptr;
	if (readyList.get(w)) {
		w->clearFlags(WATCHER_READY);
	}
	return w;
}

void Reactor::remove(Watcher *w) noexcept {
	try {
		selector.remove(w->getHandle());
		w->clearFlags(WATCHER_RUNNING);
		stop(w);
	} catch (const BaseException &e) {
		WH_FATAL_ERROR(e.what());
	}
}

} /* namespace wanhive */
