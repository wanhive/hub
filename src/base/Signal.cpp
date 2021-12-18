/*
 * Signal.cpp
 *
 * Common signal handling routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Signal.h"
#include "unix/SignalAction.h"
#include "unix/TSignal.h"

namespace wanhive {

void Signal::block(int signum) {
	SignalSet ss(false);
	ss.add(signum);
	TSignal::setMask(SIG_BLOCK, ss);
}

void Signal::unblock(int signum) {
	SignalSet ss(false);
	ss.add(signum);
	TSignal::setMask(SIG_UNBLOCK, ss);
}

void Signal::blockAll() {
	SignalSet ss(true);
	TSignal::setMask(SIG_SETMASK, ss);
}

void Signal::unblockAll() {
	SignalSet ss(false);
	TSignal::setMask(SIG_SETMASK, ss);
}

void Signal::ignore(int signum) {
	SignalAction sa(true);
	sa.setHandler(SIG_IGN);
	sa.install(signum);
}

void Signal::reset(int signum) {
	SignalAction sa(false);
	sa.install(signum);
}

void Signal::handle(int signum, void (*handler)(int), bool restart) {
	if (!handler) {
		handler = SignalAction::dummy;
	}

	SignalAction sa(restart);
	sa.setHandler(handler);
	sa.install(signum);
}

} /* namespace wanhive */
