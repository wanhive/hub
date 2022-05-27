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
/**
 * Enumeration of log priorities
 */
enum LogLevel : unsigned char {
	WH_LOGLEVEL_EMERGENCY,/**< System unusable */
	WH_LOGLEVEL_ALERT, /**< Requires immediate intervention */
	WH_LOGLEVEL_CRITICAL, /**< Critical conditions */
	WH_LOGLEVEL_ERROR, /**< Error conditions */
	WH_LOGLEVEL_WARNING, /**< Warning conditions */
	WH_LOGLEVEL_NOTICE, /**< Normal but significant condition */
	WH_LOGLEVEL_INFO, /**< Informational */
	WH_LOGLEVEL_DEBUG /**< Debug-level message */
};

/**
 * Enumeration of output targets
 */
enum LogTarget : unsigned char {
	WH_LOG_STDERR,/**< Use stderr */
	WH_LOG_SYS /**< Use syslog */
};

/**
 * Thread-safe logging utility for sending application-generated logs
 * to syslog or stderr.
 */
class Logger {
public:
	/**
	 * Default constructor: creates a new logger, the default priority is set to
	 * debug-level message and the default output target is set to stderr.
	 */
	Logger() noexcept;
	/**
	 * Destructor
	 */
	~Logger();
	//-----------------------------------------------------------------
	/**
	 * Sets the log priority (level) filter.
	 * @param level the priority value, where 0 is the highest priority and 7
	 * (or greater) is the lowest priority.
	 */
	void setLevel(unsigned int level) noexcept;
	/**
	 * Sets the log priority (level) filter.
	 * @param level the priority code
	 */
	void setLevel(LogLevel level) noexcept;
	/**
	 * Returns a value describing the current priority (level) filter.
	 * @return the priority code
	 */
	LogLevel getLevel() const noexcept;
	/**
	 * Sets the default target for writing the message logs.
	 * @param target the output target, set zero (0) to select stderr and set any
	 * other value to select the syslog facility.
	 */
	void setTarget(unsigned int target) noexcept;
	/**
	 * Sets the default target for writing the message logs.
	 * @param target the target code.
	 */
	void setTarget(LogTarget target) noexcept;
	/**
	 * Returns a value describing the output target.
	 * @return the target code
	 */
	LogTarget getTarget() const noexcept;
	/**
	 * Writes a message log.
	 * @param level message's level/priority
	 * @param format message's format (same as the printf format string).
	 */
	void log(LogLevel level, const char *format, ...) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Returns the default logger that writes to the stderr.
	 * @return reference to the default logger
	 */
	static Logger& getDefault() noexcept;
	/**
	 * Returns a string that describes the priority (level) code.
	 * @param level the priority code
	 * @return the level description string
	 */
	static const char* levelString(LogLevel level) noexcept;
	/**
	 * Returns a string that describes the target for writing the message logs.
	 * @param target the output target's code
	 * @return the output target describing string
	 */
	static const char* targetString(LogTarget target) noexcept;
private:
	volatile LogLevel level;
	volatile LogTarget target;
};

//=================================================================
/*
 * Macros for using the "default" logger.
 * The given syntax is not portable, however, it is supported by all the major
 * compilers (gcc, llvm/clang and VS).
 */

#define WH_DEFAULT_LOGGER Logger::getDefault()
#define WH_LOG_LEVEL_STR(l) Logger::levelString(l)

#define WH_LOG(l, format, ...) WH_DEFAULT_LOGGER.log(l, format "\n", ##__VA_ARGS__)
#define WH_LOGL(l, format, ...) WH_LOG(l, "[%s]: " format, WH_LOG_LEVEL_STR(l), ##__VA_ARGS__)
#define WH_LOGLF(l, format, ...) WH_LOG(l, "[%s] [%s]: " format, WH_LOG_LEVEL_STR(l), WH_FUNCTION, ##__VA_ARGS__)
//-----------------------------------------------------------------
//For general logging
#define WH_LOG_DEBUG(format, ...) WH_LOGLF(WH_LOGLEVEL_DEBUG, format, ##__VA_ARGS__)
#define WH_LOG_INFO(format, ...) WH_LOGL(WH_LOGLEVEL_INFO, format, ##__VA_ARGS__)
#define WH_LOG_NOTICE(format, ...) WH_LOGL(WH_LOGLEVEL_NOTICE, format, ##__VA_ARGS__)
#define WH_LOG_WARNING(format, ...) WH_LOGL(WH_LOGLEVEL_WARNING, format, ##__VA_ARGS__)
#define WH_LOG_ERROR(format, ...) WH_LOGL(WH_LOGLEVEL_ERROR, format, ##__VA_ARGS__)
#define WH_LOG_CRITICAL(format, ...) WH_LOGL(WH_LOGLEVEL_CRITICAL, format, ##__VA_ARGS__)
#define WH_LOG_ALERT(format, ...) WH_LOGL(WH_LOGLEVEL_ALERT, format, ##__VA_ARGS__)
#define WH_LOG_EMERGENCY(format, ...) WH_LOGL(WH_LOGLEVEL_EMERGENCY, format, ##__VA_ARGS__)
//-----------------------------------------------------------------
//For exception logging
#define WH_LOG_EXCEPTION(e) WH_LOG_DEBUG("%s", e.what())
#define WH_LOG_EXCEPTION_U() WH_LOG_DEBUG("An exception occurred")

//=================================================================

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_LOGGER_H_ */
