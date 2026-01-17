/*
 * Message.cpp
 *
 * Wanhive Message
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Message.h"
#include "../base/common/Exception.h"
#include "../base/ds/Serializer.h"

namespace wanhive {

Message::Message(uint64_t origin) noexcept :
		Pooled { 0 }, Packet { origin } {

}

Message::~Message() {

}

void* Message::operator new(size_t size) noexcept {
	return Pooled::operator new(size);
}

void Message::operator delete(void *p) noexcept {
	Pooled::operator delete(p);
}

Message* Message::create(uint64_t origin) noexcept {
	if (allocated() != poolSize()) {
		return new Message(origin);
	} else {
		return nullptr;
	}
}

bool Message::recycle(Message *message) noexcept {
	if (!message || message->getLinks() <= 1) {
		delete message;
		return true;
	} else {
		message->setLinks(message->getLinks() - 1);
		return false;
	}
}

void Message::clear() noexcept {
	State::clear();
	Frame::clear();
}

bool Message::build(Source<unsigned char> &in) {
	switch (getFlags()) {
	case 0:
		/* no break */
	case MSG_WAIT_HEADER:
		if (in.available() >= Message::HEADER_SIZE) {
			frame().clear();
			in.emit(frame().offset(), Message::HEADER_SIZE);
			//Prepare the routing header
			header().read(frame().array());
			frame().setIndex(HEADER_SIZE);
			putFlags(MSG_WAIT_DATA);
		} else {
			return false;
		}
		/* no break */
	case MSG_WAIT_DATA:
		if (testLength()) {
			auto payLoadLength = header().getLength() - HEADER_SIZE;
			if (in.available() >= payLoadLength) {
				in.emit(frame().offset(), payLoadLength);
				//Set the correct limit and index
				frame().setIndex(header().getLength());
				frame().rewind();
				putFlags(MSG_WAIT_PROCESSING);
				return true;
			} else {
				return false;
			}
		} else {
			throw Exception(EX_RANGE);
		}
		/* no break */
	case MSG_WAIT_PROCESSING:
		return true;
	default:
		throw Exception(EX_STATE);
	}
}

uint64_t Message::getLabel() const noexcept {
	return header().getLabel();
}
void Message::setLabel(uint64_t label) noexcept {
	header().setLabel(label);
}
void Message::writeLabel(uint64_t label) noexcept {
	MessageHeader::writeLabel(frame().array(), label);
}
void Message::putLabel(uint64_t label) noexcept {
	setLabel(label);
	writeLabel(label);
}

uint64_t Message::getSource() const noexcept {
	return header().getSource();
}
void Message::setSource(uint64_t source) noexcept {
	header().setSource(source);
}
void Message::writeSource(uint64_t source) noexcept {
	MessageHeader::writeSource(frame().array(), source);
}
void Message::putSource(uint64_t source) noexcept {
	setSource(source);
	writeSource(source);
}

uint64_t Message::getDestination() const noexcept {
	return header().getDestination();
}
void Message::setDestination(uint64_t destination) noexcept {
	header().setDestination(destination);
}
void Message::writeDestination(uint64_t destination) noexcept {
	MessageHeader::writeDestination(frame().array(), destination);
}
void Message::putDestination(uint64_t destination) noexcept {
	setDestination(destination);
	writeDestination(destination);
}

uint16_t Message::getLength() const noexcept {
	return header().getLength();
}

bool Message::setLength(uint16_t length) noexcept {
	if (testLength(length)) {
		header().setLength(length);
		return true;
	} else {
		return false;
	}

}
bool Message::writeLength(uint16_t length) noexcept {
	return bind(length);
}
bool Message::putLength(uint16_t length) noexcept {
	if (writeLength(length)) {
		header().setLength(length);
		return true;
	} else {
		return false;
	}
}

uint16_t Message::getSequenceNumber() const noexcept {
	return header().getSequenceNumber();
}
void Message::setSequenceNumber(uint16_t sequenceNumber) noexcept {
	header().setSequenceNumber(sequenceNumber);
}
void Message::writeSequenceNumber(uint16_t sequenceNumber) noexcept {
	MessageHeader::writeSequenceNumber(frame().array(), sequenceNumber);
}
void Message::putSequenceNumber(uint16_t sequenceNumber) noexcept {
	setSequenceNumber(sequenceNumber);
	writeSequenceNumber(sequenceNumber);
}

uint8_t Message::getSession() const noexcept {
	return header().getSession();
}
void Message::setSession(uint8_t session) noexcept {
	header().setSession(session);
}
void Message::writeSession(uint8_t session) noexcept {
	MessageHeader::writeSession(frame().array(), session);
}
void Message::putSession(uint8_t session) noexcept {
	setSession(session);
	writeSession(session);
}

uint8_t Message::getCommand() const noexcept {
	return header().getCommand();
}
void Message::setCommand(uint8_t command) noexcept {
	header().setCommand(command);
}
void Message::writeCommand(uint8_t command) noexcept {
	MessageHeader::writeCommand(frame().array(), command);
}
void Message::putCommand(uint8_t command) noexcept {
	setCommand(command);
	writeCommand(command);
}

uint8_t Message::getQualifier() const noexcept {
	return header().getQualifier();
}
void Message::setQualifier(uint8_t qualifier) noexcept {
	header().setQualifier(qualifier);
}
void Message::writeQualifier(uint8_t qualifier) noexcept {
	MessageHeader::writeQualifier(frame().array(), qualifier);
}
void Message::putQualifier(uint8_t qualifier) noexcept {
	setQualifier(qualifier);
	writeQualifier(qualifier);
}

uint8_t Message::getStatus() const noexcept {
	return header().getStatus();
}
void Message::setStatus(uint8_t status) noexcept {
	header().setStatus(status);
}
void Message::writeStatus(uint8_t status) noexcept {
	MessageHeader::writeStatus(frame().array(), status);
}
void Message::putStatus(uint8_t status) noexcept {
	setStatus(status);
	writeStatus(status);
}

void Message::getHeader(MessageHeader &header) const noexcept {
	header = this->header();
}
bool Message::setHeader(const MessageHeader &header) noexcept {
	if (testLength(header.getLength())) {
		this->header() = header;
		return true;
	} else {
		return false;
	}
}
bool Message::writeHeader(const MessageHeader &header) noexcept {
	return packHeader(header);
}
bool Message::putHeader(const MessageHeader &header) noexcept {
	if (writeHeader(header)) {
		this->header() = header;
		return true;
	} else {
		return false;
	}
}

uint64_t Message::getData64(unsigned int index) const noexcept {
	uint64_t data = 0;
	getData64(index, data);
	return data;
}
bool Message::getData64(unsigned int index, uint64_t &data) const noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint64_t))) {
		auto offset = HEADER_SIZE + index;
		data = Serializer::unpacku64(frame().array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData64(unsigned int index, uint64_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint64_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi64((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData64(uint64_t data) noexcept {
	auto offset = getLength();
	if (validate() && putLength(offset + sizeof(uint64_t))) {
		Serializer::packi64((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}

uint32_t Message::getData32(unsigned int index) const noexcept {
	uint32_t data = 0;
	getData32(index, data);
	return data;
}
bool Message::getData32(unsigned int index, uint32_t &data) const noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint32_t))) {
		auto offset = HEADER_SIZE + index;
		data = Serializer::unpacku32(frame().array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData32(unsigned int index, uint32_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint32_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi32((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData32(uint32_t data) noexcept {
	auto offset = getLength();
	if (validate() && putLength(offset + sizeof(uint32_t))) {
		Serializer::packi32((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}

uint16_t Message::getData16(unsigned int index) const noexcept {
	uint16_t data = 0;
	getData16(index, data);
	return data;
}
bool Message::getData16(unsigned int index, uint16_t &data) const noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint16_t))) {
		auto offset = HEADER_SIZE + index;
		data = Serializer::unpacku16(frame().array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData16(unsigned int index, uint16_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint16_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi16((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData16(uint16_t data) noexcept {
	auto offset = getLength();
	if (validate() && putLength(offset + sizeof(uint16_t))) {
		Serializer::packi16((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}

uint8_t Message::getData8(unsigned int index) const noexcept {
	uint8_t data = 0;
	getData8(index, data);
	return data;
}
bool Message::getData8(unsigned int index, uint8_t &data) const noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint8_t))) {
		auto offset = HEADER_SIZE + index;
		data = Serializer::unpacku8(frame().array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData8(unsigned int index, uint8_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint8_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi8((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData8(uint8_t data) noexcept {
	auto offset = getLength();
	if (validate() && putLength(offset + sizeof(uint8_t))) {
		Serializer::packi8((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}

float Message::getFloat(unsigned int index) const noexcept {
	float data = 0;
	getFloat(index, data);
	return data;
}

bool Message::getFloat(unsigned int index, float &data) const noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint32_t))) {
		auto offset = HEADER_SIZE + index;
		data = Serializer::unpackf32(frame().array() + offset);
		return true;
	} else {
		return false;
	}
}

bool Message::setFloat(unsigned int index, float data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint32_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packf32((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}

bool Message::appendFloat(float data) noexcept {
	auto offset = getLength();
	if (validate() && putLength(offset + sizeof(uint32_t))) {
		Serializer::packf32((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}

double Message::getDouble(unsigned int index) const noexcept {
	double data = 0;
	getDouble(index, data);
	return data;
}
bool Message::getDouble(unsigned int index, double &data) const noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint64_t))) {
		auto offset = HEADER_SIZE + index;
		data = Serializer::unpackf64(frame().array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setDouble(unsigned int index, double data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint64_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packf64((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendDouble(double data) noexcept {
	auto offset = getLength();
	if (validate() && putLength(offset + sizeof(uint64_t))) {
		Serializer::packf64((frame().array() + offset), data);
		return true;
	} else {
		return false;
	}
}

bool Message::getBytes(unsigned int index, unsigned char *data,
		unsigned int length) const noexcept {
	if (length && data && (length <= PAYLOAD_SIZE)
			&& (index <= (PAYLOAD_SIZE - length))) {
		auto offset = HEADER_SIZE + index;
		Serializer::unpackib(data, (frame().array() + offset), length);
		return true;
	} else if (!length) {
		return true;
	} else {
		return false;
	}
}
const unsigned char* Message::getBytes(unsigned int index) const noexcept {
	if (index < PAYLOAD_SIZE) {
		auto offset = HEADER_SIZE + index;
		return frame().array() + offset;
	} else {
		return nullptr;
	}
}
bool Message::setBytes(unsigned int index, const unsigned char *data,
		unsigned int length) noexcept {
	if ((length > PAYLOAD_SIZE) || (index > (PAYLOAD_SIZE - length))) {
		return false;
	} else if (!length) {
		return true;
	} else if (data) {
		auto offset = HEADER_SIZE + index;
		Serializer::packib((frame().array() + offset), data, length);
		return true;
	} else {
		return false;
	}
}
bool Message::appendBytes(const unsigned char *data,
		unsigned int length) noexcept {
	auto offset = getLength();
	if (!validate()) {
		return false;
	} else if (!length) {
		return true;
	} else if (data && (length <= PAYLOAD_SIZE) && putLength(offset + length)) {
		Serializer::packib((frame().array() + offset), data, length);
		return true;
	} else {
		return false;
	}
}

bool Message::pack(const MessageHeader &header, const char *format,
		...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = pack(header, format, ap);
	va_end(ap);
	return ret;
}

bool Message::pack(const MessageHeader &header, const char *format,
		va_list ap) noexcept {
	this->header() = header;
	this->header().setLength(0); //Length will be calculated

	auto size = this->header().write(frame().array());
	size += Serializer::vpack(frame().array() + HEADER_SIZE, PAYLOAD_SIZE,
			format, ap);

	if (format && format[0] && size == HEADER_SIZE) {
		return false;
	} else {
		return putLength(size);
	}
}

bool Message::pack(const MessageHeader &header,
		const unsigned char *payload) noexcept {
	if (header.getLength() > HEADER_SIZE && !payload) {
		return false;
	} else if (!putHeader(header)) {
		return false;
	} else if (payload) {
		setBytes(0, payload, getPayloadLength());
		return true;
	} else {
		return true;
	}
}

bool Message::pack(const unsigned char *message) noexcept {
	if (message) {
		MessageHeader header;
		header.read(message);

		if (!putHeader(header)) {
			return false;
		}

		return setBytes(0, message + HEADER_SIZE, getPayloadLength());
	} else {
		return false;
	}
}

bool Message::append(const char *format, ...) noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = append(format, ap);
	va_end(ap);
	return ret;
}

bool Message::append(const char *format, va_list ap) noexcept {
	if (!format || !format[0] || !validate()) {
		return false;
	}

	auto size = Serializer::vpack(frame().array() + getLength(),
			MTU - getLength(), format, ap);
	if (size) {
		return putLength(getLength() + size);
	} else {
		return false;
	}
}

bool Message::unpack(const char *format, ...) const noexcept {
	va_list ap;
	va_start(ap, format);
	auto ret = unpack(format, ap);
	va_end(ap);
	return ret;
}

bool Message::unpack(const char *format, va_list ap) const noexcept {
	if (format && format[0]) {
		return Serializer::vunpack(frame().array() + HEADER_SIZE,
				getPayloadLength(), format, ap);
	} else {
		return false;
	}
}

bool Message::available(unsigned int count) noexcept {
	return count <= unallocated();
}

unsigned int Message::link() noexcept {
	setLinks(getLinks() + 1);
	return getLinks();
}

unsigned int Message::hop() noexcept {
	setHops(getHops() + 1);
	return getHops();
}

} /* namespace wanhive */
