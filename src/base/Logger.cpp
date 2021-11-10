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

const char *Logger::logLevelStrings[] = { "EMERGENCY", "ALERT", "CRITICAL",
		"ERROR", "WARNING", "NOTICE", "INFO", "DEBUG" };

Logger::Logger(bool sysLog) noexcept :
		sysLog(sysLog), level(WH_LOGLEVEL_DEBUG) {

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

void Logger::setLevel(LogLevel level) noexcept {
	this->level = level;
}

LogLevel Logger::getLevel() const noexcept {
	return level;
}

void Logger::log(LogLevel level, const char *fmt, ...) const noexcept {
	if (level <= Logger::level) {
		va_list ap;
		va_start(ap, fmt);
		if (sysLog) {
			vsyslog(level, fmt, ap);
		} else {
			//POSIX-compliant vfprintf is thread safe
			vfprintf(stderr, fmt, ap);
		}
		va_end(ap);
	} else {
		return;
	}
}

Logger& Logger::getDefault() noexcept {
	static Logger logger(false); //Thread safe in c++11
	return logger;
}

const char* Logger::describeLevel(LogLevel level) noexcept {
	return logLevelStrings[level];
}

} /* namespace wanhive */
