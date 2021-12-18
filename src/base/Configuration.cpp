/*
 * Configuration.cpp
 *
 * Configuration management
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Configuration.h"
#include "Storage.h"
#include "Timer.h"
#include "common/Exception.h"
#include "ds/Array.h"
#include "ds/Twiddler.h"
#include <cctype>
#include <cstring>

namespace wanhive {

const char *Configuration::SECTION_REGEX = "[ %63[A-Za-z0-9] ]";
const char *Configuration::KEY_VALUE_REGEX = "%31[^= ] = %223[^\n]";

Configuration::Configuration() noexcept {
	memset(&data, 0, sizeof(data));
}

Configuration::Configuration(const char *filename) {
	memset(&data, 0, sizeof(data));
	if (!load(filename)) {
		clear();
		throw Exception(EX_INVALIDOPERATION);
	}
}

Configuration::~Configuration() {
	clear();
}

void Configuration::clear() noexcept {
	for (unsigned int i = 0; i < data.nSections; ++i) {
		memset(data.sections[i].entries, 0, sizeof(Entry));
		WH_free(data.sections[i].entries);
	}
	WH_free(data.sections);
	memset(&data, 0, sizeof(data));
}

bool Configuration::load(const char *filename, size_t *lastRow) noexcept {
	char buffer[MAX_LINE_LEN * 2];
	char section[MAX_SECTION_LEN];
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];

	auto success = true;
	auto dirty = data.status; //Back-up
	size_t rows = 0; //Processed rows count

	if (Storage::testFile(filename) != 1) {
		return false;
	}

	auto fp = Storage::openStream(filename, "r", false);
	if (fp == nullptr) {
		return false;
	}

	section[0] = '\0';	//Default section name
	//-----------------------------------------------------------------
	while (fgets(buffer, sizeof(buffer), fp)) {
		rows++;

		unsigned int eol = 0;
		auto line = Twiddler::trim(buffer, eol);
		if (line[0] == '#' || line[0] == '%' || line[0] == '!' || line[0] == ';'
				|| line[0] == '\0') {
			//Skip comment/empty line
			continue;
		} else if (line[0] == '[') {
			section[0] = '\0';
			if (line[eol - 1]
					!= ']'|| sscanf(line, SECTION_REGEX, section) == EOF) {
				success = false;
				break;
			}
		} else if (sscanf(line, KEY_VALUE_REGEX, key, value) == 2) {
			setString(section, key, value);
		} else {
			success = false;
			break;
		}
	}
	//-----------------------------------------------------------------
	//Close the file
	if (Storage::closeStream(fp)) {
		success = false;
	}

	//Return the number of rows processed so far
	if (lastRow) {
		*lastRow = rows;
	}

	//Restore the dirty flag
	data.status = dirty;
	return success;
}

bool Configuration::store(const char *filename) noexcept {
	FILE *fp = nullptr;
	auto success = true;

	if ((fp = Storage::openStream(filename, "w", true))) {
		//Print sections and entries
		success = print(fp, strrchr(filename, Storage::DIR_SEPARATOR) + 1);
	} else {
		data.status = -1;
		return false;
	}

	if (Storage::closeStream(fp)) {
		success = false;
	}

	if (success) {
		data.status = 0; //Success, changes have been committed to disk
	} else {
		data.status = -1;
	}
	return success;
}

bool Configuration::print(FILE *stream, const char *name) noexcept {
	if (!stream) {
		return false;
	}

	if (name) {
		char tf[64];
		memset(tf, 0, sizeof(tf));
		Timer::print(tf, sizeof(tf));
		//Print the header
		fprintf(stream, "#Configuration %s auto-generated on %s\n", name, tf);
	}

	//Print the sections and entries
	for (unsigned int i = 0; i < data.nSections; ++i) {
		auto sec = &data.sections[i];
		if (fprintf(stream, "\n#Section: %s\n[%s]\n", sec->name, sec->name)
				< 0) {
			return false;
		}

		//Print the entries
		for (unsigned int j = 0; j < sec->nEntries; ++j) {
			auto e = &sec->entries[j];
			if (fprintf(stream, "%s = %s\n", e->key, e->value) < 0) {
				return false;
			}
		}
	}
	return true;
}

bool Configuration::setString(const char *section, const char *option,
		const char *value) noexcept {
	if (section && option && value) {
		// Check whether the entry already exists
		Section *s = nullptr;
		auto e = findEntry(section, option, &s);
		if (e == nullptr) {
			if (s == nullptr) {
				s = addSection(section);
			}
			e = addEntry(option, s);
		}
		/* Update the Entry */
		if (strcmp(e->value, value)) {
			data.status = 1;
			memset(e->value, 0, MAX_VALUE_LEN);
			strncpy(e->value, value, MAX_VALUE_LEN - 1);
		}
		return true;
	} else {
		return false;
	}

}

const char* Configuration::getString(const char *section, const char *option,
		const char *defaultValue) const noexcept {
	if (section && option) {
		auto entry = findEntry(section, option);
		if (entry) {
			return entry->value;
		} else {
			return defaultValue;
		}
	} else {
		return defaultValue;
	}
}

bool Configuration::setNumber(const char *section, const char *option,
		unsigned long long value) noexcept {
	char s[128];
	memset(s, 0, sizeof(s));
	snprintf(s, 128, "%llu", value);
	return setString(section, option, s);
}

unsigned long long Configuration::getNumber(const char *section,
		const char *option, unsigned long long defaultValue) const noexcept {
	unsigned long long num;
	auto val = getString(section, option);
	if (val && sscanf(val, "%llu", &num) == 1) {
		return num;
	} else {
		return defaultValue;
	}
}

bool Configuration::setDouble(const char *section, const char *option,
		double value) noexcept {
	char s[128];
	memset(s, 0, sizeof(s));
	snprintf(s, 128, "%f", value);
	return setString(section, option, s);
}

double Configuration::getDouble(const char *section, const char *option,
		double defaultValue) const noexcept {
	double num;
	auto val = getString(section, option);
	if (val && sscanf(val, "%lf", &num) == 1) {
		return num;
	} else {
		return defaultValue;
	}
}

bool Configuration::setBoolean(const char *section, const char *option,
		bool value) noexcept {
	return setString(section, option, value ? "YES" : "NO");
}

bool Configuration::getBoolean(const char *section, const char *option,
		bool defaultValue) const noexcept {
	auto val = getString(section, option);
	if (!val) {
		return defaultValue;
	} else if (strcasecmp(val, "TRUE") == 0) {
		return true;
	} else if (strcasecmp(val, "YES") == 0) {
		return true;
	} else if (strcasecmp(val, "ON") == 0) {
		return true;
	} else {
		return false;
	}
}

char* Configuration::getPathName(const char *section, const char *option,
		const char *defaultValue) const noexcept {
	return expandPath(getString(section, option, defaultValue));
}

bool Configuration::haveValue(const char *section, const char *option) noexcept {
	if (section && option) {
		return findEntry(section, option);
	} else {
		return false;
	}
}

int Configuration::getStatus() const noexcept {
	return data.status;
}

void Configuration::resetStatus() noexcept {
	data.status = 0;
}

char* Configuration::expandPath(const char *pathname) const noexcept {
	if (!pathname) {
		return nullptr;
	} else if (pathname[0] != '$') {
		return Storage::expandPathName(pathname);
	}
	//-----------------------------------------------------------------
	auto tmp = WH_strdup(pathname);	//The working copy
	/*
	 * Resolve the postfix which is the substring
	 * succeeding the first path separator
	 */
	unsigned int i = 0;	//Index of the first path separator or NUL terminator
	const char *postfix = nullptr;	//Substring after the first path separator

	while (tmp[i] && (tmp[i] != Storage::DIR_SEPARATOR)) {
		i++;
	}

	if (!tmp[i]) {
		postfix = "";
	} else {
		tmp[i] = '\0';		//Inject NUL terminator to produce two substrings
		postfix = &tmp[i + 1];
	}
	//-----------------------------------------------------------------
	/*
	 * Resolve the initial substring into prefix
	 */
	//Resolve using PATHS section inside configuration
	auto prefix = getString("PATHS", &tmp[1]);
	//-----------------------------------------------------------------
	/*
	 * Expand into the full path
	 */
	if (!prefix) {
		//Restore the original string
		tmp[i] = postfix[0] ? Storage::DIR_SEPARATOR : '\0';
		//Expand into the full path and return
		auto result = Storage::expandPathName(tmp);
		WH_free(tmp);
		return result;
	} else {
		//1. Construct the string in format: prefix/postfix
		auto prefixLen = strlen(prefix);
		auto postfixLen = strlen(postfix);
		auto result = (char*) WH_malloc(prefixLen + postfixLen + 2);
		strcpy(result, prefix);
		if (postfixLen
				&& (!prefixLen
						|| (prefix[prefixLen - 1] != Storage::DIR_SEPARATOR))) {
			//Inject a path separator if required
			strcat(result, Storage::DIR_SEPARATOR_STR);
		}
		strcat(result, postfix);
		WH_free(tmp);

		//2. Expand into the full path and return
		tmp = result;
		result = Storage::expandPathName(result);
		WH_free(tmp);
		return result;
	}
}

Configuration::Section* Configuration::findSection(
		const char *section) const noexcept {
	for (unsigned int i = 0; i < data.nSections; ++i) {
		if (strcmp(section, data.sections[i].name) == 0) {
			return &data.sections[i];
		}
	}
	return nullptr;
}

Configuration::Entry* Configuration::findEntry(const char *section,
		const char *key, Section **secP) const noexcept {
	auto sec = findSection(section);
	if (secP) {
		*secP = sec;
	}
	if (sec == nullptr) {
		return nullptr;
	}
	for (unsigned int i = 0; i < sec->nEntries; ++i) {
		if (0 == strcmp(key, sec->entries[i].key)) {
			return &sec->entries[i];
		}
	}
	return nullptr;
}

Configuration::Section* Configuration::addSection(const char *name) noexcept {
	Section section;
	memset(&section, 0, sizeof(section));
	strncpy(section.name, name, MAX_SECTION_LEN - 1);
	return Array<Section>::insert(data.sections, data.capacity, data.nSections,
			section);
}

Configuration::Entry* Configuration::addEntry(const char *name,
		Section *section) noexcept {
	Entry entry;
	memset(&entry, 0, sizeof(entry));
	strncpy(entry.key, name, MAX_KEY_LEN - 1);
	return Array<Entry>::insert(section->entries, section->capacity,
			section->nEntries, entry);
}

} /* namespace wanhive */
