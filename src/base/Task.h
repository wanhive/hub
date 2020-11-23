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

#ifndef WH_BASE_TASK_H_
#define WH_BASE_TASK_H_

namespace wanhive {
/**
 * Base class for tasks
 */
class Task {
public:
	virtual ~Task() = default;
	virtual void run(void *arg) noexcept = 0;
	virtual int getStatus() const noexcept = 0;
	virtual void setStatus(int status) noexcept = 0;
};

} /* namespace wanhive */

#endif /* WH_BASE_TASK_H_ */
