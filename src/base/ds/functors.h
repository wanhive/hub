/*
 * functors.h
 *
 * Functors for common data structure operations
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_DS_FUNCTORS_H_
#define WH_BASE_DS_FUNCTORS_H_
#include <cstring>

namespace wanhive {
//-----------------------------------------------------------------
/**
 * Default hash functors
 */
struct wh_hash_fn {
	unsigned int operator()(int key) const noexcept {
		return static_cast<unsigned int>(key); //Just return the key
	}

	unsigned int operator()(unsigned int key) const noexcept {
		return key; //Just return the key
	}

	unsigned int operator()(unsigned long key) const noexcept {
		if constexpr (sizeof(unsigned long) > 4) {
			return operator()((unsigned long long) key);
		} else {
			return key; //Just return the key
		}
	}

	unsigned int operator()(unsigned long long key) const noexcept {
		return static_cast<unsigned int>(key >> 33 ^ key ^ key << 11);
	}

	unsigned int operator()(void *p) const noexcept {
		return operator()((unsigned long long) p);
	}

	unsigned int operator()(const char *s) const noexcept {
		//AC X31 HASH: <s> should not be nullptr
		unsigned int h = *s;
		if (h) {
			for (++s; *s; ++s) {
				h = (h << 5) - h + *s;
			}
		}
		return h;
	}
};
//-----------------------------------------------------------------
/**
 * Default equality test functors
 */
struct wh_eq_fn {
	bool operator()(int a, int b) const noexcept {
		return a == b;
	}

	bool operator()(unsigned int a, unsigned int b) const noexcept {
		return a == b;
	}

	bool operator()(unsigned long a, unsigned long b) const noexcept {
		return a == b;
	}

	bool operator()(unsigned long long a, unsigned long long b) const noexcept {
		return a == b;
	}

	bool operator()(void *a, void *b) const noexcept {
		return a == b;
	}

	bool operator()(const char *a, const char *b) const noexcept {
		return strcmp(a, b) == 0;
	}
};
//-----------------------------------------------------------------
/**
 * Default less-than order functors
 */
struct wh_lt_fn {
	bool operator()(int a, int b) const noexcept {
		return a < b;
	}

	bool operator()(unsigned int a, unsigned int b) const noexcept {
		return a < b;
	}

	bool operator()(unsigned long a, unsigned long b) const noexcept {
		return a < b;
	}

	bool operator()(unsigned long long a, unsigned long long b) const noexcept {
		return a < b;
	}

	bool operator()(void *a, void *b) const noexcept {
		return a < b;
	}

	bool operator()(const char *a, const char *b) const noexcept {
		return strcmp(a, b) < 0;
	}
};
//-----------------------------------------------------------------
/**
 * Default greater-than order functors
 */
struct wh_gt_fn {
	bool operator()(int a, int b) const noexcept {
		return a > b;
	}

	bool operator()(unsigned int a, unsigned int b) const noexcept {
		return a < b;
	}

	bool operator()(unsigned long a, unsigned long b) const noexcept {
		return a > b;
	}

	bool operator()(unsigned long long a, unsigned long long b) const noexcept {
		return a > b;
	}

	bool operator()(void *a, void *b) const noexcept {
		return a > b;
	}

	bool operator()(const char *a, const char *b) const noexcept {
		return strcmp(a, b) > 0;
	}
};

}  // namespace wanhive

#endif /* WH_BASE_DS_FUNCTORS_H_ */
