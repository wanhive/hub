/*
 * MessageContext.h
 *
 * Message context implementation
 *
 *
 * Copyright (C) 2022 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_MESSAGECONTEXT_H_
#define WH_UTIL_MESSAGECONTEXT_H_
#include <cstdint>

namespace wanhive {

class MessageContext {
public:
	MessageContext() noexcept;
	MessageContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	MessageContext(const unsigned char *buffer) noexcept;
	~MessageContext();
	//-----------------------------------------------------------------
	uint8_t getCommand() const noexcept;
	void setCommand(uint8_t command) noexcept;
	uint8_t getQualifier() const noexcept;
	void setQualifier(uint8_t qualifier) noexcept;
	uint8_t getStatus() const noexcept;
	void setStatus(uint8_t status) noexcept;
	//-----------------------------------------------------------------
	void getContext(uint8_t &command, uint8_t &qualifier,
			uint8_t &status) const noexcept;
	void setContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	//-----------------------------------------------------------------
	void readContext(const unsigned char *buffer) noexcept;
	void writeContext(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	static uint8_t readCommand(const unsigned char *buffer) noexcept;
	static void writeCommand(unsigned char *buffer, uint8_t command) noexcept;
	static uint8_t readQualifier(const unsigned char *buffer) noexcept;
	static void writeQualifier(unsigned char *buffer,
			uint8_t qualifier) noexcept;
	static uint8_t readStatus(const unsigned char *buffer) noexcept;
	static void writeStatus(unsigned char *buffer, uint8_t status) noexcept;
public:
	/** Serialized object size in bytes */
	static constexpr unsigned int SIZE = 3;
private:
	uint8_t command; //Command
	uint8_t qualifier; //Command's type
	uint8_t status; //Command's status
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGECONTEXT_H_ */
