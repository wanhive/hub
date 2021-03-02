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
//=================================================================
/**
 * Detect Linux
 */
#if defined(__linux__) || defined(__gnu_linux__)
#define WH_LINUX
#endif

//=================================================================
/**
 * Detect GCC
 */
#ifdef __GNUC__
#define WH_GCC
#define WH_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

//=================================================================
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
//=================================================================
/**
 * Macros for error handling and reporting
 */

/*
 * For compile time assertions
 */
#define WH_STATIC_ASSERT(expr, msg) static_assert(expr, msg)

/*
 * Enable/disable run-time assertions
 */
#ifndef WH_DEBUG
#define WH_DEBUG 0
#endif

/*
 * For debug messages
 */
#define WH_FILE __FILE__
#define WH_LINE __LINE__
#ifdef WH_GCC
#define WH_FUNCTION __PRETTY_FUNCTION__
#else
#define WH_FUNCTION __func__
#endif
//=================================================================
/**
 * Macro to string
 * REF: https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
 */
#define WH_STRINGIFY(x) #x
#define WH_TOSTRING(x) WH_STRINGIFY(x)

#endif /* WH_BASE_COMMON_DEFINES_H_ */
