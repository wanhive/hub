/*
 * Logger.h
 *
 * Logging utility
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_LOGGER_H_
#define WH_BASE_COMMON_LOGGER_H_
#include "defines.h"

namespace wanhive {

enum LogLevel : unsigned char {
	WH_LOGLEVEL_EMERGENCY,
	WH_LOGLEVEL_ALERT,
	WH_LOGLEVEL_CRITICAL,
	WH_LOGLEVEL_ERROR,
	WH_LOGLEVEL_WARNING,
	WH_LOGLEVEL_NOTICE,
	WH_LOGLEVEL_INFO,
	WH_LOGLEVEL_DEBUG
};

enum LogTarget : unsigned char {
	WH_LOG_STDERR, WH_LOG_SYS
};

/**
 * Thread-safe logging utility for sending application-generated logs
 * to syslog or stderr.
 */
class Logger {
public:
	Logger() noexcept;
	~Logger();
	//-----------------------------------------------------------------
	//Sets the level filter
	void setLevel(unsigned int level) noexcept;
	void setLevel(LogLevel level) noexcept;
	//Returns the current level filter
	LogLevel getLevel() const noexcept;
	//Sets the target filter
	void setTarget(unsigned int target) noexcept;
	void setTarget(LogTarget target) noexcept;
	//Returns the current target filter
	LogTarget getTarget() const noexcept;
	//Write a log message to the stderr
	void log(LogLevel level, const char *format, ...) const noexcept;
	//-----------------------------------------------------------------
	//Returns the default logger (writes to stderr), thread safe
	static Logger& getDefault() noexcept;
	//Returns a string describing the <level>
	static const char* levelString(LogLevel level) noexcept;
	//Returns a string describing the <target>
	static const char* targetString(LogTarget target) noexcept;
private:
	volatile LogLevel level;
	volatile LogTarget target;
};

//=================================================================
/**
 * Useful macros for using the "default" logger.
 * The given syntax is not portable, however, it is supported
 * by all the major compilers (gcc, llvm/clang and VS).
 */
#define WH_DEF_LOGGER Logger::getDefault()
#define WH_LOG_LEVEL_STR(l) Logger::levelString(l)

#define WH_LOG(l, format, ...) WH_DEF_LOGGER.log(l, format "\n", ##__VA_ARGS__)
#define WH_LOGL(l, format, ...) WH_DEF_LOGGER.log(l, "[%s]: " format "\n", WH_LOG_LEVEL_STR(l), ##__VA_ARGS__)
#define WH_LOGLF(l, format, ...) WH_DEF_LOGGER.log(l, "[%s] [%s]: " format "\n", WH_LOG_LEVEL_STR(l), WH_FUNCTION, ##__VA_ARGS__)

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

#endif /* WH_BASE_COMMON_LOGGER_H_ */
