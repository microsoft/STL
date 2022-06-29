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

#define _CONCATX(x, y) x##y
#define _CONCAT(x, y)  _CONCATX(x, y)

// Interlocked intrinsic mapping for _nf/_acq/_rel
#if defined(_M_CEE_PURE) || defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC))
#define _INTRIN_RELAXED(x) x
#define _INTRIN_ACQUIRE(x) x
#define _INTRIN_RELEASE(x) x
#define _INTRIN_ACQ_REL(x) x
#ifdef _M_CEE_PURE
#define _YIELD_PROCESSOR()
#else // ^^^ _M_CEE_PURE / !_M_CEE_PURE vvv
#define _YIELD_PROCESSOR() _mm_pause()
#endif // ^^^ !_M_CEE_PURE ^^^

#elif defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
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

#ifndef _INVALID_MEMORY_ORDER
#ifdef _DEBUG
#define _INVALID_MEMORY_ORDER _STL_REPORT_ERROR("Invalid memory order")
#else // ^^^ _DEBUG / !_DEBUG vvv
#define _INVALID_MEMORY_ORDER
#endif // _DEBUG
#endif // _INVALID_MEMORY_ORDER

#define _Compiler_barrier() _STL_DISABLE_DEPRECATED_WARNING _ReadWriteBarrier() _STL_RESTORE_DEPRECATED_WARNING

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
#define _Memory_barrier()             __dmb(0xB) // inner shared data memory barrier
#define _Compiler_or_memory_barrier() _Memory_barrier()
#elif defined(_M_IX86) || defined(_M_X64)
// x86/x64 hardware only emits memory barriers inside _Interlocked intrinsics
#define _Compiler_or_memory_barrier() _Compiler_barrier()
#else // ^^^ x86/x64 / unsupported hardware vvv
#error Unsupported hardware
#endif // hardware

#if defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC))
#define _ATOMIC_CHOOSE_INTRINSIC(_Order, _Result, _Intrinsic, ...) \
    _Check_memory_order(_Order);                                   \
    _Result = _Intrinsic(__VA_ARGS__)
#elif defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
#define _ATOMIC_CHOOSE_INTRINSIC(_Order, _Result, _Intrinsic, ...) \
    switch (_Order) {                                              \
    case memory_order_relaxed:                                     \
        _Result = _INTRIN_RELAXED(_Intrinsic)(__VA_ARGS__);        \
        break;                                                     \
    case memory_order_consume:                                     \
    case memory_order_acquire:                                     \
        _Result = _INTRIN_ACQUIRE(_Intrinsic)(__VA_ARGS__);        \
        break;                                                     \
    case memory_order_release:                                     \
        _Result = _INTRIN_RELEASE(_Intrinsic)(__VA_ARGS__);        \
        break;                                                     \
    default:                                                       \
        _INVALID_MEMORY_ORDER;                                     \
        /* [[fallthrough]]; */                                     \
    case memory_order_acq_rel:                                     \
    case memory_order_seq_cst:                                     \
        _Result = _Intrinsic(__VA_ARGS__);                         \
        break;                                                     \
    }
#endif // hardware

_EXTERN_C
enum {
    _Atomic_memory_order_relaxed,
    _Atomic_memory_order_consume,
    _Atomic_memory_order_acquire,
    _Atomic_memory_order_release,
    _Atomic_memory_order_acq_rel,
    _Atomic_memory_order_seq_cst,
};

inline void _Check_memory_order(const int _Order) {
    if (_Order > _Atomic_memory_order_seq_cst) {
        _INVALID_MEMORY_ORDER;
    }
}

inline void _Atomic_store8(volatile char* _Ptr, char _Desired, int _Order) {
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        __iso_volatile_store8(_Ptr, _Desired);
        return;
    case _Atomic_memory_order_release:
        _Compiler_or_memory_barrier();
        __iso_volatile_store8(_Ptr, _Desired);
        return;
    default:
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_acq_rel:
        _INVALID_MEMORY_ORDER;
        // [[fallthrough]];
    case _Atomic_memory_order_seq_cst:
#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
        _Memory_barrier();
        __iso_volatile_store8(_Ptr, _Desired);
        _Memory_barrier();
#else // ^^^ ARM32/ARM64/ARM64EC hardware / x86/x64 hardware vvv
        (void) _InterlockedExchange8(_Ptr, _Desired);
#endif // hardware
        return;
    }
}

inline void _Atomic_store16(volatile short* _Ptr, short _Desired, int _Order) {
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        __iso_volatile_store16(_Ptr, _Desired);
        return;
    case _Atomic_memory_order_release:
        _Compiler_or_memory_barrier();
        __iso_volatile_store16(_Ptr, _Desired);
        return;
    default:
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_acq_rel:
        _INVALID_MEMORY_ORDER;
        // [[fallthrough]];
    case _Atomic_memory_order_seq_cst:
#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
        _Memory_barrier();
        __iso_volatile_store16(_Ptr, _Desired);
        _Memory_barrier();
#else // ^^^ ARM32/ARM64/ARM64EC hardware / x86/x64 hardware vvv
        (void) _InterlockedExchange16(_Ptr, _Desired);
#endif // hardware
        return;
    }
}

inline void _Atomic_store32(volatile int* _Ptr, int _Desired, int _Order) {
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        __iso_volatile_store32(_Ptr, _Desired);
        return;
    case _Atomic_memory_order_release:
        _Compiler_or_memory_barrier();
        __iso_volatile_store32(_Ptr, _Desired);
        return;
    default:
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_acq_rel:
        _INVALID_MEMORY_ORDER;
        // [[fallthrough]];
    case _Atomic_memory_order_seq_cst:
#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
        _Memory_barrier();
        __iso_volatile_store32(_Ptr, _Desired);
        _Memory_barrier();
#else // ^^^ ARM32/ARM64/ARM64EC hardware / x86/x64 hardware vvv
        (void) _InterlockedExchange(reinterpret_cast<volatile long*>(_Ptr), static_cast<long>(_Desired));
#endif // hardware
        return;
    }
}

inline void _Atomic_store64(volatile long long* _Ptr, long long _Desired, int _Order) {
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        __iso_volatile_store64(_Ptr, _Desired);
        return;
    case _Atomic_memory_order_release:
        _Compiler_or_memory_barrier();
        __iso_volatile_store64(_Ptr, _Desired);
        return;
    default:
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_acq_rel:
        _INVALID_MEMORY_ORDER;
        // [[fallthrough]];
    case _Atomic_memory_order_seq_cst:
#if defined(_M_IX86)
        _Compiler_barrier();
        __iso_volatile_store64(_Ptr, _Desired);
        _STD atomic_thread_fence(memory_order_seq_cst);
#elif defined(_M_ARM64) || defined(_M_ARM64EC)
        _Memory_barrier();
        __iso_volatile_store64(_Ptr, _Desired);
        _Memory_barrier();
#else // ^^^ _M_ARM64, _M_ARM64EC / ARM32, x64 vvv
        (void) _InterlockedExchange64(_Ptr, _Desired);
#endif // ^^^ ARM32, x64 ^^^
        return;
    }
}
inline char _Atomic_load8(const volatile char* _Ptr, int _Order) {
    char _As_bytes = __iso_volatile_load8(_Ptr);
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        break;
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_seq_cst:
        _Compiler_or_memory_barrier();
        break;
    case _Atomic_memory_order_release:
    case _Atomic_memory_order_acq_rel:
    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
    return _As_bytes;
}

inline short _Atomic_load16(const volatile short* _Ptr, int _Order) {
    short _As_bytes = __iso_volatile_load16(_Ptr);
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        break;
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_seq_cst:
        _Compiler_or_memory_barrier();
        break;
    case _Atomic_memory_order_release:
    case _Atomic_memory_order_acq_rel:
    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
    return _As_bytes;
}

inline int _Atomic_load32(const volatile int* _Ptr, int _Order) {
    int _As_bytes = __iso_volatile_load32(_Ptr);
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        break;
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_seq_cst:
        _Compiler_or_memory_barrier();
        break;
    case _Atomic_memory_order_release:
    case _Atomic_memory_order_acq_rel:
    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
    return _As_bytes;
}

inline long long _Atomic_load64(const volatile long long* _Ptr, int _Order) {
#ifdef _M_ARM
    long long _As_bytes = __ldrexd(_Ptr);
#else
    long long _As_bytes = __iso_volatile_load64(_Ptr);
#endif
    switch (_Order) {
    case _Atomic_memory_order_relaxed:
        break;
    case _Atomic_memory_order_consume:
    case _Atomic_memory_order_acquire:
    case _Atomic_memory_order_seq_cst:
        _Compiler_or_memory_barrier();
        break;
    case _Atomic_memory_order_release:
    case _Atomic_memory_order_acq_rel:
    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
    return _As_bytes;
}

inline bool _Atomic_compare_exchange_strong8(volatile char* _Ptr, char* _Expected, char _Desired, int _Order) {
    char _Prev_bytes;
    char _Expected_bytes = *_Expected;
    _ATOMIC_CHOOSE_INTRINSIC(_Order, _Prev_bytes, _InterlockedCompareExchange8, _Ptr, _Desired, _Expected_bytes);
    if (_Prev_bytes == _Expected_bytes) {
        return true;
    }
    *_Expected = _Prev_bytes;
    return false;
}

inline bool _Atomic_compare_exchange_strong16(volatile short* _Ptr, short* _Expected, short _Desired, int _Order) {
    short _Prev_bytes;
    short _Expected_bytes = *_Expected;
    _ATOMIC_CHOOSE_INTRINSIC(_Order, _Prev_bytes, _InterlockedCompareExchange16, _Ptr, _Desired, _Expected_bytes);
    if (_Prev_bytes == _Expected_bytes) {
        return true;
    }
    *_Expected = _Prev_bytes;
    return false;
}

inline bool _Atomic_compare_exchange_strong32(volatile int* _Ptr, int* _Expected, int _Desired, int _Order) {
    long _Prev_bytes;
    long _Expected_bytes = *_Expected;
    _ATOMIC_CHOOSE_INTRINSIC(_Order, _Prev_bytes, _InterlockedCompareExchange, reinterpret_cast<volatile long*>(_Ptr),
        _Desired, _Expected_bytes);
    if (_Prev_bytes == _Expected_bytes) {
        return true;
    }
    *_Expected = _Prev_bytes;
    return false;
}

inline bool _Atomic_compare_exchange_strong64(
    volatile long long* _Ptr, long long* _Expected, long long _Desired, int _Order) {
    long long _Prev_bytes;
    long long _Expected_bytes = *_Expected;
    _ATOMIC_CHOOSE_INTRINSIC(_Order, _Prev_bytes, _InterlockedCompareExchange64, _Ptr, _Desired, _Expected_bytes);
    if (_Prev_bytes == _Expected_bytes) {
        return true;
    }
    *_Expected = _Prev_bytes;
    return false;
}

_END_EXTERN_C

_STD_BEGIN

#if _HAS_CXX20
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

template <class _Integral, class _Ty>
_NODISCARD volatile _Integral* _Atomic_address_as(_Ty& _Source) noexcept {
    // gets a pointer to the argument as an integral type (to pass to intrinsics)
    static_assert(is_integral_v<_Integral>, "Tried to reinterpret memory as non-integral");
    return &reinterpret_cast<volatile _Integral&>(_Source);
}

template <class _Integral, class _Ty>
_NODISCARD const volatile _Integral* _Atomic_address_as(const _Ty& _Source) noexcept {
    // gets a pointer to the argument as an integral type (to pass to intrinsics)
    static_assert(is_integral_v<_Integral>, "Tried to reinterpret memory as non-integral");
    return &reinterpret_cast<const volatile _Integral&>(_Source);
}

_STD_END

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _XATOMIC_H
