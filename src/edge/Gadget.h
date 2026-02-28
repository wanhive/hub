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
 * @brief Gadget Hub
 * @details This component extends the Edge Hub by adding support for both
 * end-to-end communication and group communication.
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
	/**
	 * Gets the online option flag.
	 * @return true if enabled, false if disabled
	 */
	bool online() const noexcept;
	/**
	 * Gets the multicast option flag.
	 * @return true if enabled, false if disabled
	 */
	bool multicast() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Checks permission for online data transmission.
	 * @param charge true to consume an access token; false otherwise
	 * @return true if permitted, false if denied
	 */
	bool share(bool charge = true) noexcept;
	/**
	 * Generates an appropriate outgoing message header.
	 * @param header stores the header data
	 * @param channel session or topic identifier
	 */
	void prepare(MessageHeader &header, unsigned int channel) const noexcept;
	//-----------------------------------------------------------------
	void configure(void *arg) override;
	void cleanup() noexcept override;
	void maintain() noexcept override;
	void route(Message *message) noexcept override;
	//-----------------------------------------------------------------
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
	} ctx;
};

} /* namespace wanhive */

#endif /* WH_EDGE_GADGET_H_ */
