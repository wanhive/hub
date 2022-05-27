/*
 * OverlayHubInfo.h
 *
 * Runtime statistics of an overlay hub
 *
 *
 * Copyright (C) 2019 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_OVERLAY_OVERLAYHUBINFO_H_
#define WH_SERVER_OVERLAY_OVERLAYHUBINFO_H_
#include "DHT.h"

namespace wanhive {
/**
 * Runtime metrics of an overlay hub
 */
struct OverlayHubInfo {
	unsigned long long uid;
	unsigned int mtu;
	struct {
		unsigned int maxConnections;
		unsigned int connections;
		unsigned int maxMessages;
		unsigned int messages;
	} resource;
	struct {
		double uptime;
		unsigned long long receivedPackets;
		unsigned long long receivedBytes;
		unsigned long long droppedPackets;
		unsigned long long droppedBytes;
	} stat;
	/**
	 * Routing table information follows
	 */
	unsigned long long predecessor;
	unsigned long long successor;
	bool stable; //Is the routing table in a stable state
	unsigned int routes; //Number of routing table entries
	struct { //Optional data
		unsigned long long start;
		unsigned long long current;
		unsigned long long old;
		bool connected;
	} route[DHT::IDENTIFIER_LENGTH];

	//Minimum size of the serialized structure in bytes
	static constexpr unsigned int MIN_BYTES = 84;
};

}  // namespace wanhive

#endif /* WH_SERVER_OVERLAY_OVERLAYHUBINFO_H_ */
