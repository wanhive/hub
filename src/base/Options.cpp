/*
 * Options.cpp
 *
 * Configuration management
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Options.h"
#include "Storage.h"
#include "Timer.h"
#include "common/Exception.h"
#include "common/Memory.h"
#include "ds/Twiddler.h"
#include <cctype>
#include <climits>
#include <cstring>

namespace {

const char SECTION_REGEX[] = "[ %63[A-Za-z0-9] ]";
const char KEY_VALUE_REGEX[] = "%31[^= ] = %223[^\n]";

}  // namespace

namespace wanhive {

Options::Options() noexcept {
	memset(&data, 0, sizeof(data));
}

Options::Options(const char *filename) {
	memset(&data, 0, sizeof(data));
	if (!load(filename)) {
		clear();
		throw Exception(EX_OPERATION);
	}
}

Options::~Options() {
	for (unsigned int i = 0; i < data.nSections; ++i) {
		memset(data.sections[i].entries, 0, sizeof(Entry));
		free(data.sections[i].entries);
	}
	free(data.sections);
	memset(&data, 0, sizeof(data));
}

void Options::clear() noexcept {
	for (unsigned int i = 0; i < data.nSections; ++i) {
		memset(data.sections[i].entries, 0, sizeof(Entry));
		free(data.sections[i].entries);
	}
	free(data.sections);
	memset(&data, 0, sizeof(data));
}

bool Options::load(const char *filename, size_t *count) noexcept {
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

	auto fp = Storage::open(filename, "r");
	if (fp == nullptr) {
		return false;
	}

	section[0] = '\0';	//Default section name
	//-----------------------------------------------------------------
	while (fgets(buffer, sizeof(buffer), fp)) {
		rows++;
		key[0] = '\0';
		value[0] = '\0';
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
	if (Storage::close(fp)) {
		success = false;
	}

	//Return the number of rows processed so far
	if (count) {
		*count = rows;
	}

	//Restore the dirty flag
	data.status = dirty;
	return success;
}

bool Options::store(const char *filename) noexcept {
	FILE *fp = nullptr;
	auto success = true;

	if ((fp = Storage::open(filename, "w"))) {
		//Print sections and entries
		success = print(fp, strrchr(filename, Storage::PATH_SEPARATOR) + 1);
	} else {
		data.status = -1;
		return false;
	}

	if (Storage::close(fp)) {
		success = false;
	}

	if (success) {
		data.status = 0; //Success, changes have been committed to disk
	} else {
		data.status = -1;
	}
	return success;
}

bool Options::print(FILE *stream, const char *name) noexcept {
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

bool Options::setString(const char *section, const char *option,
		const char *value) noexcept {
	if (section && option && value && option[0] && value[0]) {
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

const char* Options::getString(const char *section, const char *option,
		const char *preset) const noexcept {
	if (section && option && option[0]) {
		auto entry = findEntry(section, option);
		if (entry) {
			return entry->value;
		} else {
			return preset;
		}
	} else {
		return preset;
	}
}

bool Options::setNumber(const char *section, const char *option,
		unsigned long long value) noexcept {
	char s[128];
	memset(s, 0, sizeof(s));
	snprintf(s, 128, "%llu", value);
	return setString(section, option, s);
}

unsigned long long Options::getNumber(const char *section, const char *option,
		unsigned long long preset) const noexcept {
	unsigned long long num;
	auto val = getString(section, option);
	if (val && sscanf(val, "%llu", &num) == 1) {
		return num;
	} else {
		return preset;
	}
}

bool Options::setDouble(const char *section, const char *option,
		double value) noexcept {
	char s[128];
	memset(s, 0, sizeof(s));
	snprintf(s, 128, "%f", value);
	return setString(section, option, s);
}

double Options::getDouble(const char *section, const char *option,
		double preset) const noexcept {
	double num;
	auto val = getString(section, option);
	if (val && sscanf(val, "%lf", &num) == 1) {
		return num;
	} else {
		return preset;
	}
}

bool Options::setBoolean(const char *section, const char *option,
		bool value) noexcept {
	return setString(section, option, WH_BOOLF(value));
}

bool Options::getBoolean(const char *section, const char *option,
		bool preset) const noexcept {
	auto val = getString(section, option);
	if (!val) {
		return preset;
	} else if (strcasecmp("TRUE", val) == 0) {
		return true;
	} else if (strcasecmp("YES", val) == 0) {
		return true;
	} else if (strcasecmp("ON", val) == 0) {
		return true;
	} else {
		return false;
	}
}

char* Options::getPathName(const char *section, const char *option,
		const char *preset) const noexcept {
	return expand(getString(section, option, preset));
}

void Options::map(const char *section,
		int (&f)(const char *option, const char *value, void *arg),
		void *data) const {
	if (section) {
		auto sec = findSection(section);
		if (!sec) {
			return;
		}

		for (unsigned int i = 0; i < sec->nEntries; ++i) {
			if (f(sec->entries[i].key, sec->entries[i].value, data)) {
				break;
			}
		}
	}
}

unsigned int Options::sections() const noexcept {
	return data.nSections;
}

unsigned int Options::entries(const char *section) const noexcept {
	if (section) {
		auto sec = findSection(section);
		return sec ? sec->nEntries : 0;
	} else {
		return 0;
	}
}

bool Options::exists(const char *section, const char *option) const noexcept {
	if (section && option && option[0]) {
		return (findEntry(section, option) != nullptr);
	} else {
		return false;
	}
}

void Options::remove(const char *section, const char *option) noexcept {
	if (section && option && option[0]) {
		Section *sec = nullptr;
		auto entry = findEntry(section, option, &sec);

		if (!sec || !entry) {
			return;
		} else {
			auto lastIndex = sec->nEntries - 1;
			*entry = sec->entries[lastIndex];
			memset(&sec->entries[lastIndex], 0, sizeof(Entry));
			sec->nEntries -= 1;
			data.status = 1;
		}

		//If the array has become too sparse then fix it
		if (sec->capacity > 32 && sec->nEntries < (sec->capacity >> 2)) {
			Memory<Entry>::resize(sec->entries, sec->nEntries);
			sec->capacity = sec->nEntries;
		}
	}
}

void Options::remove(const char *section) noexcept {
	if (section) {
		auto sec = findSection(section);
		if (!sec) {
			return;
		}

		auto entries = sec->entries;
		auto lastIndex = data.nSections - 1;
		*sec = data.sections[lastIndex];
		memset(&data.sections[lastIndex], 0, sizeof(Section));
		free(entries);
		data.nSections -= 1;
		data.status = 1;

		//If the array has become too sparse then fix it
		if (data.capacity > 32 && data.nSections < (data.capacity >> 2)) {
			Memory<Section>::resize(data.sections, data.nSections);
		}
	}
}

int Options::getStatus() const noexcept {
	return data.status;
}

void Options::resetStatus() noexcept {
	data.status = 0;
}

char* Options::expand(const char *path) const noexcept {
	if (!path) {
		return nullptr;
	} else if (path[0] != '$') {
		return Storage::expand(path);
	}
	//-----------------------------------------------------------------
	char orig[PATH_MAX] { };
	strncpy(orig, path, sizeof(orig) - 1);
	//-----------------------------------------------------------------
	/*
	 * Resolve the postfix substring succeeding the first path separator
	 */
	size_t i = 0; //Index of the first path separator or NUL terminator
	while (orig[i] && (orig[i] != Storage::PATH_SEPARATOR)) {
		i++;
	}

	auto postfix = "";	//Substring after the first path separator
	if (orig[i]) {
		orig[i] = '\0';	//Inject a NUL terminator to produce two substrings
		postfix = &orig[i + 1];
	}
	//-----------------------------------------------------------------
	/*
	 * Resolve the leading substring into prefix
	 */
	//Resolve using PATHS section inside configuration
	auto prefix = getString("PATHS", &orig[1]);
	//-----------------------------------------------------------------
	/*
	 * Expand into the full path
	 */
	if (!prefix) {
		//Restore the original string
		orig[i] = postfix[0] ? Storage::PATH_SEPARATOR : '\0';
		//Expand into the full path and return
		return Storage::expand(orig);
	} else {
		//1. Construct a string in the format: prefix/postfix
		auto prefixLen = strlen(prefix);
		auto postfixLen = strlen(postfix);
		if ((prefixLen + postfixLen) >= PATH_MAX) {
			return nullptr;
		}

		char result[PATH_MAX] { };
		memcpy(result, prefix, prefixLen);
		if (postfixLen
				&& (!prefixLen
						|| (prefix[prefixLen - 1] != Storage::PATH_SEPARATOR))) {
			//Inject a path separator if required
			result[prefixLen] = Storage::PATH_SEPARATOR;
			prefixLen += 1;
		}

		//2. Expand into the full path and return
		if ((prefixLen + postfixLen) < PATH_MAX) {
			memcpy(result + prefixLen, postfix, postfixLen);
			return Storage::expand(result);
		} else {
			return nullptr;
		}
	}
}

Options::Section* Options::findSection(const char *section) const noexcept {
	for (unsigned int i = 0; i < data.nSections; ++i) {
		if (strcmp(section, data.sections[i].name) == 0) {
			return &data.sections[i];
		}
	}
	return nullptr;
}

Options::Entry* Options::findEntry(const char *section, const char *key,
		Section **secP) const noexcept {
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

Options::Section* Options::addSection(const char *name) noexcept {
	Section section;
	memset(&section, 0, sizeof(section));
	strncpy(section.name, name, MAX_SECTION_LEN - 1);
	return Memory<Section>::append(data.sections, data.capacity, data.nSections,
			section);
}

Options::Entry* Options::addEntry(const char *name, Section *section) noexcept {
	Entry entry;
	memset(&entry, 0, sizeof(entry));
	strncpy(entry.key, name, MAX_KEY_LEN - 1);
	return Memory<Entry>::append(section->entries, section->capacity,
			section->nEntries, entry);
}

} /* namespace wanhive */
