/**
 * @file SignalAction.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Signal action/disposition
 * @ref signal(7)
 */
class SignalAction {
public:
	/**
	 * Constructor: creates a default disposition.
	 * @param restart true to automatically resume an interrupted system call,
	 * false otherwise.
	 */
	SignalAction(bool restart = true) noexcept;
	/**
	 * Destructor
	 */
	~SignalAction();
	//-----------------------------------------------------------------
	/**
	 * Empties the additional signal mask and loads the default disposition.
	 */
	void reset() noexcept;
	/**
	 * Assigns a signal handler.
	 * @param handler callback function
	 */
	void setHandler(void (*handler)(int)) noexcept;
	/**
	 * Assigns a signal handler for real-time signals.
	 * @param handler callback function
	 */
	void setHandler(void (*handler)(int, siginfo_t*, void*)) noexcept;
	/**
	 * Checks whether the given signal invokes it's default action.
	 * @return true if default action, false otherwise
	 */
	bool isDefault() const noexcept;
	/**
	 * Checks whether the given signal is ignored.
	 * @return true if ignored, false otherwise
	 */
	bool isIgnored() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns direct reference to the additional signal mask.
	 * @return reference to the additional signal mask
	 */
	sigset_t& mask() noexcept;
	/**
	 * Returns constant reference to the additional signal mask.
	 * @return reference to the additional signal mask
	 */
	const sigset_t& mask() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the signal handling flags.
	 * @return signal flags
	 */
	int getFlags() const noexcept;
	/**
	 * Configures the signal handling flags.
	 * @param flags new signal flags
	 */
	void setFlags(int flags) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Changes a signal's disposition.
	 * @param signum signal number
	 */
	void install(int signum);
	/**
	 * Changes a signal's disposition and returns the old one.
	 * @param signum signal number
	 * @param old stores the old disposition
	 */
	void install(int signum, SignalAction &old);
	/**
	 * Fetches a signal's current disposition.
	 * @param signum signal number
	 */
	void fetch(int signum);
	//-----------------------------------------------------------------
	/**
	 * Returns a pointer to the internal sigaction structure.
	 * @return pointer to sigaction structure
	 */
	struct sigaction* action() noexcept;
	/**
	 * Returns a constant pointer to the internal sigaction structure.
	 * @return pointer to sigaction structure
	 */
	const struct sigaction* action() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for sigaction(2): examines or changes a signal action.
	 * @param signum signal number
	 * @param act new disposition
	 * @param old stores the old disposition (can be nullptr)
	 */
	static void update(int signum, const struct sigaction *act,
			struct sigaction *old);
	/**
	 * Dummy signal handler which does absolutely nothing.
	 * @param signum signal number
	 */
	static void dummy(int signum) noexcept;
	/**
	 * Dummy signal handler which does absolutely nothing.
	 * @param signum signal number
	 * @param si additional information about the caught signal
	 * @param arg additional argument
	 */
	static void dummy(int signum, siginfo_t *si, void *arg) noexcept;
private:
	void fixFlags() noexcept;
private:
	struct sigaction _action;
	const bool _restart;
	bool _info;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_SIGNALACTION_H_ */
