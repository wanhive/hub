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
 * An immutable chunk of binary data.
 */
struct Data {
	const unsigned char *base; /**< The base pointer */
	unsigned int length; /**< Binary data's size in bytes */
};

/**
 * The data unit for packet implementation. It consists of a routing header
 * and a frame buffer. The routing header is used locally for decision making
 * during packet delivery and forwarding while the frame buffer contains the
 * serialized binary data that is exchanged via the network.
 * Frame buffer's structure: [{MESSAGE HEADER}{VARIABLE LENGTH PAYLOAD}]
 * Routing header is an object of the type MessageHeader.
 *
 * Thread safe at class level.
 */
class Frame {
public:
	/**
	 * Constructor
	 *
	 * @param origin the local origin identifier
	 */
	Frame(uint64_t origin = 0) noexcept;
	/**
	 * Destructor
	 */
	~Frame();

	/**
	 * Returns the local origin identifier.
	 *
	 * @return the local origin identifier
	 */
	uint64_t getOrigin() const noexcept;

	/**
	 * Returns a reference to the routing header. Please note that any change
	 * made in the routing header is not automatically synchronized with the
	 * serialized header data contained at the beginning of the frame buffer and
	 * vice versa.
	 *
	 * @return reference to the routing header
	 */
	MessageHeader& header() noexcept;

	/**
	 * Returns a constant reference to the routing header. Please note that the
	 * routing header data may not be in agreement with the serialized header
	 * data contained at the beginning of the frame buffer.
	 *
	 * @return constant reference to the routing header
	 */
	const MessageHeader& header() const noexcept;

	/**
	 * Returns a pointer to the given offset from the beginning of the frame
	 * buffer's backing array (serialized data). Any changes made through this
	 * pointer are not reflected inside the routing header.
	 *
	 * @param offset the desired offset in bytes (should be less than the MTU)
	 * @return a pointer to the given offset inside the backing array if the
	 * offset is valid, nullptr otherwise.
	 */
	unsigned char* buffer(unsigned int offset = 0) noexcept;

	/**
	 * Returns a constant pointer to the given offset from the beginning of the
	 * frame buffer's backing array which stores the serialized data as a sequence
	 * of bytes.
	 *
	 * @param offset the desired offset in bytes (should be less than the MTU)
	 *
	 * @return a constant pointer to the given offset inside the backing array
	 * if the offset is valid, nullptr otherwise.
	 */
	const unsigned char* buffer(unsigned int offset = 0) const noexcept;

	/**
	 * Returns a pointer to the given offset from the beginning of the frame
	 * buffer's payload section within it's backing array. This call is equivalent
	 * to buffer(offset + HEADER_SIZE).
	 *
	 * @param offset the desired offset within the payload section (should be
	 * less than PAYLOAD_SIZE).
	 * @return a pointer to the given offset inside the payload section if the
	 * offset is valid, nullptr otherwise.
	 */
	unsigned char* payload(unsigned int offset = 0) noexcept;

	/**
	 * Returns a constant pointer to the given offset from the beginning of the
	 * frame buffer's payload section within it's backing array. This call is
	 * equivalent to buffer(offset + HEADER_SIZE).
	 *
	 * @param offset the desired offset within the payload section (should be
	 * less than PAYLOAD_SIZE).
	 * @return a constant pointer to the given offset inside the payload section
	 * if the offset is valid, nullptr otherwise.
	 */
	const unsigned char* payload(unsigned int offset = 0) const noexcept;

	/**
	 * Clears out the routing header and the frame buffer.
	 */
	void clear() noexcept;
protected:
	/**
	 * For traffic shaping: returns the hop count.
	 *
	 * @return the hop count
	 */
	unsigned int getHopCount() const noexcept;

	/**
	 * For traffic shaping: sets the hop count.
	 *
	 * @param hopCount the desired hop count
	 */
	void setHopCount(unsigned int hopCount) noexcept;

	/**
	 * For multicasting: returns the reference count.
	 *
	 * @return the reference count
	 */
	unsigned int getReferenceCount() const noexcept;

	/**
	 * For multicasting: sets the reference count.
	 *
	 * @param referenceCount the desired reference count
	 */
	void setReferenceCount(unsigned int referenceCount) noexcept;

	/**
	 * Returns a constant reference to the frame buffer.
	 *
	 * @return a constant reference to the frame buffer
	 */
	auto& frame() const noexcept {
		return _frame;
	}

	/**
	 * Returns a reference to the frame buffer.
	 *
	 * @return a reference to the frame buffer
	 */
	auto& frame() noexcept {
		return _frame;
	}
public:
	/** Header size in bytes */
	static constexpr unsigned int HEADER_SIZE = MessageHeader::SIZE;
	/** The maximum frame buffer size in bytes */
	static constexpr unsigned int MTU = 1024;
	/** The maximum payload size in bytes */
	static constexpr unsigned int PAYLOAD_SIZE = (MTU - HEADER_SIZE);
private:
	unsigned int hopCount; //Hop count
	unsigned int referenceCount; //Reference count
	const uint64_t origin; //The local origin identifier
	MessageHeader _header; //Routing header
	StaticBuffer<unsigned char, MTU> _frame; //The raw bytes
};

} /* namespace wanhive */

#endif /* WH_UTIL_FRAME_H_ */
