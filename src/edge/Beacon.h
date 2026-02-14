/**
 * @file Beacon.h
 *
 * Beacon Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_EDGE_BEACON_H_
#define WH_EDGE_BEACON_H_
#include "Gadget.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Beacon Hub
 * @note Sends time-stamped messages continuously and at regular intervals.
 */
class Beacon final: public Gadget {
public:
	/**
	 * Constructor: creates a new beacon hub.
	 * @param uid unique identifier
	 * @param path configuration file's path
	 */
	Beacon(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Beacon();
private:
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void onAlarm(unsigned long long uid, unsigned long long ticks) noexcept
			override;
private:
	bool service(Message *message) noexcept override;
	bool transmit() noexcept;
	void setup();
	void clear() noexcept;
public:
	static constexpr unsigned int CHANNEL = 17;
private:
	struct {
		unsigned int channel;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_BEACON_H_ */
