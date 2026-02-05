/*
 * Packet.cpp
 *
 * Message packet
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
			&& (header().getLength() >= HLEN);
}

unsigned int Packet::getPayloadLength() const noexcept {
	if (validate()) {
		return header().getLength() - HLEN;
	} else {
		return 0;
	}
}

bool Packet::testLength() const noexcept {
	return testLength(header().getLength());
}

bool Packet::testLength(unsigned int length) noexcept {
	return (length >= HLEN && length <= MTU);
}

unsigned int Packet::packets(unsigned int bytes) noexcept {
	return ((unsigned long long) bytes + MPS - 1) / MPS;
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

bool Packet::sign(Trust *pki) noexcept {
	if (!pki) {
		return true;
	}

	bool fixed { };
	auto siglen = pki->fingerprint(fixed);
	const auto length = header().getLength();
	if (!validate() || !fixed || (length + siglen) > MTU) {
		return false;
	}

	//Finalize the frame, otherwise verification will fail
	header().setLength(length + siglen);
	bind();

	Data message { buffer(), length };
	Cache signature { buffer(length), siglen };
	if (pki->sign(message, signature)) {
		return true;
	} else {
		//Roll back
		header().setLength(length);
		bind();
		return false;
	}
}

bool Packet::verify(Trust *pki) const noexcept {
	if (!pki) {
		return true;
	}

	bool fixed { };
	auto siglen = pki->fingerprint(fixed);
	if (fixed && getPayloadLength() >= siglen) {
		auto length = header().getLength() - siglen;
		Data message { buffer(), length };
		Data signature { buffer(length), siglen };
		return pki->verify(message, signature);
	} else {
		return false;
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
