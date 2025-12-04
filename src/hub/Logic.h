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
 * Digital logic transitions
 */
enum class LogicEdge {
	RISING, /**< Low-to-High */
	FALLING,/**< High-to-Low */
	NONE /**< Neither */
};
/**
 * Digital logic data structure
 */
struct LogicEvent {
	/*! Logic transition type */
	LogicEdge type { LogicEdge::NONE };
	/*! Event's time-stamp */
	unsigned long long timestamp { 0 };
};
//-----------------------------------------------------------------
/**
 * Digital logic watcher
 * @note Doesn't provide concrete implementation
 */
class Logic: public Watcher {
public:
	/**
	 * Constructor
	 */
	Logic() noexcept;
	/**
	 * Destructor
	 */
	~Logic();
	//-----------------------------------------------------------------
	void start() override;
	void stop() noexcept override;
	bool callback(void *arg) noexcept override;
	bool publish(void *arg) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Reports the next event.
	 * @param event stores the event
	 * @return number of bytes read on success, 0 on non-fatal error, -1 on
	 * fatal error.
	 */
	virtual ssize_t report(LogicEvent &event);
};

} /* namespace wanhive */

#endif /* WH_HUB_LOGIC_H_ */
