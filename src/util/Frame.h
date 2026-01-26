/**
 * @file Frame.h
 *
 * Message frame
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_FRAME_H_
#define WH_UTIL_FRAME_H_
#include "MessageHeader.h"
#include "../base/ds/StaticBuffer.h"

/*! @namespace wanhive */
namespace wanhive {
/**
 * Bare-bone data unit implementation. It consists of a routing header and a
 * frame buffer. Routing header provides the route information during message
 * delivery and forwarding. Frame buffer stores the serialized data.
 * @note Frame buffer's structure: [(MESSAGE HEADER)(VARIABLE LENGTH PAYLOAD)]
 */
class Frame {
public:
	/**
	 * Default constructor: sets origin to zero(0).
	 */
	Frame() noexcept;
	/**
	 * Constructor: assigns an origin.
	 * @param origin origin's identifier
	 */
	Frame(uint64_t origin) noexcept;
	/**
	 * Destructor
	 */
	~Frame();
	//-----------------------------------------------------------------
	/**
	 * Returns this frame's origin.
	 * @return origin's identifier
	 */
	uint64_t getOrigin() const noexcept;
	/**
	 * Returns a reference to the routing header. Any change made in the routing
	 * header is not automatically synchronized with the frame buffer's header
	 * data and vice versa.
	 * @return routing header's reference
	 */
	MessageHeader& header() noexcept;
	/**
	 * Returns a constant reference to the routing header. The routing header's
	 * data may or may not be in sync with the frame buffer's header data.
	 * @return routing header's reference
	 */
	const MessageHeader& header() const noexcept;

	/**
	 * Returns a pointer to the given offset within the frame buffer. Any change
	 * made through this pointer does not effect the routing header.
	 * @param offset desired offset in bytes
	 * @return a pointer corresponding to the given offset, nullptr on error
	 */
	unsigned char* buffer(unsigned int offset = 0) noexcept;
	/**
	 * Returns a constant pointer to the given offset within the frame buffer.
	 * @param offset desired offset in bytes
	 * @return a pointer corresponding to the given offset, nullptr on error
	 */
	const unsigned char* buffer(unsigned int offset = 0) const noexcept;

	/**
	 * Returns a pointer to the given offset within the frame buffer's payload
	 * data section.
	 * @param offset desired offset in bytes
	 * @return a pointer to the given offset inside payload, nullptr on error
	 */
	unsigned char* payload(unsigned int offset = 0) noexcept;
	/**
	 * Returns a constant pointer to the given offset within the frame buffer's
	 * payload data section.
	 * @param offset desired offset in bytes
	 * @return a pointer to the given offset inside payload, nullptr on error
	 */
	const unsigned char* payload(unsigned int offset = 0) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Clears the routing header and the frame buffer.
	 */
	void clear() noexcept;
protected:
	/**
	 * Returns the hops count.
	 * @return hops count
	 */
	unsigned int getHops() const noexcept;
	/**
	 * Sets the hops count.
	 * @param hops hops count
	 */
	void setHops(unsigned int hops) noexcept;
	/**
	 * Returns the reference count.
	 * @return reference count
	 */
	unsigned int getLinks() const noexcept;
	/**
	 * Sets the reference count.
	 * @param links reference count
	 */
	void setLinks(unsigned int links) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a constant reference to the frame buffer.
	 * @return frame buffer's reference
	 */
	auto& frame() const noexcept {
		return _frame;
	}
	/**
	 * Returns a reference to the frame buffer
	 * @return frame buffer's reference
	 */
	auto& frame() noexcept {
		return _frame;
	}
public:
	/*! Serialized header size in bytes */
	static constexpr unsigned int HEADER_SIZE = MessageHeader::SIZE;
	/*! The maximum frame buffer size in bytes */
	static constexpr unsigned int MTU = 1024;
	/*! The maximum payload size in bytes */
	static constexpr unsigned int PAYLOAD_SIZE = (MTU - HEADER_SIZE);
private:
	unsigned int hops { };
	unsigned int links { };
	const uint64_t origin;
	MessageHeader _header;
	StaticBuffer<unsigned char, MTU> _frame;
};

} /* namespace wanhive */

#endif /* WH_UTIL_FRAME_H_ */
