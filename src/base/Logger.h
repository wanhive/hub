/*
 * Logger.h
 *
 * Log management
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_LOGGER_H_
#define WH_BASE_LOGGER_H_
#include "common/defines.h"
#include <syslog.h>

namespace wanhive {
/**
 * Application logger
 * Log messages are written to the stderr
 * Thread safe
 */

enum LogLevel : unsigned char {
	WH_LOGLEVEL_EMERGENCY = LOG_EMERG,
	WH_LOGLEVEL_ALERT = LOG_ALERT,
	WH_LOGLEVEL_CRITICAL = LOG_CRIT,
	WH_LOGLEVEL_ERROR = LOG_ERR,
	WH_LOGLEVEL_WARNING = LOG_WARNING,
	WH_LOGLEVEL_NOTICE = LOG_NOTICE,
	WH_LOGLEVEL_INFO = LOG_INFO,
	WH_LOGLEVEL_DEBUG = LOG_DEBUG
};

class Logger {
public:
	Logger(bool sysLog = false) noexcept;
	~Logger();
	//-----------------------------------------------------------------
	//Sets the level filter
	void setLevel(unsigned int level) noexcept;
	void setLevel(LogLevel level) noexcept;
	//Returns the current level filter
	LogLevel getLevel() const noexcept;
	//Write a log message to the stderr
	void log(LogLevel level, const char *fmt, ...) const noexcept;
	//-----------------------------------------------------------------
	//Returns the default logger, thread safe
	static Logger& getDefault() noexcept;
	//Returns a string describing the <level>
	static const char* describeLevel(LogLevel level) noexcept;
private:
	const bool sysLog;
	volatile LogLevel level;
	static const char *logLevelStrings[];
};

//=================================================================
/**
 * Useful macros for using the "default" logger.
 * The given syntax is not portable, however, it is supported
 * by all the major compilers (gcc, llvm/clang and VS).
 */
#define WH_DEF_LOGGER Logger::getDefault()
#define WH_LOG(l, format, ...) WH_DEF_LOGGER.log(l, format "\n", ##__VA_ARGS__)
#define WH_LOGL(l, format, ...) WH_DEF_LOGGER.log(l, "[%s]: " format "\n", Logger::describeLevel(l), ##__VA_ARGS__)
#define WH_LOGLF(l, format, ...) WH_DEF_LOGGER.log(l, "[%s] [%s]: " format "\n", Logger::describeLevel(l), WH_FUNCTION, ##__VA_ARGS__)

//General logging
#define WH_LOG_DEBUG(format, ...) WH_LOGLF(WH_LOGLEVEL_DEBUG, format, ##__VA_ARGS__)
#define WH_LOG_INFO(format, ...) WH_LOGL(WH_LOGLEVEL_INFO, format, ##__VA_ARGS__)
#define WH_LOG_NOTICE(format, ...) WH_LOGL(WH_LOGLEVEL_NOTICE, format, ##__VA_ARGS__)
#define WH_LOG_WARNING(format, ...) WH_LOGL(WH_LOGLEVEL_WARNING, format, ##__VA_ARGS__)
#define WH_LOG_ERROR(format, ...) WH_LOGL(WH_LOGLEVEL_ERROR, format, ##__VA_ARGS__)
#define WH_LOG_CRITICAL(format, ...) WH_LOGL(WH_LOGLEVEL_CRITICAL, format, ##__VA_ARGS__)
#define WH_LOG_ALERT(format, ...) WH_LOGL(WH_LOGLEVEL_ALERT, format, ##__VA_ARGS__)
#define WH_LOG_EMERGENCY(format, ...) WH_LOGL(WH_LOGLEVEL_EMERGENCY, format, ##__VA_ARGS__)

//Exception logging
#define WH_LOG_EXCEPTION(e) WH_LOG_DEBUG("%s", e.what())
#define WH_LOG_EXCEPTION_U() WH_LOG_DEBUG("An exception occurred")
//=================================================================

} /* namespace wanhive */

#endif /* WH_BASE_LOGGER_H_ */
