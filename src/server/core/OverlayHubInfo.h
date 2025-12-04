/*
 * OverlayHubInfo.h
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

namespace wanhive {
/**
 * Routing table's information
 */
struct RouteInfo {
	unsigned long long start;
	unsigned long long current;
	unsigned long long old;
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
	 * Clears the data.
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns predecessor's identifier.
	 * @return predecessor's value
	 */
	unsigned long long getPredecessor() const noexcept;
	/**
	 * Sets predecessor's identifier.
	 * @param predecessor the new predecessor's value
	 */
	void setPredecessor(unsigned long long predecessor) noexcept;
	/**
	 * Returns successor's identifier.
	 * @return successor value
	 */
	unsigned long long getSuccessor() const noexcept;
	/**
	 * Sets successor's identifier.
	 * @param successor the new successor's value
	 */
	void setSuccessor(unsigned long long successor) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns number of routes in the routing table.
	 * @return the routes count
	 */
	unsigned int getRoutes() const noexcept;
	/**
	 * Sets number of routes in the routing table.
	 * @param routes the new routes count
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
	 * @return pointer to routing table's entry at the given index, nullptr if
	 * the index is invalid (see OverlayHubInfo::getRoutes()).
	 */
	const RouteInfo* getRoute(unsigned int index) const noexcept;
	/**
	 * Sets a routing table entry.
	 * @param table entry's data
	 * @param index entry's index, the call will fail silently if the index is
	 * invalid (see OverlayHubInfo::getRoutes()).
	 */
	void setRoute(const RouteInfo &table, unsigned int index) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Serializes this object.
	 * @param buffer pointer to data buffer
	 * @param size buffer's size in bytes
	 * @return size of serialized data on success, 0 on error.
	 */
	unsigned int pack(unsigned char *buffer, unsigned int size) const noexcept;
	/**
	 * Deserializes binary data into this object.
	 * @param buffer pointer to serialized data buffer
	 * @param size buffer's size in bytes
	 * @return the number of bytes read on success, 0 on error.
	 */
	unsigned int unpack(const unsigned char *buffer, unsigned int size) noexcept;
	//-----------------------------------------------------------------
	/**
	 * For debugging: prints data to stdout.
	 */
	void print() const noexcept;
public:
	/** The minimum serialized data size in bytes */
	static constexpr unsigned int MIN_BYTES = (HubInfo::BYTES + 21);
	/** The maximum serialized data size in bytes */
	static constexpr unsigned int MAX_BYTES = MIN_BYTES
			+ (25 * DHT::KEY_LENGTH);
private:
	unsigned long long predecessor { 0 };
	unsigned long long successor { 0 };
	unsigned int routes { 0 };
	bool stable { false };
	RouteInfo route[DHT::KEY_LENGTH];
};

} /* namespace wanhive */

#endif /* WH_SERVER_CORE_OVERLAYHUBINFO_H_ */
