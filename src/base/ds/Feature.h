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
	 * Returns the minimum and maximum values.
	 * @tparam T data type
	 * @tparam N array size
	 * @param data data array
	 * @param range stores the [minimum, maximum] values in that order
	 * @return true on success, false on failure (invalid input)
	 */
	template<typename T, size_t N> static bool minmax(const T (&data)[N],
			T (&range)[2]) noexcept {
		return minmax(N, data, range);
	}
	/**
	 * Returns the minimum and maximum values.
	 * @tparam T data type
	 * @param count array size
	 * @param data data array
	 * @param range stores the [minimum, maximum] values in that order
	 * @return true on success, false on failure (invalid input)
	 */
	template<typename T> static bool minmax(size_t count, const T *data,
			T (&range)[2]) noexcept {
		WH_ARITHMETIC_ASSERT(T);
		if (!data || !count) {
			return false;
		}

		auto max = data[0];
		auto min = data[0];

		for (size_t i = 0; i < count; ++i) {
			if (!std::isfinite(data[i])) {
				return false;
			}

			if (max < data[i]) {
				max = data[i];
			}

			if (min >= data[i]) {
				min = data[i];
			}
		}

		range[0] = min;
		range[1] = max;
		return true;
	}
	//-----------------------------------------------------------------
	/**
	 * Processes a data array.
	 * @tparam T data type
	 * @tparam N array size
	 * @param data data array for processing (value-result argument)
	 * @param f callback function (return true to continue, false to halt)
	 * @param arg callback function's additional argument
	 * @return true if the callback function ran it's course, false otherwise
	 */
	template<typename T, size_t N> static bool process(T (&data)[N],
			bool (&f)(T &v, void *e), void *arg = nullptr) {
		return process(N, data, f, arg);
	}
	/**
	 * Processes a data array.
	 * @tparam T data type
	 * @param count array size
	 * @param data data array for processing (value-result argument)
	 * @param f callback function (return true to continue, false to halt)
	 * @param arg callback function's additional argument
	 * @return true if the callback function ran it's course, false otherwise
	 */
	template<typename T> static bool process(size_t count, T *data,
			bool (&f)(T &v, void *e), void *arg = nullptr) {
		if (!count || !data) {
			return false;
		}

		for (size_t i = 0; i < count; ++i) {
			if (!f(data[i], arg)) {
				return false;
			}
		}

		return true;
	}
	//-----------------------------------------------------------------
	/**
	 * Performs min-max normalization.
	 * @tparam T data type
	 * @tparam N array size
	 * @param data data array for normalization (value-result argument)
	 * @param range stores the [minimum, maximum] values in that order
	 * @return true on success, false on failure (invalid input)
	 */
	template<typename T, size_t N> static bool rescale(T (&data)[N],
			T (&range)[2]) noexcept {
		return rescale(N, data, range);
	}
	/**
	 * Performs min-max normalization.
	 * @tparam T data type
	 * @param count array size
	 * @param data data array for normalization (value-result argument)
	 * @param range stores the [minimum, maximum] values in that order
	 * @return true on success, false on failure (invalid input)
	 */
	template<typename T> static bool rescale(size_t count, T *data,
			T (&range)[2]) noexcept {
		WH_FLOATING_POINT_ASSERT(T);
		if (!minmax(count, data, range)) {
			return false;
		}

		auto min = range[0];
		auto max = range[1];

		if (min == max) {
			for (size_t i = 0; i < count; ++i) {
				data[i] = 0;
			}
		} else {
			for (size_t i = 0; i < count; ++i) {
				data[i] = (data[i] - min) / (max - min);
			}
		}

		return true;
	}
};

} /* namespace wanhive */

#endif /* WH_BASE_DS_FEATURE_H_ */
