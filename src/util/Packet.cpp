/*
 * Packet.cpp
 *
 * Message formatter for blocking IO
 *
 *
 * Copyright (C) 2021 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Packet.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

Packet::Packet() noexcept {

}

Packet::~Packet() {

}

MessageHeader& Packet::header() noexcept {
	return _header;
}

const MessageHeader& Packet::header() const noexcept {
	return _header;
}

unsigned char* Packet::buffer(unsigned int offset) noexcept {
	if (offset < Message::MTU) {
		return (_buffer + offset);
	} else {
		return nullptr;
	}
}

const unsigned char* Packet::buffer(unsigned int offset) const noexcept {
	if (offset < Message::MTU) {
		return (_buffer + offset);
	} else {
		return nullptr;
	}
}

unsigned char* Packet::payload(unsigned int offset) noexcept {
	if (offset < Message::PAYLOAD_SIZE) {
		return (_buffer + Message::HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
}

const unsigned char* Packet::payload(unsigned int offset) const noexcept {
	if (offset < Message::PAYLOAD_SIZE) {
		return (_buffer + Message::HEADER_SIZE + offset);
	} else {
		return nullptr;
	}
}

bool Packet::pack(const MessageHeader &header,
		const unsigned char *payload) noexcept {
	if (!Message::testLength(header.getLength())) {
		return false;
	} else if (header.getLength() > Message::HEADER_SIZE && !payload) {
		return false;
	} else {
		_header = header;
		_header.serialize(_buffer);
		if (payload) {
			memcpy(_buffer + Message::HEADER_SIZE, payload,
					_header.getLength() - Message::HEADER_SIZE);
		}
		return true;
	}
}

bool Packet::pack(const unsigned char *message) noexcept {
	if (message) {
		_header.deserialize(message);
		if (Message::testLength(_header.getLength())) {
			memcpy(_buffer, message, _header.getLength());
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool Packet::pack(const MessageHeader &header, const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = pack(header, format, ap);
	va_end(ap);
	return ret;
}

bool Packet::pack(const MessageHeader &header, const char *format,
		va_list ap) noexcept {
	_header = header;
	auto size = pack(_header, _buffer, format, ap);
	_header.setLength(size);
	return (size != 0);
}

bool Packet::append(const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = append(format, ap);
	va_end(ap);
	return ret;
}

bool Packet::append(const char *format, va_list ap) noexcept {
	if (!Message::testLength(_header.getLength())
			|| MessageHeader::getLength(_buffer) != _header.getLength()) {
		//Incorrect message length
		return false;
	}

	auto size = Serializer::vpack(_buffer + _header.getLength(),
			Message::MTU - _header.getLength(), format, ap);
	if (size) {
		_header.setLength(_header.getLength() + size);
		MessageHeader::setLength(_buffer, _header.getLength());
		return true;
	} else {
		return false;
	}
}

void Packet::unpack(MessageHeader &header) const noexcept {
	header.deserialize(_buffer);
}

bool Packet::unpack(const char *format, ...) const noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = unpack(format, ap);
	va_end(ap);
	return ret;
}

bool Packet::unpack(const char *format, va_list ap) const noexcept {
	auto msgLength = _header.getLength();
	if (format && Message::testLength(msgLength)
			&& MessageHeader::getLength(_buffer) == msgLength) {
		return Serializer::vunpack(_buffer + Message::HEADER_SIZE,
				(msgLength - Message::HEADER_SIZE), format, ap);
	} else {
		return false;
	}
}

unsigned int Packet::pack(const MessageHeader &header, unsigned char *buffer,
		const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto size = pack(header, buffer, format, ap);
	va_end(ap);
	return size;
}

unsigned int Packet::pack(const MessageHeader &header, unsigned char *buffer,
		const char *format, va_list ap) noexcept {
	if (buffer) {
		auto size = header.serialize(buffer);
		size += Serializer::vpack(buffer + Message::HEADER_SIZE,
				Message::PAYLOAD_SIZE, format, ap);
		if (format && format[0] && size == Message::HEADER_SIZE) {
			//Payload formatting error
			return 0;
		} else {
			MessageHeader::setLength(buffer, size);
			return size;
		}
	} else {
		return 0;
	}
}

unsigned int Packet::unpack(MessageHeader &header, const unsigned char *buffer,
		const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto size = unpack(header, buffer, format, ap);
	va_end(ap);
	return size;
}

unsigned int Packet::unpack(MessageHeader &header, const unsigned char *buffer,
		const char *format, va_list ap) noexcept {
	if (buffer) {
		auto size = header.deserialize(buffer);
		auto msgLength = header.getLength();
		if (Message::testLength(msgLength)) {
			size += Serializer::vunpack(buffer + Message::HEADER_SIZE,
					(msgLength - Message::HEADER_SIZE), format, ap);
			return size;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

} /* namespace wanhive */
