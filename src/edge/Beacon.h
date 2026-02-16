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
 * @brief Beacon Hub
 * @details Continuously sends time-stamped messages at regular intervals for
 * end-to-end and group communication.
 */
class Beacon: public Gadget {
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
protected:
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void onAlarm(unsigned long long uid, unsigned long long ticks) noexcept
			override;
	//-----------------------------------------------------------------
	/**
	 * Returns the transmission channel (session or topic identifier).
	 * @return transmission channel
	 */
	unsigned int channel() const noexcept;
private:
	bool service(Message *message) noexcept override;
	bool transmit() noexcept;
	void setup();
	void clear() noexcept;
private:
	struct {
		unsigned int channel;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_BEACON_H_ */
