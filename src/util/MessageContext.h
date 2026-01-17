/*
 * MessageContext.h
 *
 * Message's context
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
/**
 * Message context
 */
class MessageContext {
public:
	/**
	 * Default constructor: sets all fields to zero.
	 */
	MessageContext() noexcept;
	/**
	 * Constructor: populates the context data.
	 * @param command command's value
	 * @param qualifier qualifier's value
	 * @param status status code's value
	 */
	MessageContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	/**
	 * Constructor: reads the serialized context data.
	 * @param data serialized context
	 */
	MessageContext(const unsigned char *data) noexcept;
	/**
	 * Destructor
	 */
	~MessageContext();
	//-----------------------------------------------------------------
	/**
	 * Returns the command.
	 * @return command
	 */
	uint8_t getCommand() const noexcept;
	/**
	 * Sets a new command.
	 * @param command new command
	 */
	void setCommand(uint8_t command) noexcept;
	/**
	 * Returns the qualifier.
	 * @return qualifier
	 */
	uint8_t getQualifier() const noexcept;
	/**
	 * Sets a new qualifier
	 * @param qualifier new qualifier
	 */
	void setQualifier(uint8_t qualifier) noexcept;
	/**
	 * Returns the status code.
	 * @return status code
	 */
	uint8_t getStatus() const noexcept;
	/**
	 * Sets a new status code.
	 * @param status new status code
	 */
	void setStatus(uint8_t status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns command, qualifier, and status values.
	 * @param command stores command's value
	 * @param qualifier stores qualifier's value
	 * @param status stores status code's value
	 */
	void get(uint8_t &command, uint8_t &qualifier,
			uint8_t &status) const noexcept;
	/**
	 * Sets command, qualifier, and status values.
	 * @param command new command
	 * @param qualifier new qualifier
	 * @param status new status code
	 */
	void set(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
protected:
	/**
	 * Reads the serialized context data.
	 * @param data serialized context
	 */
	void read(const unsigned char *data) noexcept;
	/**
	 * Serializes the context data.
	 * @param data output buffer
	 */
	void write(unsigned char *data) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized context's command.
	 * @param data serialized context
	 * @return command
	 */
	static uint8_t readCommand(const unsigned char *data) noexcept;
	/**
	 * Updates serialized context's command.
	 * @param data serialized context
	 * @param command new command
	 */
	static void writeCommand(unsigned char *data, uint8_t command) noexcept;
	/**
	 * Reads serialized context's qualifier.
	 * @param data serialized context
	 * @return qualifier
	 */
	static uint8_t readQualifier(const unsigned char *data) noexcept;
	/**
	 * Updates serialized context's qualifier.
	 * @param data serialized context
	 * @param qualifier new qualifier
	 */
	static void writeQualifier(unsigned char *data, uint8_t qualifier) noexcept;
	/**
	 * Reads serialized context's status code.
	 * @param data serialized context
	 * @return status code
	 */
	static uint8_t readStatus(const unsigned char *data) noexcept;
	/**
	 * Updates serialized context's status code.
	 * @param data serialized context
	 * @param status new status code
	 */
	static void writeStatus(unsigned char *data, uint8_t status) noexcept;
public:
	/*! Serialized context's size in bytes */
	static constexpr unsigned int SIZE = 3;
private:
	uint8_t command;
	uint8_t qualifier;
	uint8_t status;
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGECONTEXT_H_ */
