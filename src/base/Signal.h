/**
 * @file Signal.h
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

/*! @namespace wanhive */
namespace wanhive {
/**
 * Common signal handling routines
 */
class Signal {
public:
	/**
	 * Blocks all signals.
	 */
	static void block();
	/**
	 * Unblocks all signals.
	 */
	static void unblock();
	/**
	 * Blocks a signal.
	 * @param signum signal number
	 */
	static void block(int signum);
	/**
	 * Unblocks a signal.
	 * @param signum signal number
	 */
	static void unblock(int signum);
	/**
	 * Specifies that a signal should be ignored.
	 * @param signum signal number
	 */
	static void ignore(int signum);
	/**
	 * Installs a signal's default handler.
	 * @param signum signal number
	 */
	static void reset(int signum);
	/**
	 * Installs a signal handler.
	 * @param signum signal number
	 * @param handler signal handler (nullptr for dummy handler)
	 * @param restart true to automatically restart certain system calls if
	 * interrupted by a signal, false to fail with an error.
	 */
	static void handle(int signum, void (*handler)(int) = nullptr,
			bool restart = true);
};

} /* namespace wanhive */

#endif /* WH_BASE_SIGNAL_H_ */
