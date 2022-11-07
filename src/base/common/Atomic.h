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
#include "reflect.h"
#ifndef WH_GCC
#error "This package must be compiled by GCC."
#endif

#if !defined(WH_GCC_VERSION) || !(WH_GCC_VERSION >= 40700)
#error "Unsupported GCC version."
#endif

namespace wanhive {
/**
 * Enumeration of memory orders
 */
enum MemoryOrder {
	MO_RELAXED = __ATOMIC_RELAXED,/**< No inter-thread ordering constraints. */
	MO_CONSUME = __ATOMIC_CONSUME,/**< Equivalent to MO_ACQUIRE */
	MO_ACQUIRE = __ATOMIC_ACQUIRE,/**< Read-Acquire barrier */
	MO_RELEASE = __ATOMIC_RELEASE,/**< Write-Release barrier */
	MO_ACQ_REL = __ATOMIC_ACQ_REL,/**< Combines Acquire-Release */
	MO_SEQ_CST = __ATOMIC_SEQ_CST /**< Enforces total ordering */
};

/**
 * Following operations can be used with any integral scalar or pointer type
 * that is 1, 2, 4, or 8 bytes in length. If ‘__int128’ (see __int128) is
 * supported by the architecture then 16-byte integral types are also allowed.
 * @ref https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
 * @tparam X integral or pointer data type
 */
template<typename X = unsigned int> class Atomic {
public:
	/**
	 * Atomic load operation, returns the contents of *P.
	 * @param P the memory location
	 * @param m the memory order (MO_RELAXED, MO_SEQ_CST, MO_ACQUIRE, or MO_CONSUME)
	 * @return contents of *P
	 */
	static X load(const X *P, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_load_n(P, m);
	}
	/**
	 * Atomic store operation. It writes value into *P.
	 * @param P the memory location
	 * @param value the value to write into *P
	 * @param m the memory order (MO_RELAXED, MO_SEQ_CST, or MO_RELEASE)
	 */
	static void store(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		__atomic_store_n(P, value, m);
	}
	//-----------------------------------------------------------------
	/**
	 * Atomic exchange operation. It writes value into *P, and returns the
	 * previous contents of *P.
	 * @param P the memory location
	 * @param value the value to write into *P
	 * @param m the memory order (MO_RELAXED, MO_SEQ_CST, MO_ACQUIRE, MO_RELEASE,
	 * or MO_ACQ_REL)
	 * @return the value that had previously been in *P.
	 */
	static X exchange(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_exchange_n(P, value, m);
	}
	/**
	 * Atomic compare and exchange (CAS) operation: this compares the contents
	 * of *P with the contents of *expected. If equal, the operation is a
	 * read-modify-write operation that writes desired into *P. If they are not
	 * equal, the operation is a read and the current contents of *P are written
	 * into *expected.
	 * @param P the memory location
	 * @param expected pointer to the expected value at *P
	 * @param desired the desired value to write at *P if *P equals *expected.
	 * @param forSuccess the memory order to use if *P equals *expected.
	 * @param forFailure the memory order to use if *P is different from
	 * *expected. It cannot be MO_RELEASE nor MO_ACQ_REL. It also cannot be a
	 * stronger order than that specified by forSuccess.
	 * @param weak true for weak comparison which may fail spuriously, false for
	 * the strong variation which never fails spuriously. Many targets only offer
	 * the strong variation and ignore the parameter. When in doubt, use the
	 * strong variation.
	 * @return true if desired is written into *P, false otherwise.
	 */
	static bool compareExchange(X *P, X *expected, X desired,
			MemoryOrder forSuccess = MO_RELAXED, MemoryOrder forFailure =
					MO_RELAXED, bool weak = false) noexcept {
		return __atomic_compare_exchange_n(P, expected, desired, weak,
				forSuccess, forFailure);
	}
	//-----------------------------------------------------------------
	/**
	 * Add and fetch operation, adds the given value into *P and returns the
	 * result of the operation.
	 * @param P the memory location
	 * @param value the value to add into *P
	 * @param m the memory order to use
	 * @return result of the operation
	 */
	static X addAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_add_fetch(P, value, m);
	}
	/**
	 * Subtract and fetch operation, subtracts the given value from *P and
	 * returns the result of the operation.
	 * @param P the memory location
	 * @param value the value to subtract from *P
	 * @param m the memory order to use
	 * @return result of the operation
	 */
	static X subAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_sub_fetch(P, value, m);
	}
	/**
	 * AND and fetch operation, performs bitwise AND of the given value with the
	 * value at *P and returns the result of the operation.
	 * @param P the memory location
	 * @param value the value to bitwise AND into *P
	 * @param m the memory order to use
	 * @return result of the operation
	 */
	static X andAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_and_fetch(P, value, m);
	}
	/**
	 * XOR and fetch operation, performs bitwise XOR of the given value with the
	 * value at *P and returns the result of the operation.
	 * @param P the memory location
	 * @param value the value to bitwise XOR into *P
	 * @param m the memory order to use
	 * @return result of the operation
	 */
	static X xorAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_xor_fetch(P, value, m);
	}
	/**
	 * OR and fetch operation, performs bitwise OR of the given value with the
	 * value at *P and returns the result of the operation.
	 * @param P the memory location
	 * @param value the value to bitwise OR into *P
	 * @param m the memory order to use
	 * @return result of the operation
	 */
	static X orAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_or_fetch(P, value, m);
	}
	/**
	 * NAND and fetch operation, performs bitwise NAND of the given value with
	 * the value at *P and returns the result of the operation.
	 * @param P the memory location
	 * @param value the value to bitwise NAND into *P
	 * @param m the memory order to use
	 * @return result of the operation
	 */
	static X nandAndFetch(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_nand_fetch(P, value, m);
	}
	//-----------------------------------------------------------------
	/**
	 * Fetch and add operation, adds the given value into *P and returns the
	 * original value of *P.
	 * @param P the memory location
	 * @param value the value to add into *P
	 * @param m the memory order to use
	 * @return the value that had previously been in *P
	 */
	static X fetchAndAdd(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_add(P, value, m);
	}
	/**
	 * Fetch and subtract operation, subtracts the given value from *P and
	 * returns the original value of *P.
	 * @param P the memory location
	 * @param value the value to subtract from *P
	 * @param m the memory order to use
	 * @return the value that had previously been in *P
	 */
	static X fetchAndSub(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_sub(P, value, m);
	}
	/**
	 * Fetch and AND operation, performs bitwise AND of the given value with
	 * the value at *P and returns the original value of *P.
	 * @param P the memory location
	 * @param value the value to bitwise AND into *P
	 * @param m the memory order to use
	 * @return the value that had previously been in *P
	 */
	static X fetchAndAnd(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_and(P, value, m);
	}
	/**
	 * Fetch and XOR operation, performs bitwise XOR of the given value with
	 * the value at *P and returns the original value of *P.
	 * @param P the memory location
	 * @param value the value to bitwise XOR into *P
	 * @param m the memory order to use
	 * @return the value that had previously been in *P
	 */
	static X fetchAndXor(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_xor(P, value, m);
	}
	/**
	 * Fetch and OR operation, performs bitwise OR of the given value with
	 * the value at *P and returns the original value of *P.
	 * @param P the memory location
	 * @param value the value to bitwise OR into *P
	 * @param m the memory order to use
	 * @return the value that had previously been in *P
	 */
	static X fetchAndOr(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_or(P, value, m);
	}
	/**
	 * Fetch and NAND operation, performs bitwise NAND of the given value with
	 * the value at *P and returns the original value of *P.
	 * @param P the memory location
	 * @param value the value to bitwise NAND into *P
	 * @param m the memory order to use
	 * @return the value that had previously been in *P
	 */
	static X fetchAndNand(X *P, X value, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_fetch_nand(P, value, m);
	}
	//-----------------------------------------------------------------
	/**
	 * Performs an atomic test-and-set operation on the byte at *P. The byte is
	 * set to some implementation defined nonzero “set” value.
	 * @param P the memory location of type bool or char
	 * @param m the memory order to use
	 * @return true if the previous content were set, false otherwise
	 */
	static bool testAndSet(X *P, MemoryOrder m = MO_RELAXED) noexcept {
		return __atomic_test_and_set(P, m);
	}
	/**
	 * Performs an atomic clear operation on *P. Use in conjunction with testAndSet.
	 * After the operation, *P contains 0.
	 * @param P the memory location of type bool or char
	 * @param m the memory order to use (MO_RELAXED, MO_SEQ_CST, or MO_RELEASE).
	 */
	static void clear(X *P, MemoryOrder m = MO_RELAXED) noexcept {
		__atomic_clear(P, m);
	}
	/**
	 * Synchronization fence between threads based on the specified memory order.
	 * @param m the memory order to use (all memory orders are valid)
	 */
	static void threadFence(MemoryOrder m) noexcept {
		__atomic_thread_fence(m);
	}
	/**
	 * Synchronization fence between a thread and signal handlers based in the
	 * same thread.
	 * @param m the memory order to use (all memory orders are valid)
	 */
	static void signalFence(MemoryOrder m) noexcept {
		__atomic_signal_fence(m);
	}
	/**
	 * Returns true if objects of the given type always generate lock-free atomic
	 * instructions. Result resolves to a compile-time constant.
	 * @param P optional pointer to determine alignment. A value of 0 indicates
	 * typical alignment should be used. The compiler may also ignore this parameter.
	 * @return true if the objects of the given type always generate lock-free
	 * atomic instructions for the target architecture, false otherwise.
	 */
	static constexpr bool isAlwaysLockFree(const X *P = 0) noexcept {
		//Size parameter must resolve to compile time constant
		return __atomic_always_lock_free(sizeof(X), P);
	}
	/**
	 * Returns true if objects of the given type always generate lock-free atomic
	 * instructions for the target architecture.
	 * @param P optional pointer to the object that may be used to determine
	 * alignment. A value of 0 indicates typical alignment should be used. The
	 * compiler may also ignore this parameter.
	 * @return true if the objects of the given type always generate lock-free
	 * atomic instructions for the target architecture, false otherwise.
	 */
	static bool isLockFree(const X *P = 0) noexcept {
		return __atomic_is_lock_free(sizeof(X), P);
	}
private:
	WH_SCALAR_ASSERT(X);
	static_assert(sizeof(X) <= 16, "Invalid object size");
};

} /* namespace wanhive */

#endif /* WH_BASE_COMMON_ATOMIC_H_ */
