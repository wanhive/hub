/*
 * Worker.h
 *
 * Background task
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_WORKER_H_
#define WH_HUB_WORKER_H_
#include "../base/common/Activity.h"
#include "../base/Thread.h"

namespace wanhive {
/**
 * Background task
 */
class Worker: private Activity {
public:
	/**
	 * Constructor: creates a worker.
	 */
	Worker() noexcept;
	/**
	 * Destructor: aborts if the worker was not stopped.
	 */
	~Worker();
protected:
	/**
	 * Starts the worker in a new thread.
	 * @param arg additional argument
	 * @return true on success, false if no action is available
	 */
	bool start(void *arg);
	/**
	 * Stops the worker.
	 */
	void stop();
private:
	/*
	 * Activity interface adapters.
	 */
	bool doable() const noexcept override;
	void act(void *arg) noexcept override;
	void cease() noexcept override;
private:
	class Job: public Task {
	public:
		Job(Activity &action) noexcept;
		~Job();
		void run(void *arg) noexcept override;
		int getStatus() const noexcept override;
		void setStatus(int status) noexcept override;
	private:
		Activity &action;
	};
private:
	Job job;
	Thread *thread { nullptr };
};

} /* namespace wanhive */

#endif /* WH_HUB_WORKER_H_ */
