/*
 * Configuration.h
 *
 * Configuration management
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_CONFIGURATION_H_
#define WH_BASE_CONFIGURATION_H_
#include <cstdio>

namespace wanhive {
/**
 * Configuration manager which accepts files matching
 * the INI file format with several restrictions.
 */
class Configuration {
public:
	Configuration() noexcept;
	Configuration(const char *filename);
	~Configuration();
	//Resets the configuration
	void clear() noexcept;
	/*
	 * Loads configuration from given file, returns true on success, false on
	 * error. If <lastRow> is not nullptr then it contains the number of rows
	 * processed.
	 */
	bool load(const char *filename, size_t *lastRow = nullptr) noexcept;
	//Dumps the configuration into the given file
	bool store(const char *filename) noexcept;
	//Prints the configuration to the given stream
	bool print(FILE *stream, const char *name = nullptr) noexcept;
	//=================================================================
	/**
	 * Getters and Setters, <defaultValue> is returned if the desired
	 * entry doesn't exist
	 */
	bool setString(const char *section, const char *option,
			const char *value) noexcept;
	const char* getString(const char *section, const char *option,
			const char *defaultValue = nullptr) const noexcept;

	bool setNumber(const char *section, const char *option,
			unsigned long long value) noexcept;
	unsigned long long getNumber(const char *section, const char *option,
			unsigned long long defaultValue = 0) const noexcept;

	bool setDouble(const char *section, const char *option,
			double value) noexcept;
	double getDouble(const char *section, const char *option,
			double defaultValue = 0) const noexcept;

	bool setBoolean(const char *section, const char *option,
			bool value) noexcept;
	bool getBoolean(const char *section, const char *option, bool defaultValue =
			false) const noexcept;
	/*
	 * Expands a path name using the following mechanism and returns it:
	 * If the option string starts with a '$' character then the substring
	 * between the '$' character and the first path separator is expanded:
	 * 1. Using the option corresponding to the substring from "PATHS" section or
	 * 2. Using a environment variable matching the substring
	 * Finally, a posix shell like expansion of the pathname is performed.
	 * NOTE: Returned buffer must be freed using WH_free
	 */
	char* getPathName(const char *section, const char *option,
			const char *defaultValue = nullptr) const noexcept;
	//=================================================================
	//Returns true if we have a value for the given <section> and  <option>
	bool haveValue(const char *section, const char *option) noexcept;
	/*
	 * Returns 0: object is empty or doesn't contain any uncommitted changes.
	 * Returns 1: object contains uncommitted changes
	 * Returns -1: the last commit failed
	 */
	int getStatus() const noexcept;
	//Resets the status to 0
	void resetStatus() noexcept;
private:
	struct Entry;	//Forward Declaration
	struct Section;	//Forward Declaration
	char* expandPath(const char *pathname) const noexcept;
	Section* findSection(const char *section) const noexcept;
	Entry* findEntry(const char *section, const char *key, Section **secP =
			nullptr) const noexcept;
	Section* addSection(const char *name) noexcept;
	Entry* addEntry(const char *name, Section *section) noexcept;
public:
	//ASCII-string lengths including the NUL terminator
	static constexpr unsigned MAX_LINE_LEN = 256;
	static constexpr unsigned MAX_SECTION_LEN = 64;
	static constexpr unsigned MAX_KEY_LEN = 32;
	static constexpr unsigned MAX_VALUE_LEN = 224;
private:
	static const char *SECTION_REGEX;
	static const char *KEY_VALUE_REGEX;
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
#endif /* WH_BASE_CONFIGURATION_H_ */
