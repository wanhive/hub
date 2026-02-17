/*
 * Gradient.cpp
 *
 * Heatmap generator
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Gradient.h"
#include <cmath>

namespace {

wanhive::Color grayscale(double data) noexcept {
	if (std::isnan(data) || data <= 0) {
		return {0, 0, 0};
	} else if (data >= 1) {
		return {255, 255, 255};
	} else {
		unsigned char value = static_cast<unsigned char>(data * 255);
		return {value, value, value};
	}
}

wanhive::Color colored(double data) noexcept {
	static constexpr unsigned NUM_COLORS = 7;
	static constexpr double colors[NUM_COLORS][3] = { { 0, 0, 0 }, { 0, 0, 1 },
			{ 0, 1, 1 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, { 1, 1, 1 } };

	unsigned idx1 { 0 };
	unsigned idx2 { 0 };
	double fractBetween { 0 };

	if (std::isnan(data) || data <= 0) {
		idx1 = idx2 = 0;
	} else if (data >= 1) {
		idx1 = idx2 = NUM_COLORS - 1;
	} else {
		data = data * (NUM_COLORS - 1);
		idx1 = data;
		idx2 = idx1 + 1;
		fractBetween = data - idx1;
	}

	auto red = (colors[idx2][0] - colors[idx1][0]) * fractBetween
			+ colors[idx1][0];
	auto green = (colors[idx2][1] - colors[idx1][1]) * fractBetween
			+ colors[idx1][1];
	auto blue = (colors[idx2][2] - colors[idx1][2]) * fractBetween
			+ colors[idx1][2];

	wanhive::Color color;
	color.red = static_cast<unsigned char>(red * 255);
	color.green = static_cast<unsigned char>(green * 255);
	color.blue = static_cast<unsigned char>(blue * 255);
	return color;
}

}  // namespace

namespace wanhive {

Gradient::Gradient(bool colored) noexcept :
		_colored { colored } {

}

Gradient::~Gradient() {

}

Color Gradient::map(double data) const noexcept {
	if (_colored) {
		return colored(data);
	} else {
		return grayscale(data);
	}
}

} /* namespace wanhive */
