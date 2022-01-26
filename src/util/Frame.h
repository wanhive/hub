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
 * The message frame
 * Internal structure: [{MESSAGE HEADER}{VARIABLE LENGTH PAYLOAD}]
 */
class Frame {
public:
	Frame(uint64_t origin = 0) noexcept;
	~Frame();

	//Returns the origin identifier
	uint64_t getOrigin() const noexcept;
	//Reference to the routing header
	MessageHeader& header() noexcept;
	//Constant reference to the routing header
	const MessageHeader& header() const noexcept;
	//Pointer to the <offset> in the IO buffer (null on error)
	unsigned char* buffer(unsigned int offset = 0) noexcept;
	//Constant pointer to the <offset> in the IO buffer (null on error)
	const unsigned char* buffer(unsigned int offset = 0) const noexcept;
	//Pointer to payload's <offset> in the IO buffer (null on error)
	unsigned char* payload(unsigned int offset = 0) noexcept;
	//Constant pointer to payload's <offset> in the IO buffer (null on error)
	const unsigned char* payload(unsigned int offset = 0) const noexcept;
	//Clears out the routing header and the frame buffer
	void clear() noexcept;
protected:
	//Returns the hop count
	unsigned int getHopCount() const noexcept;
	//Sets the hop count
	void setHopCount(unsigned int hopCount) noexcept;
	//Returns the reference count
	unsigned int getReferenceCount() const noexcept;
	//Sets the reference count
	void setReferenceCount(unsigned int referenceCount) noexcept;

	//Returns a constant reference to the frame buffer
	auto& frame() const noexcept {
		return _frame;
	}

	//Returns a reference to the frame buffer
	auto& frame() noexcept {
		return _frame;
	}
public:
	//Header size in bytes
	static constexpr unsigned int HEADER_SIZE = MessageHeader::SIZE;
	//The maximum frame size in bytes
	static constexpr unsigned int MTU = 1024;
	//The maximum payload size in bytes
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
