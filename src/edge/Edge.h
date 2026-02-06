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
	 * Returns the online option flag.
	 * @return online flag
	 */
	bool online() const noexcept;
	/**
	 * Returns the multicast option flag.
	 * @return multicast flag
	 */
	bool multicast() const noexcept;
	/**
	 * Processes and responds to an association request.
	 * @param message pairing request
	 * @param interval reporting interval
	 * @return true on success, false on error (request denied)
	 */
	bool accept(Message *message, unsigned int interval = 0) noexcept;
	/**
	 * Creates an association with a remote node.
	 * @param id remote node's identifier
	 * @param tokens access tokens count
	 * @return true on success, false on error
	 */
	bool accept(unsigned long long id, unsigned int tokens) noexcept;
	/**
	 * Breaks existing association with remote node.
	 */
	void revoke() noexcept;
	/**
	 * Checks for active association with a remote node.
	 * @return true if an association exists, false otherwise
	 */
	bool live() const noexcept;
	/**
	 * Checks for active association with a remote node and consumes an
	 * access token.
	 * @return true if an association exists, false otherwise
	 */
	bool report() noexcept;
	/**
	 * Returns the associated remote node's identifier.
	 * @return remote node's identifier
	 */
	unsigned long long peer() const noexcept;
	/**
	 * Returns a timestamp ideal for recording an event's occurrence.
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
	} slot;

	struct {
		bool online;
		bool multicast;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_EDGE_H_ */
