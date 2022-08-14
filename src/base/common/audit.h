/*
 * audit.h
 *
 * Run time condition checking
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_AUDIT_H_
#define WH_BASE_COMMON_AUDIT_H_
#include "defines.h"
#include <cstdlib> //For abort(), malloc etc

#if WH_DEBUG
#include <cstdio> //For fprintf()
#endif

//=================================================================
/**
 * For run-time assertions and errors
 */
#if WH_DEBUG
#define WH_FATAL_ASSERT_FLF(expr, msg, file, line, function) \
		do { if (!(expr)) { fprintf(stderr, "%s: assertion failed at %s:%d in %s.\n", msg, file, line, function); abort(); } } while(0)
#define WH_NONFATAL_ASSERT_FLF(expr, msg, file, line, function) \
		do { if (!(expr)) { fprintf(stderr, "%s: assertion failed at %s:%d in %s.\n", msg, file, line, function); } } while(0)
#define WH_FATAL_ERROR_FLF(msg, file, line, function) \
	do { fprintf(stderr, "%s: Error at %s:%d in %s.\n", msg, file, line, function); abort(); } while(0)
#define WH_NONFATAL_ERROR_FLF(msg, file, line, function) \
	do { fprintf(stderr, "%s: Error at %s:%d in %s.\n", msg, file, line, function); } while(0)
#else
#define WH_FATAL_ASSERT_FLF(expr, msg, file, line, function)
#define WH_NONFATAL_ASSERT_FLF(expr, msg, file, line, function)
#define WH_FATAL_ERROR_FLF(msg, file, line, function) abort()
#define WH_NONFATAL_ERROR_FLF(msg, file, line, function)
#endif //WH_DEBUG

#define WH_FATAL_ASSERT(expr, msg) WH_FATAL_ASSERT_FLF(expr, msg, WH_FILE, WH_LINE, WH_FUNCTION)
#define WH_NONFATAL_ASSERT(expr, msg) WH_NONFATAL_ASSERT_FLF(expr, msg, WH_FILE, WH_LINE, WH_FUNCTION)
#define WH_FATAL_ERROR(msg) WH_FATAL_ERROR_FLF(msg, WH_FILE, WH_LINE, WH_FUNCTION)
#define WH_NONFATAL_ERROR(msg) WH_NONFATAL_ERROR_FLF(msg, WH_FILE, WH_LINE, WH_FUNCTION)

#endif /* WH_BASE_COMMON_AUDIT_H_ */
