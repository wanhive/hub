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

} /* namespace wanhive */

#endif /* WH_BASE_DS_SPATIAL_H_ */
