/*
 * Atomic.h
 *
 * Atomic operations with integral and pointer types
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_COMMON_ATOMIC_H_
#define WH_BASE_COMMON_ATOMIC_H_
#include "pod.h"
#ifndef WH_GCC
#error "This package must be compiled by GCC."
#endif

#if !defined(WH_GCC_VERSION) || !(WH_GCC_VERSION >= 40700)
#error "Unsupported GCC version."
#endif

namespace wanhive {
enum MemoryOrder {
	MO_RELAXED = __ATOMIC_RELAXED,
	MO_CONSUME = __ATOMIC_CONSUME,
	MO_ACQUIRE = __ATOMIC_ACQUIRE,
	MO_RELEASE = __ATOMIC_RELEASE,
	MO_ACQ_REL = __ATOMIC_ACQ_REL,
	MO_SEQ_CST = __ATOMIC_SEQ_CST
};
/**
 * Following operations can be used with any integral scalar or pointer type
 * that is 1, 2, 4, or 8 bytes in length. If ‘__int128’ (see __int128) is
 * supported by the architecture then 16-byte integral types are also allowed.
 * REF: https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 */
template<typename X = unsigned int> class Atomic {
public:
	/*
	 * Atomic load operation, returns the contents of *P.
	 * Valid memory orders: MM_RELAXED, MM_SEQ_CST, MM_ACQUIRE, and MM_CONSUME
	 */
	static X load(const X *P, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_load_n(P, m);
	}

	/*
	 * Atomic store operation. It writes value into *P.
	 * Valid memory orders: MM_RELAXED, MM_SEQ_CST, and MM_RELEASE
	 */
	static void store(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		__atomic_store_n(P, value, m);
	}

	/*
	 * Atomic exchange operation. It writes value into *P, and returns the previous
	 * contents of *P. Valid memory orders: MM_RELAXED, MM_SEQ_CST, MM_ACQUIRE,
	 * MM_RELEASE, and MM_ACQ_REL
	 */
	static X exchange(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_exchange_n(P, value, m);
	}

	/*
	 * Atomic compare and exchange operation. This compares the contents of *P
	 * with the contents of *expected. If equal, the operation is a read-modify-write
	 * operation that writes desired into *P. If they are not equal, the operation is
	 * a read and the current contents of *P are written into *expected. weak is true
	 * for weak compare_exchange, which may fail spuriously and false for the strong
	 * variation, which never fails spuriously. Many targets only offer the strong
	 * variation and ignore the parameter. When in doubt, use the strong variation.
	 * If desired is written into *P then true is returned and memory is affected
	 * according to the memory order specified by forSuccess. There are no restrictions
	 * on what memory order can be used here. Otherwise, false is returned and
	 * memory is affected according to forFailure. This memory order cannot be
	 * MM_RELEASE nor MM_ACQ_REL. It also cannot be a stronger order than that
	 * specified by success_memorder.
	 */
	static bool compareExchange(X *P, X *expected, X desired,
			MemoryOrder forSuccess = MO_RELAXED, MemoryOrder forFailure =
					MO_RELAXED, bool weak = false) noexcept {
		return __atomic_compare_exchange_n(P, expected, desired, weak,
				forSuccess, forFailure);
	}
	//-----------------------------------------------------------------
	/**
	 * These built-in functions perform the operation suggested by their names,
	 * and return the result of the operation. All memory orders are valid.
	 */
	static X addAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_add_fetch(P, value, m);
	}

	static X subAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_sub_fetch(P, value, m);
	}

	static X andAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_and_fetch(P, value, m);
	}

	static X xorAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_xor_fetch(P, value, m);
	}

	static X orAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_or_fetch(P, value, m);
	}

	static X nandAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_nand_fetch(P, value, m);
	}

	static X fetchAndAdd(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_add(P, value, m);
	}

	static X fetchAndSub(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_sub(P, value, m);
	}

	static X fetchAndAnd(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_and(P, value, m);
	}

	static X fetchAndXor(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_xor(P, value, m);
	}

	static X fetchAndOr(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_or(P, value, m);
	}

	static X fetchAndNand(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_nand(P, value, m);
	}
	//-----------------------------------------------------------------
	/*
	 * Performs an atomic test-and-set operation on the byte at *P.
	 * The byte is set to some implementation defined nonzero “set” value and
	 * the return value is true if and only if the previous contents were “set”.
	 * It should be only used for operands of type bool or char.
	 * For other types only part of the value may be set.
	 * All memory orders are valid.
	 */
	static bool testAndSet(X *P, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_test_and_set(P, m);
	}

	/*
	 * Performs an atomic clear operation on *ptr. After the operation, *P contains 0.
	 * It should be only used for operands of type bool or char and in conjunction
	 * with testAndSet. For other types it may only clear partially. If the type
	 * is not bool prefer using store. Valid memory orders: MM_RELAXED, MM_SEQ_CST,
	 * and MM_RELEASE.
	 */
	static void clear(X *P, MemoryOrder m = MO_RELAXED) noexcept {
		__atomic_clear(P, m);
	}

	/*
	 * Synchronization fence between threads based on the specified memory order.
	 * All memory orders are valid.
	 */
	static void threadFence(MemoryOrder m) noexcept {
		__atomic_thread_fence(m);
	}

	/*
	 * Synchronization fence between a thread and signal handlers based in the
	 * same thread. All memory orders are valid.
	 */
	static void signalFence(MemoryOrder m) noexcept {
		__atomic_signal_fence(m);
	}

	/*
	 * Returns true if objects of type X always generate lock-free atomic instructions
	 * for the target architecture. Result resolves to a compile-time constant.
	 * P is an optional pointer to the object that may be used to determine alignment.
	 * A value of 0 indicates typical alignment should be used. The compiler may also
	 * ignore this parameter.
	 */
	static constexpr bool isAlwaysLockFree(const X *P = 0) noexcept {
		//Size parameter must resolve to compile time constant
		return __atomic_always_lock_free(sizeof(X), P);
	}

	/*
	 * Returns true if objects of type X always generate lock-free atomic instructions
	 * for the target architecture. If the built-in function is not known to be
	 * lock-free, a call is made to a runtime routine named __atomic_is_lock_free.
	 * P is an optional pointer to the object that may be used to determine alignment.
	 * A value of 0 indicates typical alignment should be used. The compiler may also
	 * ignore this parameter.
	 */
	static bool isLockFree(const X *P = 0) noexcept {
		return __atomic_is_lock_free(sizeof(X), P);
	}
private:
	WH_POD_ASSERT(X);
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_ATOMIC_H_ */
