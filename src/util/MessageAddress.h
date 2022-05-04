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
/**
 * Message's address (label, source, destination)
 */
class MessageAddress {
public:
	/**
	 * Default constructor: zeroes (0) out all the fields.
	 */
	MessageAddress() noexcept;
	/**
	 * Constructor: initializes the source and destination identifiers to
	 * the given values. Sets the label to zero (0).
	 * @param source the source identifier
	 * @param destination the destination identifier
	 */
	MessageAddress(uint64_t source, uint64_t destination) noexcept;
	/**
	 * Constructor: uses serialized data for initialization.
	 * @param buffer pointer to the serialized object
	 */
	MessageAddress(const unsigned char *buffer) noexcept;
	/**
	 * Destructor
	 */
	~MessageAddress();
	//-----------------------------------------------------------------
	/**
	 * Returns the label.
	 * @return the label (application dependent)
	 */
	uint64_t getLabel() const noexcept;
	/**
	 * Sets the label.
	 * @param label the new label
	 */
	void setLabel(uint64_t label) noexcept;
	/**
	 * Returns the source identifier.
	 * @return the source identifier
	 */
	uint64_t getSource() const noexcept;
	/**
	 * Sets the source identifier.
	 * @param source the new source identifier
	 */
	void setSource(uint64_t source) noexcept;
	/**
	 * Returns the destination identifier.
	 * @return the destination identifier
	 */
	uint64_t getDestination() const noexcept;
	/**
	 * Sets the destination identifier.
	 * @param destination the new destination identifier
	 */
	void setDestination(uint64_t destination) noexcept;

	/**
	 * Returns the source and destination identifiers.
	 * @param source object for storing the source identifier
	 * @param destination object for storing the destination identifier
	 */
	void getAddress(uint64_t &source, uint64_t &destination) const noexcept;
	/**
	 * Sets the source and destination identifiers (label isn't modified).
	 * @param source the new source identifier
	 * @param destination the new destination identifier
	 */
	void setAddress(uint64_t source, uint64_t destination) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the serialized data and stores them into this object.
	 * @param buffer pointer to the serialized object
	 */
	void readAddress(const unsigned char *buffer) noexcept;
	/**
	 * Serializes this object into the given buffer.
	 * @param buffer pointer to the buffer for storing the result. The
	 * minimum buffer size should be MessageAddress::SIZE bytes.
	 */
	void writeAddress(unsigned char *buffer) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads the label from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the label
	 */
	static uint64_t readLabel(const unsigned char *buffer) noexcept;
	/**
	 * Writes new label into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param label the new label
	 */
	static void writeLabel(unsigned char *buffer, uint64_t label) noexcept;
	/**
	 * Reads the source identifier from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the source identifier
	 */
	static uint64_t readSource(const unsigned char *buffer) noexcept;
	/**
	 * Writes new source identifier into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param source the new source identifier
	 */
	static void writeSource(unsigned char *buffer, uint64_t source) noexcept;
	/**
	 * Reads the destination identifier from a serialized object.
	 * @param buffer pointer to the serialized object
	 * @return the destination identifier
	 */
	static uint64_t readDestination(const unsigned char *buffer) noexcept;
	/**
	 * Writes new destination identifier into a serialized object.
	 * @param buffer pointer to the serialized object
	 * @param destination the new destination identifier
	 */
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
