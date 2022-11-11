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
 */
class Signal {
public:
	/**
	 * Blocks a given signal.
	 * @param signum signal number
	 */
	static void block(int signum);
	/**
	 * Unblocks a given signal.
	 * @param signum signal number
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
	 * Specifies that a given signal should be ignored.
	 * @param signum signal number
	 */
	static void ignore(int signum);
	/**
	 * Installs a given signal's default handler.
	 * @param signum signal number
	 */
	static void reset(int signum);
	/**
	 * Installs a given signal's handler.
	 * @param signum signal number
	 * @param handler signal handler (nullptr for a dummy handler)
	 * @param restart true to force restart of interruptible system calls when
	 * interrupted by a signal handler, false otherwise.
	 */
	static void handle(int signum, void (*handler)(int) = nullptr,
			bool restart = true);
};

} /* namespace wanhive */

#endif /* WH_BASE_SIGNAL_H_ */
