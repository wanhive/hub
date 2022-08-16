/*
 * Logic.h
 *
 * Digital logic watcher
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_LOGIC_H_
#define WH_HUB_LOGIC_H_
#include "../reactor/Watcher.h"

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Possible digital logic transitions
 */
enum class LogicEdge {
	RISING, /**< Rising edge */
	FALLING,/**< Falling edge */
	NONE /**< Neither */
};
/**
 * Structure for reporting digital logic transitions
 */
struct LogicEvent {
	LogicEdge type { LogicEdge::NONE };
	unsigned long long timestamp { 0 };
};
//-----------------------------------------------------------------
/**
 * Digital logic watcher (base class for concrete implementation)
 */
class Logic: public Watcher {
public:
	Logic();
	~Logic();
	//-----------------------------------------------------------------
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Reads the next event.
	 * @param event object for storing the event
	 * @return the number of bytes read on success, 0 on temporary error, -1 on
	 * fatal error.
	 */
	virtual ssize_t update(LogicEvent &event);
};

} /* namespace wanhive */

#endif /* WH_HUB_LOGIC_H_ */
