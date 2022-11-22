/*
 * Frame.h
 *
 * The message frame
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

namespace wanhive {
/**
 * Bare-bone data unit implementation. It consists of a routing header and a
 * frame buffer. The routing header provides the route information during the
 * message delivery and forwarding. Frame buffer stores serialized data.
 * @note Frame buffer's structure: [(MESSAGE HEADER)(VARIABLE LENGTH PAYLOAD)]
 */
class Frame {
public:
	/**
	 * Default constructor: sets origin to zero(0).
	 */
	Frame() noexcept;
	/**
	 * Constructor: sets frame's origin.
	 * @param origin origin's identifier
	 */
	Frame(uint64_t origin) noexcept;
	/**
	 * Destructor
	 */
	~Frame();
	//-----------------------------------------------------------------
	/**
	 * Returns the origin identifier of this object.
	 * @return the origin identifier
	 */
	uint64_t getOrigin() const noexcept;
	/**
	 * Returns a reference to the routing header. Any change made in the routing
	 * header is not automatically synchronized with the serialized header data
	 * stored at the beginning of the frame buffer and vice versa.
	 * @return reference to the routing header
	 */
	MessageHeader& header() noexcept;
	/**
	 * Returns a constant reference to the routing header. The routing header's
	 * data may or may not be in sync with the serialized header data stored at
	 * the beginning of the frame buffer.
	 * @return constant reference to the routing header
	 */
	const MessageHeader& header() const noexcept;

	/**
	 * Returns a pointer to the given offset within the frame buffer's backing
	 * array (serialized data). Any changes made through this pointer are not
	 * reflected inside the routing header.
	 * @param offset desired offset in bytes, this value should be less than
	 * Frame::MTU.
	 * @return a pointer to the given offset inside the backing array if the
	 * offset is valid, nullptr otherwise.
	 */
	unsigned char* buffer(unsigned int offset = 0) noexcept;
	/**
	 * Returns a constant pointer to the given offset within the frame buffer's
	 * backing array (serialized data).
	 * @param offset desired offset in bytes, this value should be less than
	 * Frame::MTU.
	 * @return a constant pointer to the given offset inside the backing array
	 * if the offset is valid, nullptr otherwise.
	 */
	const unsigned char* buffer(unsigned int offset = 0) const noexcept;

	/**
	 * Returns a pointer to the given offset within the frame buffer's payload
	 * data (equivalent to Frame::buffer(offset + Frame::HEADER_SIZE)).
	 * @param offset desired offset within the payload section, this value
	 * should be less than Frame::PAYLOAD_SIZE.
	 * @return a pointer to the given offset inside the payload section if the
	 * offset is valid, nullptr otherwise.
	 */
	unsigned char* payload(unsigned int offset = 0) noexcept;
	/**
	 * Returns a constant pointer to the given offset within the frame buffer's
	 * payload data (equivalent to Frame::buffer(offset + Frame::HEADER_SIZE)).
	 * @param offset desired offset within the payload section, this value
	 * should be less than Frame::PAYLOAD_SIZE.
	 * @return a constant pointer to the given offset inside the payload section
	 * if the offset is valid, nullptr otherwise.
	 */
	const unsigned char* payload(unsigned int offset = 0) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Clears out the routing header and the frame buffer.
	 */
	void clear() noexcept;
protected:
	/**
	 * For traffic shaping: returns the hop count.
	 * @return the hop count
	 */
	unsigned int getHopCount() const noexcept;
	/**
	 * For traffic shaping: sets the hop count.
	 * @param hopCount the desired hop count
	 */
	void setHopCount(unsigned int hopCount) noexcept;

	/**
	 * For multicasting: returns the reference count.
	 * @return the reference count
	 */
	unsigned int getReferenceCount() const noexcept;
	/**
	 * For multicasting: sets the reference count.
	 * @param referenceCount the desired reference count
	 */
	void setReferenceCount(unsigned int referenceCount) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a constant reference to the frame buffer. This call allows the
	 * derived classes to directly investigate the frame buffer.
	 * @return a constant reference to the frame buffer
	 */
	auto& frame() const noexcept {
		return _frame;
	}
	/**
	 * Returns a reference to the frame buffer. This call allows the derived
	 * classes to directly manipulate the frame buffer.
	 * @return a reference to the frame buffer
	 */
	auto& frame() noexcept {
		return _frame;
	}
public:
	/** Serialized header size in bytes */
	static constexpr unsigned int HEADER_SIZE = MessageHeader::SIZE;
	/** The maximum frame buffer size in bytes */
	static constexpr unsigned int MTU = 1024;
	/** The maximum payload size in bytes */
	static constexpr unsigned int PAYLOAD_SIZE = (MTU - HEADER_SIZE);
private:
	unsigned int hopCount { 0 }; //Hop count
	unsigned int referenceCount { 0 }; //Reference count
	const uint64_t origin; //Origin's identifier
	MessageHeader _header; //Routing header
	StaticBuffer<unsigned char, MTU> _frame; //Frame buffer
};

} /* namespace wanhive */

#endif /* WH_UTIL_FRAME_H_ */
