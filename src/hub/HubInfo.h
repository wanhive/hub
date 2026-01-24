/*
 * HubInfo.h
 *
 * Hub's runtime information
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_HUB_HUBINFO_H_
#define WH_HUB_HUBINFO_H_

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Resource usage metrics
 */
struct ResourceInfo {
	/*! Maximum allocation  */
	unsigned int max;
	/*! Current usage */
	unsigned int used;
};
/**
 * Traffic metrics
 */
struct TrafficInfo {
	/*! Number of units */
	unsigned long long units;
	/*! Number of bytes */
	unsigned long long bytes;
};
//-----------------------------------------------------------------
/**
 * Hub's runtime metrics
 */
class HubInfo {
public:
	/**
	 * Default constructor: clears out the data.
	 */
	HubInfo() noexcept;
	/**
	 * Destructor
	 */
	~HubInfo();
	/**
	 * Clears out the data.
	 */
	void clear() noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the unique identifier.
	 * @return unique identifier.
	 */
	unsigned long long getUid() const noexcept;
	/**
	 * Sets the unique identifier.
	 * @param uid the new unique identifier value
	 */
	void setUid(unsigned long long uid) noexcept;
	/**
	 * Returns the uptime value in seconds.
	 * @return uptime value
	 */
	double getUptime() const noexcept;
	/**
	 * Sets the uptime value (in seconds).
	 * @param uptime the new uptime value
	 */
	void setUptime(double uptime) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns incoming traffic metrics.
	 * @return incoming traffic metrics
	 */
	const TrafficInfo& getReceived() const noexcept;
	/**
	 * Sets incoming traffic metrics.
	 * @param received the new incoming traffic metrics
	 */
	void setReceived(const TrafficInfo &received) noexcept;
	/**
	 * Returns dropped traffic metrics.
	 * @return dropped traffic metrics
	 */
	const TrafficInfo& getDropped() const noexcept;
	/**
	 * Sets dropped traffic metrics.
	 * @param dropped the new dropped traffic metrics
	 */
	void setDropped(const TrafficInfo &dropped) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns connection allocation and utilization metrics.
	 * @return connection usage data
	 */
	const ResourceInfo& getConnections() const noexcept;
	/**
	 * Sets connection allocation and utilization metrics.
	 * @param connections the new connection usage metrics
	 */
	void setConnections(const ResourceInfo &connections) noexcept;
	/**
	 * Returns message allocation and utilization metrics.
	 * @return message usage data
	 */
	const ResourceInfo& getMessages() const noexcept;
	/**
	 * Sets message allocation and utilization metrics.
	 * @param messages the new message usage metrics
	 */
	void setMessages(const ResourceInfo &messages) noexcept;
	/**
	 * Returns the current MTU (maximum transmission unit) value in bytes.
	 * @return the MTU value
	 */
	unsigned int getMTU() const noexcept;
	/**
	 * Sets the MTU (maximum transmission unit) value in bytes.
	 * @param mtu the new MTU value
	 */
	void setMTU(unsigned int mtu) noexcept;
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
	/*! Serialized data size in bytes */
	static constexpr unsigned int BYTES = 68;
private:
	unsigned long long uid { };
	double uptime { };
	TrafficInfo received { };
	TrafficInfo dropped { };
	ResourceInfo connections { };
	ResourceInfo messages { };
	unsigned int mtu { };
};

} /* namespace wanhive */

#endif /* WH_HUB_HUBINFO_H_ */
