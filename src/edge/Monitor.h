/**
 * @file Monitor.h
 *
 * Monitoring Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_EDGE_MONITOR_H_
#define WH_EDGE_MONITOR_H_
#include "../hub/Agent.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Monitoring Hub
 */
class Monitor: public Agent {
public:
	/**
	 * Constructor: creates a new monitoring hub.
	 * @param uid unique identifier
	 * @param path configuration file's path
	 */
	Monitor(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Monitor();
protected:
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Periodically sends an association request to the designated remote node.
	 * @param interval polling interval
	 * @param sqn sequence number
	 * @param tokens default access tokens count if the remote host's latency
	 * is zero.
	 * @return true on success, false on error
	 */
	bool heartbeat(unsigned int interval, unsigned int sqn,
			unsigned int tokens = 0) noexcept;
	/**
	 * Sends association request to a remote node.
	 * @param id remote node's identifier
	 * @param sqn sequence number
	 * @param tokens access tokens count
	 * @return true on success, false on error
	 */
	bool sample(unsigned long long id, unsigned int sqn,
			unsigned int tokens = 0) noexcept;
	/**
	 * Processes association response from a remote node.
	 * @param message association response
	 * @return true on success, false on error
	 */
	bool connect(Message *message) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Assigns a remote node for monitoring.
	 * @param peer remote node's identifier
	 * @param latency remote node's reporting interval
	 * @return true on success, false on error
	 */
	bool target(unsigned long long id, unsigned int latency = 0) noexcept;
	/**
	 * Returns remote node's identifier.
	 * @return remote node's identifier
	 */
	unsigned long long host() const noexcept;
	/**
	 * Returns remote node's reporting interval.
	 * @return reporting interval
	 */
	unsigned int latency() const noexcept;
	/**
	 * Breaks the association with the remote node.
	 */
	void revoke() noexcept;
private:
	void setup();
	void clear() noexcept;
private:
	struct {
		unsigned long long id;
		unsigned int latency;
		unsigned int sqn;
	} edge;
};

} /* namespace wanhive */

#endif /* WH_EDGE_MONITOR_H_ */
