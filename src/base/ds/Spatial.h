/*
 * Spatial.h
 *
 * Common spatial data types
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_SPATIAL_H_
#define WH_BASE_DS_SPATIAL_H_

namespace wanhive {
/**
 * Two dimensional planar data type.
 * @tparam T component type
 */
template<typename T> struct Planar {
	/*! X component */
	T x;
	/*! Y component */
	T y;
};

/**
 * Three-dimensional spatial data type.
 * @tparam T component type
 */
template<typename T> struct Spatial {
	/*! X component */
	T x;
	/*! Y component */
	T y;
	/*! Z component */
	T z;
};

/**
 * RGB (additive) color model.
 */
struct Color {
	/*! Red channel */
	unsigned char red;
	/*! Green channel */
	unsigned char green;
	/*! Blue channel */
	unsigned char blue;
};

/**
 * Cyclic function's period.
 */
struct Period {
	/*! Initial expiration */
	unsigned int once;
	/*! Repetition interval */
	unsigned int interval;
};

/**
 * Commonly used metric prefixes.
 */
struct Factor {
	/*! Metric prefix Nano- */
	constexpr static long NANO = 1000000000L;
	/*! Metric prefix Micro- */
	constexpr static long MICRO = 1000000L;
	/*! Metric prefix Milli- */
	constexpr static long MILLI = 1000L;
	/*! Nano- in Micro- */
	constexpr static long NANO_IN_MICRO = (NANO / MICRO);
	/*! Nano- in Milli- */
	constexpr static long NANO_IN_MILLI = (NANO / MILLI);
	/*! Micro- in Milli- */
	constexpr static long MICRO_IN_MILLI = (MICRO / MILLI);
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_SPATIAL_H_ */
