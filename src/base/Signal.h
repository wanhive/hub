/*
 * Signal.h
 *
 * Common signal handling routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_SIGNAL_H_
#define WH_BASE_SIGNAL_H_
#include <signal.h>

namespace wanhive {
/**
 * Common signal handling routines
 * All the methods are thread safe
 */
class Signal {
public:
	/**
	 * Blocks the given signal signal, will be delivered when unblocked.
	 * @param signum the signal number
	 */
	static void block(int signum);
	/**
	 * Unblocks the given signal.
	 * @param signum the signal number
	 */
	static void unblock(int signum);
	/**
	 * Blocks all signals.
	 */
	static void blockAll();
	/**
	 * Unblock all signals.
	 */
	static void unblockAll();
	/**
	 * Specifies that the given signal should be ignored (the signal handler set
	 * to SIG_IGN).
	 * @param signum the signal number
	 */
	static void ignore(int signum);
	/**
	 * Specifies that the given signal should be assigned it's default behavior
	 * (the handler set to SIG_DFL).
	 * @param signum the signal number
	 */
	static void reset(int signum);
	/**
	 * Installs a callback (handler) for the given signal.
	 * @param signum the signal number
	 * @param handler the signal handler, if nullptr then uses a dummy handler
	 * @param restart true to set the SA_RESTART flag, false otherwise
	 */
	static void handle(int signum, void (*handler)(int)=nullptr, bool restart =
			true);
};

} /* namespace wanhive */

#endif /* WH_BASE_SIGNAL_H_ */
