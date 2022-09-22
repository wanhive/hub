/*
 * PSignal.h
 *
 * Signal handling for processes
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_PSIGNAL_H_
#define WH_BASE_UNIX_PSIGNAL_H_
#include "SignalSet.h"
#include <sys/types.h>

namespace wanhive {
/**
 * Signal handling for processes
 */
class PSignal {
public:
	/**
	 * Sends signal to the calling process.
	 * @param signum signal number
	 */
	static void raise(int signum);
	/**
	 * Wrapper for kill(2): sends signal to a process
	 * @param pid process identifier
	 * @param signum signal number
	 */
	static void raise(pid_t pid, int signum);
	/**
	 * Wrapper for alarm(2): schedules an alarm.
	 * @param seconds alarm's expiration in seconds
	 * @return number of seconds remaining in the previously scheduled alarm
	 */
	static unsigned int alarm(unsigned int seconds) noexcept;
	/**
	 * Wrapper for pause(2): waits for a signal.
	 */
	static void pause() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Sets signal mask.
	 * @param how operation type
	 * @param ss new mask
	 */
	static void setMask(int how, const SignalSet &ss);
	/**
	 * Sets new signal mask and returns the old mask.
	 * @param how operation type
	 * @param ss new mask
	 * @param old stores the old mask
	 */
	static void setMask(int how, const SignalSet &ss, SignalSet &old);
	/**
	 * Returns the current signal mask.
	 * @param ss stores the current mask
	 */
	static void getMask(SignalSet &ss);
	/**
	 * Wrapper for sigprocmask(2): changes/fetches the signal mask.
	 * @param how operation type
	 * @param set new mask
	 * @param old stores the old mask
	 */
	static void mask(int how, const sigset_t *set, sigset_t *old);
	//-----------------------------------------------------------------
	/**
	 * Examines pending signals.
	 * @param ss stores the pending signals
	 */
	static void pending(SignalSet &ss);
	/**
	 * Wrapper for sigpending(2): examines pending signals.
	 * @param set stores the pending signals
	 */
	static void pending(sigset_t *set);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for sigsuspend(2): waits for a signal.
	 * @param ss signals to wait for
	 */
	static void suspend(const SignalSet &ss);
	/**
	 * Wrapper for sigsuspend(2): waits for a signal.
	 * @param set signals to wait for
	 */
	static void suspend(const sigset_t *set);
	//-----------------------------------------------------------------
	/**
	 * Wrapper for sigqueue(2): queues up a signal to a process
	 * @param pid process' identifier
	 * @param signum signal number
	 * @param value additional data
	 */
	static void queue(pid_t pid, int signum, const sigval value);
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PSIGNAL_H_ */
