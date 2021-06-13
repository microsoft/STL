// stdatomic.h standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _STDATOMIC_H_
#define _STDATOMIC_H_
#include <yvals.h>
#if _STL_COMPILER_PREPROCESSOR

#ifdef _M_CEE_PURE
#error <stdatomic.h> is not supported when compiling with /clr:pure.
#endif // _M_CEE_PURE

#if !_HAS_CXX23
#pragma message("The contents of <stdatomic.h> are available only with C++23 or later.")
#else // ^^^ !_HAS_CXX23 / _HAS_CXX23 vvv

#include <atomic>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

template <class _Ty>
using _Std_atomic = std::atomic<_Ty>;

#define _Atomic(T) _Std_atomic<T>

// clang-format off
using std::memory_order;
using std::memory_order_relaxed;
using std::memory_order_consume;
using std::memory_order_acquire;
using std::memory_order_release;
using std::memory_order_acq_rel;
using std::memory_order_seq_cst;

using std::atomic_flag;

using std::atomic_char;
using std::atomic_schar;
using std::atomic_uchar;
using std::atomic_short;
using std::atomic_ushort;
using std::atomic_int;
using std::atomic_uint;
using std::atomic_long;
using std::atomic_ulong;
using std::atomic_llong;
using std::atomic_ullong;
using std::atomic_char16_t;
using std::atomic_char32_t;
using std::atomic_wchar_t;
using std::atomic_int8_t;
using std::atomic_uint8_t;
using std::atomic_int16_t;
using std::atomic_uint16_t;
using std::atomic_int32_t;
using std::atomic_uint32_t;
using std::atomic_int64_t;
using std::atomic_uint64_t;
using std::atomic_int_least8_t;
using std::atomic_uint_least8_t;
using std::atomic_int_least16_t;
using std::atomic_uint_least16_t;
using std::atomic_int_least32_t;
using std::atomic_uint_least32_t;
using std::atomic_int_least64_t;
using std::atomic_uint_least64_t;
using std::atomic_int_fast8_t;
using std::atomic_uint_fast8_t;
using std::atomic_int_fast16_t;
using std::atomic_uint_fast16_t;
using std::atomic_int_fast32_t;
using std::atomic_uint_fast32_t;
using std::atomic_int_fast64_t;
using std::atomic_uint_fast64_t;
using std::atomic_intptr_t;
using std::atomic_uintptr_t;
using std::atomic_size_t;
using std::atomic_ptrdiff_t;
using std::atomic_intmax_t;
using std::atomic_uintmax_t;

using std::atomic_is_lock_free;
using std::atomic_load;
using std::atomic_load_explicit;
using std::atomic_store;
using std::atomic_store_explicit;
using std::atomic_exchange;
using std::atomic_exchange_explicit;
using std::atomic_compare_exchange_strong;
using std::atomic_compare_exchange_strong_explicit;
using std::atomic_compare_exchange_weak;
using std::atomic_compare_exchange_weak_explicit;
using std::atomic_fetch_add;
using std::atomic_fetch_add_explicit;
using std::atomic_fetch_sub;
using std::atomic_fetch_sub_explicit;
using std::atomic_fetch_or;
using std::atomic_fetch_or_explicit;
using std::atomic_fetch_and;
using std::atomic_fetch_and_explicit;
using std::atomic_flag_test_and_set;
using std::atomic_flag_test_and_set_explicit;
using std::atomic_flag_clear;
using std::atomic_flag_clear_explicit;

using std::atomic_thread_fence;
using std::atomic_signal_fence;
// clang-format on

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // ^^^ _HAS_CXX23 ^^^

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _STDATOMIC_H_
