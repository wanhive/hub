/**
 * @file Edge.h
 *
 * Edge Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_EDGE_EDGE_H_
#define WH_EDGE_EDGE_H_
#include "../hub/Agent.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Edge Hub
 */
class Edge: public Agent {
public:
	/**
	 * Constructor: creates a new edge hub.
	 * @param uid unique identifier
	 * @param path configuration file's path
	 */
	Edge(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Edge();
protected:
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Processes and responds to an association request.
	 * @param message association request
	 * @param interval reporting interval
	 * @return true on success, false on error (request denied)
	 */
	bool accept(Message *message, unsigned int interval = 0) noexcept;
	/**
	 * Creates association with a remote node.
	 * @param id remote node's identifier
	 * @param tokens access tokens count
	 * @return true on success, false on error
	 */
	bool accept(unsigned long long id, unsigned int tokens = 0) noexcept;
	/**
	 * Confirms an active association with the remote node.
	 * @return true if an association exists, false otherwise
	 */
	bool linked() const noexcept;
	/**
	 * Verifies the active association with a remote node and, if successful,
	 * consumes an access token.
	 * @return true if an association exists, false otherwise
	 */
	bool sample() noexcept;
	/**
	 * Returns remote node's identifier (may not be active).
	 * @return remote node's identifier
	 */
	unsigned long long host() const noexcept;
	/**
	 * Breaks the association with the remote node.
	 */
	void revoke() noexcept;
	/**
	 * Provides a timestamp suitable for recording when an event occurs.
	 * @return seconds elapsed since the Epoch
	 */
	static double timestamp() noexcept;
private:
	void setup();
	void clear() noexcept;
private:
	struct {
		unsigned long long id;
		unsigned int tokens;
	} meta;
};

} /* namespace wanhive */

#endif /* WH_EDGE_EDGE_H_ */
