/*
 * Packet.h
 *
 * Message buffer for blocking IO
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef SRC_UTIL_PACKET_H_
#define SRC_UTIL_PACKET_H_
#include "Message.h"
#include "PKI.h"

namespace wanhive {
/**
 * Message buffer for blocking IO
 */
class Packet {
public:
	Packet() noexcept;
	~Packet();

	//Reference to the routing header
	MessageHeader& header() noexcept;
	//Constant reference to the routing header
	const MessageHeader& header() const noexcept;
	//Pointer to <offset> in the IO buffer (null on error)
	unsigned char* buffer(unsigned int offset = 0) noexcept;
	//Constant pointer to <offset> in the IO buffer (null on error)
	const unsigned char* buffer(unsigned int offset = 0) const noexcept;
	//Pointer to payload's <offset> in the IO buffer (null on error)
	unsigned char* payload(unsigned int offset = 0) noexcept;
	//Constant pointer to payload's <offset> in the IO buffer (null on error)
	const unsigned char* payload(unsigned int offset = 0) const noexcept;
private:
	MessageHeader _header; //Routing header
	unsigned char _buffer[Message::MTU]; //IO buffer
};

} /* namespace wanhive */

#endif /* SRC_UTIL_PACKET_H_ */
