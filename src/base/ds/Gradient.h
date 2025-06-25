/*
 * Gradient.h
 *
 * Heatmap generator
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_GRADIENT_H_
#define WH_BASE_DS_GRADIENT_H_
#include "Spatial.h"

namespace wanhive {
/**
 * Heatmap generator
 * @ref https://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients
 */
class Gradient {
public:
	/**
	 * Constructor: configures the color map.
	 * @param colored true for 7-color gradient, false for gray-scale
	 */
	Gradient(bool colored = true) noexcept;
	/**
	 * Destructor
	 */
	~Gradient();
	/**
	 * Maps a given value to it's corresponding color code.
	 * @param data normalized data in [0, 1] range
	 * @param color output color code
	 */
	void map(double data, Color &color) const noexcept;
private:
	const bool _colored;
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_GRADIENT_H_ */
