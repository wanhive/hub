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
	 * Activates or deactivates the remote session feature.
	 * @param enable true to activate, otherwise false
	 */
	void session(bool enable) noexcept;
	/**
	 * Checks if remote session access is allowed.
	 * @return true if allowed, otherwise false
	 */
	bool session() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Handles and responds to a session request.
	 * @param message the session request
	 * @param interval reporting interval
	 * @return true on success, false on error (request denied)
	 */
	bool accept(Message *message, unsigned int interval = 0) noexcept;
	/**
	 * Creates an active session with a remote node.
	 * @param id remote node's identifier
	 * @param tokens access tokens count
	 * @return true on success, false on error
	 */
	bool accept(unsigned long long id, unsigned int tokens = 0) noexcept;
	/**
	 * Ends the current session.
	 */
	void close() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Verifies whether an active session with a remote node exists.
	 * @return true if active, false if inactive
	 */
	bool live() const noexcept;
	/**
	 * Validates the active session with a remote node and, if successful,
	 * consumes an access token.
	 * @return true if active, false if inactive
	 */
	bool engage() noexcept;
	/**
	 * Gets the remote node's identifier (may be inactive).
	 * @return remote node's identifier
	 */
	unsigned long long host() const noexcept;
	//-----------------------------------------------------------------
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

	struct {
		bool session;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_EDGE_H_ */
