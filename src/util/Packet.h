/*
 * Packet.h
 *
 * Message formatter for blocking IO
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
 * Message formatter for blocking IO
 */
class Packet {
public:
	Packet() noexcept;
	~Packet();
	//-----------------------------------------------------------------
	/**
	 * For efficient implementation of additional protocols in the subclasses.
	 * These methods violate encapsulation and must be used with great care
	 * because incorrect usage can introduce hard to debug issues.
	 */
	//Reference to the routing header
	MessageHeader& header() noexcept;
	//Constant reference to the routing header
	const MessageHeader& header() const noexcept;
	//Pointer to <offset> within the IO buffer (nullptr on overflow)
	unsigned char* buffer(unsigned int offset = 0) noexcept;
	//Constant pointer to <offset> within the IO buffer (nullptr on overflow)
	const unsigned char* buffer(unsigned int offset = 0) const noexcept;
	//Pointer to payload's <offset> within the IO buffer (nullptr on overflow)
	unsigned char* payload(unsigned int offset = 0) noexcept;
	//Constant pointer to payload's <offset> within the IO buffer (nullptr on overflow)
	const unsigned char* payload(unsigned int offset = 0) const noexcept;
	//-----------------------------------------------------------------
	//Copy and serialize the <header> and the <payload> (can be nullptr)
	bool pack(const MessageHeader &header,
			const unsigned char *payload) noexcept;
	//Copy the full message consisting of serialized header and payload
	bool pack(const unsigned char *message) noexcept;
	//Message length is automatically calculated, <format> can be nullptr
	bool pack(const MessageHeader &header, const char *format, ...) noexcept;
	bool pack(const MessageHeader &header, const char *format,
			va_list ap) noexcept;
	//Append the data at the end of the message and update message's length
	bool append(const char *format, ...) noexcept;
	bool append(const char *format, va_list ap) noexcept;

	//Extract the message header from the IO buffer
	void unpack(MessageHeader &header) const noexcept;
	//Deserialize the incoming payload data
	bool unpack(const char *format, ...) const noexcept;
	bool unpack(const char *format, va_list ap) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Following methods pack and unpack a message (header and payload) to and
	 * from the <buffer>. <format> specifies the message payload format. If the
	 * payload is empty then the format must be nullptr. All the functions return
	 * the number of bytes transferred to/from the <buffer>, 0 on error.
	 * NOTE 1: <buffer> should point to valid memory of sufficient size.
	 * NOTE 2: The format string follows the Serializer class.
	 */
	//Message length is always automatically calculated
	static unsigned int pack(const MessageHeader &header, unsigned char *buffer,
			const char *format, ...) noexcept;
	static unsigned int pack(const MessageHeader &header, unsigned char *buffer,
			const char *format, va_list list) noexcept;

	//<header> is populated from the serialized header data in the <buffer>
	static unsigned int unpack(MessageHeader &header,
			const unsigned char *buffer, const char *format, ...) noexcept;
	static unsigned int unpack(MessageHeader &header,
			const unsigned char *buffer, const char *format,
			va_list list) noexcept;
public:
private:
	MessageHeader _header; //Routing header
	unsigned char _buffer[Message::MTU]; //IO buffer
};

} /* namespace wanhive */

#endif /* SRC_UTIL_PACKET_H_ */
