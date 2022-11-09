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
 * Message context implementation
 */
class MessageContext {
public:
	/**
	 * Default constructor: zero-initializes the data members.
	 */
	MessageContext() noexcept;
	/**
	 * Constructor: initializes data members with the given values.
	 * @param command command's value
	 * @param qualifier qualifier's value
	 * @param status status code's value
	 */
	MessageContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	/**
	 * Constructor: extracts values from a serialized object.
	 * @param data serialized object
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
	void getContext(uint8_t &command, uint8_t &qualifier,
			uint8_t &status) const noexcept;
	/**
	 * Sets command, qualifier, and status values.
	 * @param command new command
	 * @param qualifier new qualifier
	 * @param status new status code
	 */
	void setContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Extracts values from a serialized object.
	 * @param data serialized object
	 */
	void readContext(const unsigned char *data) noexcept;
	/**
	 * Translates context data into a serialized object.
	 * @param data output buffer (MessageContext::SIZE is the minimum required
	 * buffer size in bytes).
	 */
	void writeContext(unsigned char *data) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads serialized object's command.
	 * @param data serialized object
	 * @return command
	 */
	static uint8_t readCommand(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's command.
	 * @param data serialized object
	 * @param command new command
	 */
	static void writeCommand(unsigned char *data, uint8_t command) noexcept;
	/**
	 * Reads serialized object's qualifier.
	 * @param data serialized object
	 * @return qualifier
	 */
	static uint8_t readQualifier(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's qualifier.
	 * @param data serialized object
	 * @param qualifier new qualifier
	 */
	static void writeQualifier(unsigned char *data, uint8_t qualifier) noexcept;
	/**
	 * Reads serialized object's status code.
	 * @param data serialized object
	 * @return status code
	 */
	static uint8_t readStatus(const unsigned char *data) noexcept;
	/**
	 * Updates serialized object's status code.
	 * @param data serialized object
	 * @param status new status code
	 */
	static void writeStatus(unsigned char *data, uint8_t status) noexcept;
public:
	/** Serialized object's size in bytes */
	static constexpr unsigned int SIZE = 3;
private:
	uint8_t command; //Command
	uint8_t qualifier; //Command's type
	uint8_t status; //Command's status
};

} /* namespace wanhive */

#endif /* WH_UTIL_MESSAGECONTEXT_H_ */
