/*
 * defines.h
 *
 * For compatibility with the compiler and the target platform
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_DEFINES_H_
#define WH_BASE_COMMON_DEFINES_H_
//-----------------------------------------------------------------
/**
 * Detect Linux
 */
#if defined(__linux__) || defined(__gnu_linux__)
#define WH_LINUX
#endif

//-----------------------------------------------------------------
/**
 * Detect GCC
 */
#ifdef __GNUC__
#define WH_GCC
#define WH_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

//-----------------------------------------------------------------
/**
 * Detect CPU (X86, X86-64 and Arm-32/64 are supported)
 */
#ifdef __i386__
#define WH_ARCH_X86
#endif

#if defined(__i386__) || defined(__x86_64__)
#define WH_ARCH_X86
#endif

#if defined(__x86_64__) && !defined(__ILP32__)
#define WH_ARCH_X86_64
#endif

#if defined(__arm__) || defined(__aarch64__)
#define WH_ARCH_ARM
#endif

#if defined(__aarch64__)  && !defined(__ILP32__)
#define WH_ARCH_ARM_64
#endif
//-----------------------------------------------------------------
/**
 * Macros for error handling and reporting
 */

/*
 * Enable/disable run-time assertions
 */
#ifndef WH_DEBUG
#define WH_DEBUG 0
#endif

/*
 * For debug messages
 */
#if WH_DEBUG
#define WH_FILE __FILE__
#define WH_LINE __LINE__
#else
#define WH_FILE ""
#define WH_LINE 0
#endif

#ifdef WH_GCC
#define WH_FUNCTION __PRETTY_FUNCTION__
#else
#define WH_FUNCTION __func__
#endif
//-----------------------------------------------------------------
/**
 * Macro to string
 * REF: https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
 */
#define WH_STRINGIFY(x) #x
#define WH_TOSTRING(x) WH_STRINGIFY(x)
//-----------------------------------------------------------------
/**
 * Boolean value to string literal
 */
#define WH_BOOLF(expr) ((expr) ? "YES" : "NO")
//-----------------------------------------------------------------
/**
 * For probing a C-style structure
 */
#define WH_FIELD_SIZEOF(type, field) sizeof(((type *)0)->field)
#define WH_FIELD_TYPEOF(type, field) decltype(((type *)0)->field)
//-----------------------------------------------------------------
/**
 * For evaluation of an array's length
 */
#define WH_ARRAYLEN(x) (sizeof(x) / sizeof(x[0]))
//-----------------------------------------------------------------
/**
 * For secure logging
 */
#ifndef WH_LOG_SAFE
#define WH_LOG_SAFE 1
#endif

#if WH_LOG_SAFE
#define WH_SEC_VAL(x) ((decltype(x))(0))
#define WH_SEC_STR(x) "***"
#define WH_MASK_VAL(p, x) ((p) ? WH_SEC_VAL(x) : (x))
#define WH_MASK_STR(p, x) ((p) ? WH_SEC_STR(x) : (x))
#else
#define WH_SEC_VAL(x) (x)
#define WH_SEC_STR(x) (x)
#define WH_MASK_VAL(p, x) (x)
#define WH_MASK_STR(p, x) (x)
#endif
//-----------------------------------------------------------------

#endif /* WH_BASE_COMMON_DEFINES_H_ */
