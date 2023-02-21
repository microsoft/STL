// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <__msvc_cxx_stdatomic.hpp>

static_assert(ATOMIC_BOOL_LOCK_FREE == 2);
static_assert(ATOMIC_CHAR_LOCK_FREE == 2);
static_assert(ATOMIC_CHAR16_T_LOCK_FREE == 2);
static_assert(ATOMIC_CHAR32_T_LOCK_FREE == 2);
static_assert(ATOMIC_WCHAR_T_LOCK_FREE == 2);
static_assert(ATOMIC_SHORT_LOCK_FREE == 2);
static_assert(ATOMIC_INT_LOCK_FREE == 2);
static_assert(ATOMIC_LONG_LOCK_FREE == 2);
static_assert(ATOMIC_LLONG_LOCK_FREE == 2);
static_assert(ATOMIC_POINTER_LOCK_FREE == 2);

#include <atomic>
#include <type_traits>

using std::is_same_v;

static_assert(is_same_v<_Atomic(int), std::atomic<int>>);
static_assert(is_same_v<_Atomic(unsigned int), std::atomic<unsigned int>>);
static_assert(is_same_v<_Atomic(float), std::atomic<float>>);
static_assert(is_same_v<_Atomic(char), std::atomic<char>>);

static_assert(is_same_v<std::memory_order, memory_order>);
static_assert(std::memory_order_relaxed == memory_order_relaxed);
static_assert(std::memory_order_consume == memory_order_consume);
static_assert(std::memory_order_acquire == memory_order_acquire);
static_assert(std::memory_order_release == memory_order_release);
static_assert(std::memory_order_acq_rel == memory_order_acq_rel);
static_assert(std::memory_order_seq_cst == memory_order_seq_cst);

static_assert(is_same_v<std::atomic_flag, atomic_flag>);

static_assert(is_same_v<std::atomic_bool, atomic_bool>);
static_assert(is_same_v<std::atomic_char, atomic_char>);
static_assert(is_same_v<std::atomic_schar, atomic_schar>);
static_assert(is_same_v<std::atomic_uchar, atomic_uchar>);
static_assert(is_same_v<std::atomic_short, atomic_short>);
static_assert(is_same_v<std::atomic_ushort, atomic_ushort>);
static_assert(is_same_v<std::atomic_int, atomic_int>);
static_assert(is_same_v<std::atomic_uint, atomic_uint>);
static_assert(is_same_v<std::atomic_long, atomic_long>);
static_assert(is_same_v<std::atomic_ulong, atomic_ulong>);
static_assert(is_same_v<std::atomic_llong, atomic_llong>);
static_assert(is_same_v<std::atomic_ullong, atomic_ullong>);

#ifdef __cpp_lib_char8_t
static_assert(is_same_v<std::atomic_char8_t, atomic_char8_t>);
#endif // __cpp_lib_char8_t

static_assert(is_same_v<std::atomic_char16_t, atomic_char16_t>);
static_assert(is_same_v<std::atomic_char32_t, atomic_char32_t>);
static_assert(is_same_v<std::atomic_wchar_t, atomic_wchar_t>);
static_assert(is_same_v<std::atomic_int8_t, atomic_int8_t>);
static_assert(is_same_v<std::atomic_uint8_t, atomic_uint8_t>);
static_assert(is_same_v<std::atomic_int16_t, atomic_int16_t>);
static_assert(is_same_v<std::atomic_uint16_t, atomic_uint16_t>);
static_assert(is_same_v<std::atomic_int32_t, atomic_int32_t>);
static_assert(is_same_v<std::atomic_uint32_t, atomic_uint32_t>);
static_assert(is_same_v<std::atomic_int64_t, atomic_int64_t>);
static_assert(is_same_v<std::atomic_uint64_t, atomic_uint64_t>);
static_assert(is_same_v<std::atomic_int_least8_t, atomic_int_least8_t>);
static_assert(is_same_v<std::atomic_uint_least8_t, atomic_uint_least8_t>);
static_assert(is_same_v<std::atomic_int_least16_t, atomic_int_least16_t>);
static_assert(is_same_v<std::atomic_uint_least16_t, atomic_uint_least16_t>);
static_assert(is_same_v<std::atomic_int_least32_t, atomic_int_least32_t>);
static_assert(is_same_v<std::atomic_uint_least32_t, atomic_uint_least32_t>);
static_assert(is_same_v<std::atomic_int_least64_t, atomic_int_least64_t>);
static_assert(is_same_v<std::atomic_uint_least64_t, atomic_uint_least64_t>);
static_assert(is_same_v<std::atomic_int_fast8_t, atomic_int_fast8_t>);
static_assert(is_same_v<std::atomic_uint_fast8_t, atomic_uint_fast8_t>);
static_assert(is_same_v<std::atomic_int_fast16_t, atomic_int_fast16_t>);
static_assert(is_same_v<std::atomic_uint_fast16_t, atomic_uint_fast16_t>);
static_assert(is_same_v<std::atomic_int_fast32_t, atomic_int_fast32_t>);
static_assert(is_same_v<std::atomic_uint_fast32_t, atomic_uint_fast32_t>);
static_assert(is_same_v<std::atomic_int_fast64_t, atomic_int_fast64_t>);
static_assert(is_same_v<std::atomic_uint_fast64_t, atomic_uint_fast64_t>);
static_assert(is_same_v<std::atomic_intptr_t, atomic_intptr_t>);
static_assert(is_same_v<std::atomic_uintptr_t, atomic_uintptr_t>);
static_assert(is_same_v<std::atomic_size_t, atomic_size_t>);
static_assert(is_same_v<std::atomic_ptrdiff_t, atomic_ptrdiff_t>);
static_assert(is_same_v<std::atomic_intmax_t, atomic_intmax_t>);
static_assert(is_same_v<std::atomic_uintmax_t, atomic_uintmax_t>);

namespace test {
    using ::atomic_compare_exchange_strong;
    using ::atomic_compare_exchange_strong_explicit;
    using ::atomic_compare_exchange_weak;
    using ::atomic_compare_exchange_weak_explicit;
    using ::atomic_exchange;
    using ::atomic_exchange_explicit;
    using ::atomic_fetch_add;
    using ::atomic_fetch_add_explicit;
    using ::atomic_fetch_and;
    using ::atomic_fetch_and_explicit;
    using ::atomic_fetch_or;
    using ::atomic_fetch_or_explicit;
    using ::atomic_fetch_sub;
    using ::atomic_fetch_sub_explicit;
    using ::atomic_fetch_xor;
    using ::atomic_fetch_xor_explicit;
    using ::atomic_flag_clear;
    using ::atomic_flag_clear_explicit;
    using ::atomic_flag_test_and_set;
    using ::atomic_flag_test_and_set_explicit;
    using ::atomic_is_lock_free;
    using ::atomic_load;
    using ::atomic_load_explicit;
    using ::atomic_store;
    using ::atomic_store_explicit;
} // namespace test

static_assert(std::atomic_thread_fence == atomic_thread_fence);
static_assert(std::atomic_signal_fence == atomic_signal_fence);
