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
/**
 * Message's context: (command, qualifier, status)
 */
class MessageContext {
public:
	/**
	 * Default constructor: zeroes (0) out all the fields.
	 */
	MessageContext() noexcept;
	/**
	 * Constructor: initializes command, qualifier and status fields to
	 * the given values.
	 * @param command the command
	 * @param qualifier the qualifier
	 * @param status the status code
	 */
	MessageContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	/**
	 * Constructor: uses serialized data for initialization.
	 * @param buffer pointer to the serialized object
	 */
	MessageContext(const unsigned char *buffer) noexcept;
	/**
	 * Destructor
	 */
	~MessageContext();
	//-----------------------------------------------------------------
	/**
	 * Returns the command classifier.
	 * @return the command
	 */
	uint8_t getCommand() const noexcept;
	/**
	 * Sets the command.
	 * @param command the new command
	 */
	void setCommand(uint8_t command) noexcept;
	/**
	 * Returns the qualifier.
	 * @return the qualifier
	 */
	uint8_t getQualifier() const noexcept;
	/**
	 * Sets the qualifier
	 * @param qualifier the new qualifier
	 */
	void setQualifier(uint8_t qualifier) noexcept;
	/**
	 * Returns the status code.
	 * @return the status code
	 */
	uint8_t getStatus() const noexcept;
	/**
	 * Sets the status code.
	 * @param status the new status code
	 */
	void setStatus(uint8_t status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the command, qualifier, and status values in a single call.
	 * @param command object for storing the command
	 * @param qualifier object for storing the qualifier
	 * @param status object for storing the status code
	 */
	void getContext(uint8_t &command, uint8_t &qualifier,
			uint8_t &status) const noexcept;
	/**
	 * Sets the command, qualifier, and status values in a single call.
	 * @param command the new command
	 * @param qualifier the new qualifier
	 * @param status the new status code
	 */
	void setContext(uint8_t command, uint8_t qualifier, uint8_t status) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the serialized data and stores them into this object.
	 * @param buffer pointer to the serialized object
	 */
	void readContext(const unsigned char *buffer) noexcept;
	/**
	 * Serializes this object into the given buffer
	 * @param buffer pointer to the buffer for storing the result. The
	 * minimum buffer size should be MessageContext::SIZE bytes.
	 */
	void writeContext(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the command from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the command
	 */
	static uint8_t readCommand(const unsigned char *buffer) noexcept;
	/**
	 * Writes new command into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param command the new command
	 */
	static void writeCommand(unsigned char *buffer, uint8_t command) noexcept;
	/**
	 * Reads the qualifier from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the qualifier
	 */
	static uint8_t readQualifier(const unsigned char *buffer) noexcept;
	/**
	 * Writes new qualifier into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param qualifier the new qualifier
	 */
	static void writeQualifier(unsigned char *buffer,
			uint8_t qualifier) noexcept;
	/**
	 * Reads the status code from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the status code
	 */
	static uint8_t readStatus(const unsigned char *buffer) noexcept;
	/**
	 * Writes new status code into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param status the new status code
	 */
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
