/**
 * @file OverlayHubInfo.h
 *
 * Overlay hub's runtime statistics
 *
 *
 * Copyright (C) 2022 Wanhive Systems Private Limited (info@wanhive.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_SERVER_CORE_OVERLAYHUBINFO_H_
#define WH_SERVER_CORE_OVERLAYHUBINFO_H_
#include "DHT.h"
#include "../../hub/HubInfo.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Routing table entry
 */
struct RouteInfo {
	/*! Start index */
	unsigned long long start;
	/*! Current index */
	unsigned long long current;
	/*! Old index */
	unsigned long long old;
	/*! Connection status */
	bool connected;
};

/**
 * Overlay hub's runtime metrics
 */
class OverlayHubInfo: public HubInfo {
public:
	/**
	 * Constructor: clears all data.
	 */
	OverlayHubInfo() noexcept;
	/**
	 * Destructor
	 */
	~OverlayHubInfo();
	/**
	 * Clears all data.
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns predecessor's identifier.
	 * @return predecessor
	 */
	unsigned long long getPredecessor() const noexcept;
	/**
	 * Sets predecessor's identifier.
	 * @param predecessor new predecessor
	 */
	void setPredecessor(unsigned long long predecessor) noexcept;
	/**
	 * Returns successor's identifier.
	 * @return successor
	 */
	unsigned long long getSuccessor() const noexcept;
	/**
	 * Sets successor's identifier.
	 * @param successor new successor
	 */
	void setSuccessor(unsigned long long successor) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the number of routes in the routing table.
	 * @return routes count
	 */
	unsigned int getRoutes() const noexcept;
	/**
	 * Sets the number of routes in the routing table.
	 * @param routes new routes count
	 */
	void setRoutes(unsigned int routes) noexcept;
	/**
	 * Returns routing table's status.
	 * @return true if the routing table is stable, false otherwise
	 */
	bool isStable() const noexcept;
	/**
	 * Sets routing table's status.
	 * @param stable true for stable, false for unstable
	 */
	void setStable(bool stable) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a routing table entry.
	 * @param index entry's index
	 * @return pointer to routing table's entry, nullptr on invalid index
	 */
	const RouteInfo* getRoute(unsigned int index) const noexcept;
	/**
	 * Updates the routing table entry at a given index. Fails silently if the
	 * index is invalid.
	 * @param table entry's data
	 * @param index entry's index
	 */
	void setRoute(const RouteInfo &table, unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Serializes the runtime metrics.
	 * @param buffer output buffer
	 * @param size buffer's size in bytes
	 * @return serialized data's size in bytes on success, 0 on error.
	 */
	unsigned int pack(unsigned char *buffer, unsigned int size) const noexcept;
	/**
	 * Loads the serialized runtime metrics data.
	 * @param buffer serialized data
	 * @param size buffer's size in bytes
	 * @return number of bytes read on success, 0 on error
	 */
	unsigned int unpack(const unsigned char *buffer, unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging: prints data to stdout.
	 */
	void print() const noexcept;
public:
	/*! The minimum serialized data size in bytes */
	static constexpr unsigned int MIN_BYTES = (HubInfo::BYTES + 21);
	/*! The maximum serialized data size in bytes */
	static constexpr unsigned int MAX_BYTES = MIN_BYTES
			+ (25 * DHT::KEY_LENGTH);
private:
	unsigned long long predecessor { };
	unsigned long long successor { };
	unsigned int routes { };
	bool stable { };
	RouteInfo route[DHT::KEY_LENGTH];
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYHUBINFO_H_ */
