/*
 * SignalAction.cpp
 *
 * Signal action/disposition
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "SignalAction.h"
#include "SystemException.h"
#include <cstring>

namespace wanhive {

SignalAction::SignalAction(bool restart) noexcept :
		_restart(restart) {
	reset();
}

SignalAction::~SignalAction() {

}

void SignalAction::reset() noexcept {
	::memset(&_action, 0, sizeof(_action));
	::sigemptyset(&_action.sa_mask);
	setHandler(SIG_DFL);
	setFlags(0);
}

void SignalAction::setHandler(void (*handler)(int)) noexcept {
	_action.sa_handler = handler;
	_info = false;
}

void SignalAction::setHandler(void (*handler)(int, siginfo_t*, void*)) noexcept {
	_action.sa_sigaction = handler;
	_info = true;
}

bool SignalAction::isDefault() const noexcept {
	return (_action.sa_handler == SIG_DFL);
}

bool SignalAction::isIgnored() const noexcept {
	return (_action.sa_handler == SIG_IGN);
}

sigset_t& SignalAction::mask() noexcept {
	return _action.sa_mask;
}

const sigset_t& SignalAction::mask() const noexcept {
	return _action.sa_mask;
}

int SignalAction::getFlags() const noexcept {
	return _action.sa_flags;
}

void SignalAction::setFlags(int flags) noexcept {
	_action.sa_flags = flags;
}

void SignalAction::install(int signum) {
	fixFlags();
	update(signum, &_action, nullptr);
}

void SignalAction::install(int signum, SignalAction &old) {
	fixFlags();
	update(signum, &_action, &old._action);
}

void SignalAction::fetch(int signum) {
	update(signum, nullptr, &_action);
	_info = (bool) (_action.sa_flags & SA_SIGINFO);
}

struct sigaction* SignalAction::action() noexcept {
	return &_action;
}

const struct sigaction* SignalAction::action() const noexcept {
	return &_action;
}

void SignalAction::update(int signum, const struct sigaction *act,
		struct sigaction *old) {
	if (::sigaction(signum, act, old) == -1) {
		throw SystemException();
	} else {
		return;
	}
}

void SignalAction::dummy(int signum) noexcept {

}

void SignalAction::dummy(int signum, siginfo_t *si, void *arg) noexcept {

}

void SignalAction::fixFlags() noexcept {
	_action.sa_flags |= (_restart ? SA_RESTART : 0);
	_action.sa_flags &= ~(SA_NODEFER | SA_RESETHAND);
	if (_info) {
		_action.sa_flags |= SA_SIGINFO;
	} else {
		_action.sa_flags &= ~SA_SIGINFO;
	}
}

} /* namespace wanhive */
