// xatomic.h internal header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _XATOMIC_H
#define _XATOMIC_H
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#include <intrin0.h>
#include <type_traits>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

// Interlocked intrinsic mapping for _nf/_acq/_rel
#if defined(_M_CEE_PURE) || defined(_M_IX86) || defined(_M_X64)
#define _INTRIN_RELAXED(x) x
#define _INTRIN_ACQUIRE(x) x
#define _INTRIN_RELEASE(x) x
#define _INTRIN_ACQ_REL(x) x
#define _YIELD_PROCESSOR()

#elif defined(_M_ARM) || defined(_M_ARM64)
#define _INTRIN_RELAXED(x) _CONCAT(x, _nf)
#define _INTRIN_ACQUIRE(x) _CONCAT(x, _acq)
#define _INTRIN_RELEASE(x) _CONCAT(x, _rel)
// We don't have interlocked intrinsics for acquire-release ordering, even on
// ARM32/ARM64, so fall back to sequentially consistent.
#define _INTRIN_ACQ_REL(x) x
#define _YIELD_PROCESSOR() __yield()

#else // ^^^ ARM32/ARM64 / unsupported hardware vvv
#error Unsupported hardware
#endif // hardware

#define _MT_INCR(x) _INTRIN_RELAXED(_InterlockedIncrement)(reinterpret_cast<volatile long*>(&x))
#define _MT_DECR(x) _INTRIN_ACQ_REL(_InterlockedDecrement)(reinterpret_cast<volatile long*>(&x))

#if defined(_M_CEE_PURE) || defined(_M_IX86) || defined(_M_X64)
#define _ISO_VOLATILE_LOAD32(_Storage) (*_Atomic_address_as<const long>(_Storage))

#elif defined(_M_ARM) || defined(_M_ARM64)
#define _ISO_VOLATILE_LOAD32(_Storage) __iso_volatile_load32(_Atomic_address_as<const int>(_Storage))

#else // ^^^ ARM32/ARM64 / unsupported hardware vvv
#error Unsupported hardware
#endif // hardware

_STD_BEGIN

#if _HAS_CXX20
// ENUM CLASS memory_order
enum class memory_order : int {
    relaxed,
    consume,
    acquire,
    release,
    acq_rel,
    seq_cst,

    // LWG-3268
    memory_order_relaxed = relaxed,
    memory_order_consume = consume,
    memory_order_acquire = acquire,
    memory_order_release = release,
    memory_order_acq_rel = acq_rel,
    memory_order_seq_cst = seq_cst
};
inline constexpr memory_order memory_order_relaxed = memory_order::relaxed;
inline constexpr memory_order memory_order_consume = memory_order::consume;
inline constexpr memory_order memory_order_acquire = memory_order::acquire;
inline constexpr memory_order memory_order_release = memory_order::release;
inline constexpr memory_order memory_order_acq_rel = memory_order::acq_rel;
inline constexpr memory_order memory_order_seq_cst = memory_order::seq_cst;
#else // _HAS_CXX20
// ENUM memory_order
enum memory_order {
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
};
#endif // _HAS_CXX20

using _Atomic_counter_t = unsigned long;

// FUNCTION TEMPLATE _Atomic_address_as
template <class _Integral, class _Ty>
_NODISCARD volatile _Integral* _Atomic_address_as(_Ty& _Source) noexcept {
    // gets a pointer to the argument as an integral type (to pass to intrinsics)
    static_assert(is_integral_v<_Integral>, "Tried to reinterpret memory as non-integral");
    return &reinterpret_cast<volatile _Integral&>(_Source);
}

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XATOMIC_H
