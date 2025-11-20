/*
 * Job.h
 *
 * Asynchronous task
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_JOB_H_
#define WH_HUB_JOB_H_
#include "../base/common/Activity.h"
#include "../base/Thread.h"

namespace wanhive {
/**
 * Asynchronous task
 */
class Job: private Activity {
public:
	/**
	 * Constructor: creates a new job.
	 */
	Job() noexcept;
	/**
	 * Destructor: aborts if the job was not stopped.
	 */
	~Job();
protected:
	/**
	 * Starts the job in a separate thread.
	 * @param arg additional argument
	 * @return true on success, false if no action is available
	 */
	bool start(void *arg);
	/**
	 * Stops the job.
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
	class Runner: public Task {
	public:
		Runner(Activity &action) noexcept;
		~Runner();
		void run(void *arg) noexcept override;
		int getStatus() const noexcept override;
		void setStatus(int status) noexcept override;
	private:
		Activity &action;
	};
private:
	Runner runner;
	Thread *thread { nullptr };
};

} /* namespace wanhive */

#endif /* WH_HUB_JOB_H_ */
