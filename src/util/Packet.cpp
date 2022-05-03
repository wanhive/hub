/*
 * Packet.cpp
 *
 * The message packet
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

Packet::Packet(uint64_t origin) noexcept :
		Frame { origin } {

}

Packet::~Packet() {

}

bool Packet::packHeader(const MessageHeader &header) noexcept {
	auto length = header.getLength();
	if (testLength(length) && (frame().getIndex() == 0)
			&& frame().setLimit(length)) {
		return header.write(frame().array());
	} else {
		return false;
	}
}

bool Packet::packHeader() noexcept {
	return packHeader(header());
}

bool Packet::unpackHeader(MessageHeader &header) const noexcept {
	return header.read(frame().array());
}

bool Packet::unpackHeader() noexcept {
	auto length = MessageHeader::readLength(frame().array());
	return testLength(length) && (frame().getIndex() == 0)
			&& frame().setLimit(length) && unpackHeader(header());
}

bool Packet::bind(unsigned int length) noexcept {
	if (testLength(length) && (frame().getIndex() == 0)
			&& frame().setLimit(length)) {
		MessageHeader::writeLength(frame().array(), length);
		return true;
	} else {
		return false;
	}
}

bool Packet::bind() noexcept {
	return bind(header().getLength());
}

bool Packet::validate() const noexcept {
	return (frame().getIndex() == 0)
			&& (frame().getLimit() == header().getLength())
			&& (header().getLength() >= HEADER_SIZE);
}

unsigned int Packet::getPayloadLength() const noexcept {
	if (testLength()) {
		return header().getLength() - HEADER_SIZE;
	} else {
		return 0;
	}
}

bool Packet::testLength() const noexcept {
	return testLength(header().getLength());
}

bool Packet::testLength(unsigned int length) noexcept {
	return (length >= HEADER_SIZE && length <= MTU);
}

unsigned int Packet::packets(unsigned int bytes) noexcept {
	return ((unsigned long long) bytes + PAYLOAD_SIZE - 1) / PAYLOAD_SIZE;
}

bool Packet::checkContext(const MessageHeader &header, uint8_t command,
		uint8_t qualifier) noexcept {
	return header.getCommand() == command && header.getQualifier() == qualifier;
}

bool Packet::checkContext(uint8_t command, uint8_t qualifier) const noexcept {
	return checkContext(header(), command, qualifier);
}

bool Packet::checkContext(const MessageHeader &header, uint8_t command,
		uint8_t qualifier, uint8_t status) noexcept {
	return checkContext(header, command, qualifier)
			&& header.getStatus() == status;
}

bool Packet::checkContext(uint8_t command, uint8_t qualifier,
		uint8_t status) const noexcept {
	return checkContext(header(), command, qualifier, status);
}

bool Packet::sign(const PKI *pki) noexcept {
	if (pki && validate()
			&& header().getLength() <= (MTU - PKI::SIGNATURE_LENGTH)) {
		const auto length = header().getLength(); //To roll back

		//Finalize the frame, otherwise verification will fail
		header().setLength(length + PKI::SIGNATURE_LENGTH);
		bind();

		if (pki->sign(buffer(), length, (Signature*) buffer(length))) {
			return true;
		} else {
			//Roll back
			header().setLength(length);
			bind();
			return false;
		}
	} else {
		return !pki;
	}
}

bool Packet::verify(const PKI *pki) const noexcept {
	if (pki && validate() && getPayloadLength() >= PKI::SIGNATURE_LENGTH) {
		auto length = header().getLength() - PKI::SIGNATURE_LENGTH;
		return pki->verify(buffer(), length, (const Signature*) buffer(length));
	} else {
		return !pki;
	}
}

void Packet::printHeader(bool deep) const noexcept {
	if (deep) {
		MessageHeader header;
		unpackHeader(header);
		header.print();
	} else {
		this->header().print();
	}
}

} /* namespace wanhive */
