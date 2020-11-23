/*
 * Logger.cpp
 *
 * Log management
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Logger.h"
#include <cstdarg>
#include <cstdio>

namespace wanhive {

const char *Logger::logLevelString[] = { "EMERGENCY", "ALERT", "CRITICAL",
		"ERROR", "WARNING", "NOTICE", "INFO", "DEBUG" };

Logger::Logger() noexcept :
		level(WH_LOGLEVEL_DEBUG) {

}

Logger::~Logger() {

}

void Logger::setLevel(unsigned int level) noexcept {
	switch (level) {
	case WH_LOGLEVEL_EMERGENCY:
		Logger::level = WH_LOGLEVEL_EMERGENCY;
		break;
	case WH_LOGLEVEL_ALERT:
		Logger::level = WH_LOGLEVEL_ALERT;
		break;
	case WH_LOGLEVEL_CRITICAL:
		Logger::level = WH_LOGLEVEL_CRITICAL;
		break;
	case WH_LOGLEVEL_ERROR:
		Logger::level = WH_LOGLEVEL_ERROR;
		break;
	case WH_LOGLEVEL_WARNING:
		Logger::level = WH_LOGLEVEL_WARNING;
		break;
	case WH_LOGLEVEL_NOTICE:
		Logger::level = WH_LOGLEVEL_NOTICE;
		break;
	case WH_LOGLEVEL_INFO:
		Logger::level = WH_LOGLEVEL_INFO;
		break;
	case WH_LOGLEVEL_DEBUG:
		Logger::level = WH_LOGLEVEL_DEBUG;
		break;
	default:
		Logger::level = WH_LOGLEVEL_DEBUG;
		break;
	}
}

LogLevel Logger::getLevel() const noexcept {
	return level;
}

void Logger::log(LogLevel level, const char *fmt, ...) const noexcept {
	if (level <= Logger::level) {
		va_list ap;
		va_start(ap, fmt);
		//POSIX compliant vfprintf is thread safe (no intermingling)
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	} else {
		return;
	}
}

Logger& Logger::getDefault() noexcept {
	static Logger instance; //Thread safe in c++11
	return instance;
}

const char* Logger::describeLevel(LogLevel level) noexcept {
	return logLevelString[level];
}

} /* namespace wanhive */
