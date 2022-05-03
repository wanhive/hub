/*
 * MessageAddress.h
 *
 * Message address implementation
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGEADDRESS_H_
#define WH_UTIL_MESSAGEADDRESS_H_
#include <cstdint>

namespace wanhive {

class MessageAddress {
public:
	MessageAddress() noexcept;
	MessageAddress(uint64_t source, uint64_t destination) noexcept;
	MessageAddress(const unsigned char *buffer) noexcept;
	~MessageAddress();
	//-----------------------------------------------------------------
	uint64_t getLabel() const noexcept;
	void setLabel(uint64_t label) noexcept;
	uint64_t getSource() const noexcept;
	void setSource(uint64_t source) noexcept;
	uint64_t getDestination() const noexcept;
	void setDestination(uint64_t destination) noexcept;

	void getAddress(uint64_t &source, uint64_t &destination) const noexcept;
	void setAddress(uint64_t source, uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	void readAddress(const unsigned char *buffer) noexcept;
	void writeAddress(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	static uint64_t readLabel(const unsigned char *buffer) noexcept;
	static void writeLabel(unsigned char *buffer, uint64_t label) noexcept;
	static uint64_t readSource(const unsigned char *buffer) noexcept;
	static void writeSource(unsigned char *buffer, uint64_t source) noexcept;
	static uint64_t readDestination(const unsigned char *buffer) noexcept;
	static void writeDestination(unsigned char *buffer,
			uint64_t destination) noexcept;
public:
	/** Serialized object size in bytes */
	static constexpr unsigned int SIZE = 24;
private:
	uint64_t label { }; //Application dependent
	uint64_t source; //Source identifier
	uint64_t destination; //Destination identifier
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGEADDRESS_H_ */
