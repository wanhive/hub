/*
 * Message.cpp
 *
 * Message implementation
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
MemoryPool Message::pool;
Message::Message(uint64_t origin) noexcept :
		referenceCount(0), ttl(0), origin(origin) {

}

Message::~Message() {

}

void* Message::operator new(size_t size) noexcept {
	return pool.allocate();
}

void Message::operator delete(void *p) noexcept {
	pool.deallocate(p);
}

Message* Message::create(uint64_t origin) noexcept {
	if (allocated() != poolSize()) {
		return new Message(origin);
	} else {
		return nullptr;
	}
}

void Message::recycle(Message *p) noexcept {
	if (!p || !p->referenceCount || !(--p->referenceCount)) { //:-)
		delete p;
	}
}

void Message::clear() noexcept {
	State::clear();
	_header.clear();
	_buffer.clear();
}

bool Message::validate() const noexcept {
	return (_buffer.getIndex() == 0) && (_buffer.getLimit() == getLength())
			&& (getLength() >= HEADER_SIZE);
}

uint64_t Message::getOrigin() const noexcept {
	return origin;
}

uint64_t Message::getLabel() const noexcept {
	return _header.getLabel();
}
void Message::setLabel(uint64_t label) noexcept {
	_header.setLabel(label);
}
void Message::updateLabel(uint64_t label) noexcept {
	MessageHeader::setLabel(_buffer.array(), label);
}
void Message::putLabel(uint64_t label) noexcept {
	setLabel(label);
	updateLabel(label);
}

uint64_t Message::getSource() const noexcept {
	return _header.getSource();
}
void Message::setSource(uint64_t source) noexcept {
	_header.setSource(source);
}
void Message::updateSource(uint64_t source) noexcept {
	MessageHeader::setSource(_buffer.array(), source);
}
void Message::putSource(uint64_t source) noexcept {
	setSource(source);
	updateSource(source);
}

uint64_t Message::getDestination() const noexcept {
	return _header.getDestination();
}
void Message::setDestination(uint64_t destination) noexcept {
	_header.setDestination(destination);
}
void Message::updateDestination(uint64_t destination) noexcept {
	MessageHeader::setDestination(_buffer.array(), destination);
}
void Message::putDestination(uint64_t destination) noexcept {
	setDestination(destination);
	updateDestination(destination);
}

uint16_t Message::getLength() const noexcept {
	return _header.getLength();
}

bool Message::testLength() const noexcept {
	return testLength(_header.getLength());
}

bool Message::setLength(uint16_t length) noexcept {
	if (testLength(length)) {
		_header.setLength(length);
		return true;
	} else {
		return false;
	}

}
bool Message::updateLength(uint16_t length) noexcept {
	if (testLength(length) && _buffer.setLimit(length)) {
		MessageHeader::setLength(_buffer.array(), length);
		return true;
	} else {
		return false;
	}
}
bool Message::putLength(uint16_t length) noexcept {
	if (testLength(length) && _buffer.setLimit(length)) {
		_header.setLength(length);
		MessageHeader::setLength(_buffer.array(), length);
		return true;
	} else {
		return false;
	}
}

uint16_t Message::getSequenceNumber() const noexcept {
	return _header.getSequenceNumber();
}
void Message::setSequenceNumber(uint16_t sequenceNumber) noexcept {
	_header.setSequenceNumber(sequenceNumber);
}
void Message::updateSequenceNumber(uint16_t sequenceNumber) noexcept {
	MessageHeader::setSequenceNumber(_buffer.array(), sequenceNumber);
}
void Message::putSequenceNumber(uint16_t sequenceNumber) noexcept {
	setSequenceNumber(sequenceNumber);
	updateSequenceNumber(sequenceNumber);
}

uint8_t Message::getSession() const noexcept {
	return _header.getSession();
}
void Message::setSession(uint8_t session) noexcept {
	_header.setSession(session);
}
void Message::updateSession(uint8_t session) noexcept {
	MessageHeader::setSession(_buffer.array(), session);
}
void Message::putSession(uint8_t session) noexcept {
	setSession(session);
	updateSession(session);
}

uint8_t Message::getCommand() const noexcept {
	return _header.getCommand();
}
void Message::setCommand(uint8_t command) noexcept {
	_header.setCommand(command);
}
void Message::updateCommand(uint8_t command) noexcept {
	MessageHeader::setCommand(_buffer.array(), command);
}
void Message::putCommand(uint8_t command) noexcept {
	setCommand(command);
	updateCommand(command);
}

uint8_t Message::getQualifier() const noexcept {
	return _header.getQualifier();
}
void Message::setQualifier(uint8_t qualifier) noexcept {
	_header.setQualifier(qualifier);
}
void Message::updateQualifier(uint8_t qualifier) noexcept {
	MessageHeader::setQualifier(_buffer.array(), qualifier);
}
void Message::putQualifier(uint8_t qualifier) noexcept {
	setQualifier(qualifier);
	updateQualifier(qualifier);
}

uint8_t Message::getStatus() const noexcept {
	return _header.getStatus();
}
void Message::setStatus(uint8_t status) noexcept {
	_header.setStatus(status);
}
void Message::updateStatus(uint8_t status) noexcept {
	MessageHeader::setStatus(_buffer.array(), status);
}
void Message::putStatus(uint8_t status) noexcept {
	setStatus(status);
	updateStatus(status);
}

void Message::getHeader(MessageHeader &header) const noexcept {
	header = this->_header;
}
bool Message::setHeader(uint64_t source, uint64_t destination, uint16_t length,
		uint16_t sequenceNumber, uint8_t session, uint8_t command,
		uint8_t qualifier, uint8_t status, uint64_t label) noexcept {
	if (testLength(length)) {
		_header.load(source, destination, length, sequenceNumber, session,
				command, qualifier, status, label);
		return true;
	} else {
		return false;
	}
}
bool Message::setHeader(const MessageHeader &header) noexcept {
	if (testLength(header.getLength())) {
		this->_header = header;
		return true;
	} else {
		return false;
	}
}
bool Message::updateHeader(uint64_t source, uint64_t destination,
		uint16_t length, uint16_t sequenceNumber, uint8_t session,
		uint8_t command, uint8_t qualifier, uint8_t status,
		uint64_t label) noexcept {
	if (testLength(length) && _buffer.setLimit(length)) {
		MessageHeader::serialize(_buffer.array(), source, destination, length,
				sequenceNumber, session, command, qualifier, status, label);
		return true;
	} else {
		return false;
	}
}
bool Message::updateHeader(const MessageHeader &header) noexcept {
	if (testLength(header.getLength())
			&& _buffer.setLimit(header.getLength())) {
		header.serialize(_buffer.array());
		return true;
	} else {
		return false;
	}
}
bool Message::putHeader(uint64_t source, uint64_t destination, uint16_t length,
		uint16_t sequenceNumber, uint8_t session, uint8_t command,
		uint8_t qualifier, uint8_t status, uint64_t label) noexcept {
	if (testLength(length) && _buffer.setLimit(length)) {
		_header.load(source, destination, length, sequenceNumber, session,
				command, qualifier, status, label);
		MessageHeader::serialize(_buffer.array(), source, destination, length,
				sequenceNumber, session, command, qualifier, status, label);
		return true;
	} else {
		return false;
	}
}
bool Message::putHeader(const MessageHeader &header) noexcept {
	if (testLength(header.getLength())
			&& _buffer.setLimit(header.getLength())) {
		this->_header = header;
		header.serialize(_buffer.array());
		return true;
	} else {
		return false;
	}
}

uint16_t Message::getPayloadLength() const noexcept {
	if (testLength()) {
		return _header.getLength() - HEADER_SIZE;
	} else {
		return 0;
	}
}

void Message::unpackHeader(MessageHeader &header) const noexcept {
	header.deserialize(_buffer.array());
}

uint64_t Message::getData64(unsigned int index) const noexcept {
	uint64_t data = 0;
	getData64(index, data);
	return data;
}
bool Message::getData64(unsigned int index, uint64_t &data) const noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint64_t))) {
		auto offset = HEADER_SIZE + index;
		data = Serializer::unpacku64(_buffer.array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData64(unsigned int index, uint64_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint64_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi64((_buffer.array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData64(uint64_t data) noexcept {
	auto offset = getLength();
	if (putLength(offset + sizeof(uint64_t))) {
		Serializer::packi64((_buffer.array() + offset), data);
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
		data = Serializer::unpacku32(_buffer.array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData32(unsigned int index, uint32_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint32_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi32((_buffer.array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData32(uint32_t data) noexcept {
	auto offset = getLength();
	if (putLength(offset + sizeof(uint32_t))) {
		Serializer::packi32((_buffer.array() + offset), data);
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
		data = Serializer::unpacku16(_buffer.array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData16(unsigned int index, uint16_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint16_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi16((_buffer.array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData16(uint16_t data) noexcept {
	auto offset = getLength();
	if (putLength(offset + sizeof(uint16_t))) {
		Serializer::packi16((_buffer.array() + offset), data);
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
		data = Serializer::unpacku8(_buffer.array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setData8(unsigned int index, uint8_t data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint8_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packi8((_buffer.array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendData8(uint8_t data) noexcept {
	auto offset = getLength();
	if (putLength(offset + sizeof(uint8_t))) {
		Serializer::packi8((_buffer.array() + offset), data);
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
		data = Serializer::unpackf64(_buffer.array() + offset);
		return true;
	} else {
		return false;
	}
}
bool Message::setDouble(unsigned int index, double data) noexcept {
	if (index <= (PAYLOAD_SIZE - sizeof(uint64_t))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packf64((_buffer.array() + offset), data);
		return true;
	} else {
		return false;
	}
}
bool Message::appendDouble(double data) noexcept {
	auto offset = getLength();
	if (putLength(offset + sizeof(uint64_t))) {
		Serializer::packf64((_buffer.array() + offset), data);
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
		Serializer::unpackib(data, (_buffer.array() + offset), length);
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
		return _buffer.array() + offset;
	} else {
		return nullptr;
	}
}
bool Message::setBytes(unsigned int index, const unsigned char *data,
		unsigned int length) noexcept {
	if (length && data && (length <= PAYLOAD_SIZE)
			&& (index <= (PAYLOAD_SIZE - length))) {
		auto offset = HEADER_SIZE + index;
		Serializer::packib((_buffer.array() + offset), data, length);
		return true;
	} else if (!length) {
		return true;
	} else {
		return false;
	}
}
bool Message::appendBytes(const unsigned char *data,
		unsigned int length) noexcept {
	auto offset = getLength();
	if (length && data && (length <= PAYLOAD_SIZE)
			&& putLength(offset + length)) {
		Serializer::packib((_buffer.array() + offset), data, length);
		return true;
	} else if (!length) {
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
	this->_header = header;
	this->_header.setLength(0); //Length will be calculated

	auto size = this->_header.serialize(_buffer.array());
	size += Serializer::vpack(_buffer.array() + HEADER_SIZE, PAYLOAD_SIZE,
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
		header.deserialize(message);

		if (!putHeader(header)) {
			return false;
		}

		if (getPayloadLength()) {
			setBytes(0, message + HEADER_SIZE, getPayloadLength());
		}
		return true;
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

	auto size = Serializer::vpack(_buffer.array() + getLength(),
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
	if (format && format[0] && validate()) {
		return Serializer::vunpack(_buffer.array() + HEADER_SIZE,
				getPayloadLength(), format, ap);
	} else {
		return false;
	}
}

void Message::printHeader(bool deep) const noexcept {
	if (deep) {
		MessageHeader header;
		header.deserialize(_buffer.array());
		header.print();
	} else {
		this->_header.print();
	}
}

bool Message::testLength(unsigned int length) noexcept {
	return (length >= HEADER_SIZE && length <= MTU);
}

unsigned int Message::packets(unsigned int bytes) noexcept {
	return ((unsigned long long) bytes + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE;
}

bool Message::available(unsigned int count) noexcept {
	return count <= unallocated();
}

MessageHeader& Message::header() noexcept {
	return this->_header;
}

const MessageHeader& Message::header() const noexcept {
	return this->_header;
}

unsigned char* Message::buffer() noexcept {
	return _buffer.array();
}

const unsigned char* Message::buffer() const noexcept {
	return _buffer.array();
}

bool Message::build(Source<unsigned char> &in) {
	switch (getFlags()) {
	case 0:
		/* no break */
	case MSG_WAIT_HEADER:
		if (in.available() >= Message::HEADER_SIZE) {
			_buffer.clear();
			in.take(_buffer.offset(), Message::HEADER_SIZE);
			//Prepare the routing header
			_header.deserialize(_buffer.array());
			_buffer.setIndex(HEADER_SIZE);
			putFlags(MSG_WAIT_DATA);
		} else {
			return false;
		}
		/* no break */
	case MSG_WAIT_DATA:
		if (testLength()) {
			auto payLoadLength = getPayloadLength();
			if (in.available() >= payLoadLength) {
				in.take(_buffer.offset(), payLoadLength);
				//Set the correct limit and index
				_buffer.setIndex(_header.getLength());
				_buffer.rewind();
				putFlags(MSG_WAIT_PROCESSING);
				return true;
			} else {
				return false;
			}
		} else {
			throw Exception(EX_INVALIDRANGE);
		}
		/* no break */
	case MSG_WAIT_PROCESSING:
		return true;
	default:
		throw Exception(EX_INVALIDSTATE);
	}
}

unsigned int Message::addReferenceCount() noexcept {
	return ++referenceCount;
}

unsigned int Message::addTTL() noexcept {
	return ++ttl;
}

void Message::initPool(unsigned int size) {
	pool.initialize(sizeof(Message), size);
}

void Message::destroyPool() {
	if (pool.destroy()) {
		throw Exception(EX_INVALIDSTATE);
	}
}

unsigned int Message::poolSize() noexcept {
	return pool.capacity();
}

unsigned int Message::allocated() noexcept {
	return pool.allocated();
}

unsigned int Message::unallocated() noexcept {
	//How many more messages can we create
	return poolSize() - allocated();
}

} /* namespace wanhive */
