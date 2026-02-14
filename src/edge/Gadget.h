/**
 * @file Gadget.h
 *
 * Gadget Hub
 *
 *
 * Copyright (C) 2026 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_EDGE_GADGET_H_
#define WH_EDGE_GADGET_H_
#include "Edge.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Gadget Hub
 */
class Gadget: public Edge {
public:
	/**
	 * Constructor: creates a new gadget hub.
	 * @param uid unique identifier
	 * @param path configuration file's path
	 */
	Gadget(unsigned long long uid, const char *path = nullptr) noexcept;
	/**
	 * Destructor
	 */
	~Gadget();
protected:
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void maintain() noexcept override;
	void route(Message *message) noexcept override;
	//-----------------------------------------------------------------
	/**
	 * Returns the online option flag.
	 * @return true if enabled, false if disabled
	 */
	bool online() const noexcept;
	/**
	 * Returns the multicast option flag.
	 * @return true if enabled, false if disabled
	 */
	bool multicast() const noexcept;
	/**
	 * Sets the reporting interval.
	 * @param value reporting interval
	 */
	void interval(unsigned int value) noexcept;
	/**
	 * Gets the reporting interval.
	 * @return reporting interval
	 */
	unsigned int interval() const noexcept;
	/**
	 * Prepares a suitable outgoing message header.
	 * @param header message header
	 * @param session session identifier
	 */
	void prepare(MessageHeader &header, unsigned int session) const noexcept;
private:
	//-----------------------------------------------------------------
	/**
	 * Adapter: processes the incoming messages.
	 * @param message incoming message
	 * @return true on success, false on error
	 */
	virtual bool service(Message *message) noexcept;
	//-----------------------------------------------------------------
private:
	void setup();
	void clear() noexcept;
private:
	struct {
		bool online;
		bool multicast;
		unsigned int interval;
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_GADGET_H_ */
