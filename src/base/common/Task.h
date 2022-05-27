/*
 * Task.h
 *
 * Base class for tasks
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_TASK_H_
#define WH_BASE_COMMON_TASK_H_

namespace wanhive {
/**
 * Base class for tasks
 */
class Task {
public:
	/**
	 * Virtual destructor
	 */
	virtual ~Task() = default;
	/**
	 * Runs this task.
	 * @param arg the additional argument
	 */
	virtual void run(void *arg) noexcept = 0;
	/**
	 * Returns the status code.
	 * @return the status
	 */
	virtual int getStatus() const noexcept = 0;
	/**
	 * Sets a new status code.
	 * @param status the new status
	 */
	virtual void setStatus(int status) noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_TASK_H_ */
