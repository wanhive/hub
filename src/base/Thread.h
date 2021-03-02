/*
 * Thread.h
 *
 * Concurrent task execution, uses Linux's pthread
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_THREAD_H_
#define WH_BASE_THREAD_H_
#include "Task.h"
#include <pthread.h>
#include <signal.h>

namespace wanhive {

/**
 * Linux pthreads(7) abstraction
 * Thread safe at class level
 */
class Thread: public Task {
public:
	Thread() noexcept;
	Thread(Task *target) noexcept;
	virtual ~Thread();
	void run(void *arg = nullptr) noexcept override;
	int getStatus() const noexcept override;
	void setStatus(int status) noexcept override;
	//Start the thread, pass the user data via <arg>
	void start(void *arg = nullptr);
	/**
	 * Either join() or detach() should be called for each thread
	 * that an application creates, so that system resources for the
	 * thread can be released
	 */
	//Separates the thread of execution from this object, wrapper for pthread_detach()
	void detach();
	//Caller will wait for this thread to finish/die, wrapper for pthread_join()
	void join();
	//Returns true if the thread is running in parallel context (always false if detached)
	bool isAlive() const noexcept;

	//Send a signal to this thread, throw an Exception on error
	void signal(int signum);
	//Same as the above, however fails silently and uses SIGUSR1 by default
	void interrupt(int signum = SIGUSR1) noexcept;

	//Sets the CPU affinity mask of the thread
	void setAffinity(int cpuNumber);

	//Returns the user data
	void* getArgument() const noexcept;
	//Returns the Task
	Task* getTask() const noexcept;

	//Number of CPUs on this machine
	static long getNumberOfCPUs();
private:
	static void* entryPoint(void *ptr) noexcept;
	//Clear the internal states and settings
	void clear() noexcept;
private:
	Task *task;
	void *arg;
	pthread_t threadId;
	bool alive;
};

} /* namespace wanhive */

#endif /* WH_BASE_THREAD_H_ */
