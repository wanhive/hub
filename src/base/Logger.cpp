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
#include <syslog.h>

namespace {

//Maps the syslog priority codes to the levels
const int priorities[] = { LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR, LOG_WARNING,
LOG_NOTICE, LOG_INFO, LOG_DEBUG };

const char *levelNames[] = { "EMERGENCY", "ALERT", "CRITICAL", "ERROR",
		"WARNING", "NOTICE", "INFO", "DEBUG" };
const char *targetNames[] = { "STDERR", "SYSLOG" };
}  // namespace

namespace wanhive {

Logger::Logger() noexcept :
		level(WH_LOGLEVEL_DEBUG), target(WH_LOG_STDERR) {

}

Logger::~Logger() {

}

void Logger::setLevel(unsigned int level) noexcept {
	switch (level) {
	case WH_LOGLEVEL_EMERGENCY:
		setLevel(WH_LOGLEVEL_EMERGENCY);
		break;
	case WH_LOGLEVEL_ALERT:
		setLevel(WH_LOGLEVEL_ALERT);
		break;
	case WH_LOGLEVEL_CRITICAL:
		setLevel(WH_LOGLEVEL_CRITICAL);
		break;
	case WH_LOGLEVEL_ERROR:
		setLevel(WH_LOGLEVEL_ERROR);
		break;
	case WH_LOGLEVEL_WARNING:
		setLevel(WH_LOGLEVEL_WARNING);
		break;
	case WH_LOGLEVEL_NOTICE:
		setLevel(WH_LOGLEVEL_NOTICE);
		break;
	case WH_LOGLEVEL_INFO:
		setLevel(WH_LOGLEVEL_INFO);
		break;
	case WH_LOGLEVEL_DEBUG:
		setLevel(WH_LOGLEVEL_DEBUG);
		break;
	default:
		setLevel(WH_LOGLEVEL_DEBUG);
		break;
	}
}

void Logger::setLevel(LogLevel level) noexcept {
	this->level = level;
}

LogLevel Logger::getLevel() const noexcept {
	return level;
}

void Logger::setTarget(unsigned int target) noexcept {
	switch (target) {
	case WH_LOG_STDERR:
		setTarget(WH_LOG_STDERR);
		break;
	case WH_LOG_SYS:
		setTarget(WH_LOG_SYS);
		break;
	default:
		setTarget(WH_LOG_STDERR);
		break;
	}
}

void Logger::setTarget(LogTarget target) noexcept {
	this->target = target;
}

LogTarget Logger::getTarget() const noexcept {
	return target;
}

void Logger::log(LogLevel level, const char *fmt, ...) const noexcept {
	if (level <= Logger::level) {
		va_list ap;
		va_start(ap, fmt);
		switch (Logger::target) {
		case WH_LOG_STDERR:
			//POSIX-compliant vfprintf is thread safe
			vfprintf(stderr, fmt, ap);
			break;
		case WH_LOG_SYS:
			vsyslog(priorities[level], fmt, ap);
			break;
		default:
			break;
		}
		va_end(ap);
	} else {
		return;
	}
}

Logger& Logger::getDefault() noexcept {
	static Logger logger; //Thread safe in c++11
	return logger;
}

const char* Logger::levelString(LogLevel level) noexcept {
	return levelNames[level];
}

const char* Logger::targetString(LogTarget target) noexcept {
	return targetNames[target];
}

} /* namespace wanhive */
