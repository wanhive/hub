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
	 * Sends association request to a remote host.
	 * @param id remote node's identifier
	 * @param sqn sequence number
	 * @param tokens access tokens count
	 * @return true on success, false on error
	 */
	bool engage(unsigned long long id, unsigned int sqn,
			unsigned int tokens) noexcept;
	/**
	 * Processes association response from a remote host.
	 * @param message association response
	 * @return true on success, false on error
	 */
	bool connect(Message *message) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Breaks existing association with remote node.
	 */
	void revoke() noexcept;
	/**
	 * Returns remote node's identifier.
	 * @return remote node's identifier
	 */
	unsigned long long getPeer() const noexcept;
	/**
	 * Updates remote node's identifier.
	 * @param host remote node's identifier
	 */
	void setPeer(unsigned long long peer) noexcept;
	/**
	 * Returns remote node's reporting interval.
	 * @return reporting interval
	 */
	unsigned int getLatency() const noexcept;
	/**
	 * Updates remote node's reporting interval.
	 * @param latency reporting interval
	 */
	void setLatency(unsigned int latency) noexcept;
private:
	bool connect(unsigned long long id, unsigned int sqn,
			unsigned int latency) noexcept;
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
