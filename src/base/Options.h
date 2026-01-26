/**
 * @file Options.h
 *
 * Configuration management
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_OPTIONS_H_
#define WH_BASE_OPTIONS_H_
#include "common/NonCopyable.h"
#include <cstdio>

/*! @namespace wanhive */
namespace wanhive {
/**
 * Configuration manager that accepts simplified INI file format.
 * @ref https://en.wikipedia.org/wiki/INI_file
 */
class Options: private NonCopyable {
public:
	/**
	 * Constructor: initializes an empty object.
	 */
	Options() noexcept;
	/**
	 * Constructor: loads configuration data from a file.
	 * @param filename configuration file's path
	 */
	Options(const char *filename);
	/**
	 * Destructor
	 */
	~Options();
	//-----------------------------------------------------------------
	/**
	 * Clears out the configuration data.
	 */
	void clear() noexcept;
	/**
	 * Loads configuration data from a file, overwrites data on conflict.
	 * @param filename configuration file's path
	 * @param count if not nullptr then stores the processed rows count
	 * @return true on success, false on error (parsing error)
	 */
	bool load(const char *filename, size_t *count = nullptr) noexcept;
	/**
	 * Stores the configuration data in a file.
	 * @param filename configuration file's path
	 * @return true on success, false on error
	 */
	bool store(const char *filename) noexcept;
	/**
	 * Prints configuration data to a stream.
	 * @param stream output stream
	 * @param name a name/tag for the configuration data
	 * @return true on success, false on error
	 */
	bool print(FILE *stream, const char *name = nullptr) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes a property as key-value pair.
	 * @param section section name, can be empty string
	 * @param option key's name, cannot be empty string
	 * @param value key's value, cannot be empty string
	 * @return true on success, false on error
	 */
	bool setString(const char *section, const char *option,
			const char *value) noexcept;
	/**
	 * Reads a property and returns it's value as string.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param preset default value to return if the key not found
	 * @return pointer to the value (valid until a configuration update, caller
	 * must copy the string).
	 */
	const char* getString(const char *section, const char *option,
			const char *preset = nullptr) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes a property as key-value pair where value is a number.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param value key's value
	 * @return true on success, false on failure
	 */
	bool setNumber(const char *section, const char *option,
			unsigned long long value) noexcept;
	/**
	 * Reads a property and returns it's value as a number.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param preset default value to return if no valid entry found
	 * @return numerical value associated with the given key
	 */
	unsigned long long getNumber(const char *section, const char *option,
			unsigned long long preset = 0) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes a property as key-value pair where the value is decimal oriented.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param value key's value
	 * @return true on success, false on failure
	 */
	bool setDouble(const char *section, const char *option,
			double value) noexcept;
	/**
	 * Reads a property and returns it's decimal oriented value.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param preset default value to return if no valid entry found
	 * @return decimal value associated with the given key
	 */
	double getDouble(const char *section, const char *option,
			double preset = 0) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Writes a property as key-value pair where the value is of boolean type.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param value key's value
	 * @return true on success, false otherwise
	 */
	bool setBoolean(const char *section, const char *option,
			bool value) noexcept;
	/**
	 * Reads a property and returns it's boolean value.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param preset default value to return if no valid entry found
	 * @return boolean value associated with the given key
	 */
	bool getBoolean(const char *section, const char *option,
			bool preset = false) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Reads a property and expands/converts it's value into a pathname. If the
	 * value string starts with a '$' character then the substring between the
	 * '$' character and the first path-separator is expanded using
	 * 1. the option corresponding to the substring from "PATHS" section or,
	 * 2. a environment variable matching the substring and,
	 * finally, a posix shell like expansion of the pathname is performed.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @param preset default value to process if no valid entry found
	 * @return expanded pathname associated with the given key (caller must free
	 * this string).
	 */
	char* getPathName(const char *section, const char *option,
			const char *preset = nullptr) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Traverses a given section within the configuration.
	 * @param section section name, can be empty string
	 * @param f the callback function (return 0 to continue, 1 to halt)
	 * @param data additional argument for the callback function
	 */
	void map(const char *section,
			int (&f)(const char *option, const char *value, void *arg),
			void *data) const;
	//-----------------------------------------------------------------
	/**
	 * Returns the number of sections.
	 * @return sections/groups count
	 */
	unsigned int sections() const noexcept;
	/**
	 * Returns the number of properties within a section.
	 * @param section section name, can be empty string
	 * @return properties count
	 */
	unsigned int entries(const char *section) const noexcept;
	/**
	 * Checks if a given property exists.
	 * @param section section name, can be empty string
	 * @param option key's name
	 * @return true if the property exists, false otherwise
	 */
	bool exists(const char *section, const char *option) const noexcept;
	/**
	 * Removes a property.
	 * @param section section name, can be empty string
	 * @param option key's name
	 */
	void remove(const char *section, const char *option) noexcept;
	/**
	 * Removes a section.
	 * @param section section name, can be empty string
	 */
	void remove(const char *section) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns a status code describing the configuration data's state.
	 * @return 0 if the object is empty or doesn't contain any uncommitted
	 * changes, 1 if the object contains uncommitted changes, -1 if the last
	 * commit failed.
	 */
	int getStatus() const noexcept;
	/**
	 * Resets the status code to zero(0).
	 */
	void resetStatus() noexcept;
private:
	struct Entry;	//Forward Declaration
	struct Section;	//Forward Declaration
	char* expand(const char *path) const noexcept;
	Section* findSection(const char *section) const noexcept;
	Entry* findEntry(const char *section, const char *key, Section **secP =
			nullptr) const noexcept;
	Section* addSection(const char *name) noexcept;
	Entry* addEntry(const char *name, Section *section) noexcept;
public:
	/** Maximum input string (row) length including the NUL terminator */
	static constexpr unsigned MAX_LINE_LEN = 256;
	/** Maximum section name length including the NUL terminator */
	static constexpr unsigned MAX_SECTION_LEN = 64;
	/** Maximum key length including the NUL terminator */
	static constexpr unsigned MAX_KEY_LEN = 32;
	/** Maximum value length including the NUL terminator */
	static constexpr unsigned MAX_VALUE_LEN = 224;
private:
	// Entries in a section
	struct Entry {
		//Key for this entry
		char key[MAX_KEY_LEN];
		//Current, committed value
		char value[MAX_VALUE_LEN];
	};
	// Section containing entries
	struct Section {
		//Name of the section
		char name[MAX_SECTION_LEN];
		//Number of entries in the section
		unsigned int nEntries;
		//Capacity of the <entries> array
		unsigned int capacity;
		//Entries in the section
		Entry *entries;
	};
	// Configuration Data
	struct {
		//0 if clean, 1 if modified, -1 if an error occurred
		int status;
		//How many sections do we have?
		unsigned int nSections;
		//Capacity of the <sections> array
		unsigned int capacity;
		//Array of "nSections" elements
		Section *sections;
	} data;
};

} /* namespace wanhive */

#endif /* WH_BASE_OPTIONS_H_ */
