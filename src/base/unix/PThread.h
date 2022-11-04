/*
 * PThread.h
 *
 * Posix thread
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_UNIX_PTHREAD_H_
#define WH_BASE_UNIX_PTHREAD_H_
#include "../common/Task.h"
#include <pthread.h>

namespace wanhive {
/**
 * Posix thread abstraction
 * @ref pthreads(7)
 */
class PThread {
public:
	/**
	 * Constructor: creates a new thread of execution.
	 * @param task reference to task
	 * @param arg arguments for task
	 * @param detached true for detached thread, false otherwise
	 */
	PThread(Task &task, void *arg, bool detached);
	/**
	 * Destructor
	 */
	~PThread();
	//-----------------------------------------------------------------
	/**
	 * Wrapper for pthread_join(3): waits for the thread to terminate.
	 * @return terminated thread's status
	 */
	void* join();
	/**
	 * Wrapper for pthread_exit(3): terminates the calling thread
	 * @param arg terminated thread's status
	 */
	static void exit(void *arg);
	/**
	 * Returns the running thread's identifier.
	 * @return thread identifier
	 */
	pthread_t getId() const noexcept;
	/**
	 * Returns task's status (see Task::getStatus()).
	 * @return task's status
	 */
	int getStatus() const noexcept;
	/**
	 * Sets task's status (see Task::setStatus()).
	 * @param status new status
	 */
	void setStatus(int status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Wrapper for pthread_self(3): returns calling thread's identifier.
	 * @return calling thread's identifier
	 */
	static pthread_t self() noexcept;
	/**
	 * Wrapper for pthread_equal(3): compares thread identifiers.
	 * @param t1 first value for comparison
	 * @param t2 second value for comparison
	 * @return true if the two identifiers are equal, false otherwise
	 */
	static bool equal(pthread_t t1, pthread_t t2) noexcept;
private:
	void start();
	void run(void *arg) noexcept;
	static void* routine(void *arg);
private:
	pthread_t tid;
	Task &task;
	void *arg;
	const bool detached;
};

} /* namespace wanhive */

#endif /* WH_BASE_UNIX_PTHREAD_H_ */
