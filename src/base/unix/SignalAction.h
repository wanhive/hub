/*
 * SignalAction.h
 *
 * Signal action/disposition
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_SIGNALACTION_H_
#define WH_BASE_UNIX_SIGNALACTION_H_
#include "SignalSet.h"

namespace wanhive {
/**
 * Signal action/disposition
 * REF: signal(7)
 */
class SignalAction {
public:
	//If <restart> is true then the SA_RESTART flag is automatically set
	SignalAction(bool restart = true) noexcept;
	~SignalAction();

	//Loads the default disposition
	void reset() noexcept;
	//Configures the signal handler
	void setHandler(void (*handler)(int)) noexcept;
	//Configures the signal handler
	void setHandler(void (*handler)(int, siginfo_t*, void*)) noexcept;

	//Default action
	bool isDefault() const noexcept;
	//Ignore signal
	bool isIgnored() const noexcept;

	//Direct reference to the additional signal mask
	sigset_t& mask() noexcept;
	//Const reference to the additional signal mask
	const sigset_t& mask() const noexcept;

	//Returns the signal handling flags
	int getFlags() const noexcept;
	//Configures the signal handling flags
	void setFlags(int flags) noexcept;

	//Installs this signal action for the signal <signum>
	void install(int signum);
	//Installs this action for the signal <signum> and returns the <old> one
	void install(int signum, SignalAction &old);
	//Fetches the current signal action for <signum>
	void fetch(int signum);

	//Direct pointer to the internal structure
	struct sigaction* action() noexcept;
	//Const pointer to the internal structure
	const struct sigaction* action() const noexcept;

	//Wrapper for sigaction(2): examines or changes a signal action
	static void update(int signum, const struct sigaction *act,
			struct sigaction *old);

	//Dummy signal handler function which does absolutely nothing
	static void dummy(int signum) noexcept;
	//Dummy signal handler function which does absolutely nothing
	static void dummy(int, siginfo_t *si, void *arg) noexcept;
private:
	//Fixes the signal action flags to match the additional settings
	void fixFlags() noexcept;
private:
	struct sigaction _action;
	const bool _restart;
	bool _info;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_SIGNALACTION_H_ */
