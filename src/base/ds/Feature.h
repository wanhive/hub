/*
 * Feature.h
 *
 * Feature scaling routines
 *
 *
 * Copyright (C) 2025 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_FEATURE_H_
#define WH_BASE_DS_FEATURE_H_
#include "../common/reflect.h"
#include "Spatial.h"
#include <cmath>

namespace wanhive {
/**
 * Feature scaling routines.
 */
class Feature {
public:
	/**
	 * Performs min-max normalization.
	 * @tparam T data type
	 * @tparam N array size
	 * @param data data array for normalization
	 * @param range the [minimum, maximum] values in that order
	 * @return true on success, false on failure (invalid input)
	 */
	template<typename T, size_t N> static bool rescale(T (&data)[N],
			T (&range)[2]) noexcept {
		return rescale(N, data, range);
	}

	/**
	 * Performs min-max normalization.
	 * @tparam T data type
	 * @param count elements count
	 * @param data data array
	 * @param range the [minimum, maximum] values in that order
	 * @return true on success, false on failure (invalid input)
	 */
	template<typename T> static bool rescale(size_t count, T *data,
			T (&range)[2]) noexcept {
		WH_FLOATING_POINT_ASSERT(T);
		if (!data || !count) {
			return false;
		}

		auto max = data[0];
		auto min = data[0];

		for (size_t i = 0; i < count; ++i) {
			if (std::isnan(data[i])) {
				return false;
			}

			if (max < data[i]) {
				max = data[i];
			}

			if (min >= data[i]) {
				min = data[i];
			}
		}

		if (max == min) {
			for (size_t i = 0; i < count; ++i) {
				data[i] = 0;
			}
		} else {
			for (size_t i = 0; i < count; ++i) {
				data[i] = (data[i] - min) / (max - min);
			}
		}

		range[0] = min;
		range[1] = max;

		return true;
	}
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_FEATURE_H_ */
