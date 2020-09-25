// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {} // COMPILE-ONLY

// LIBRARY FEATURE-TEST MACROS

#ifndef __cpp_lib_addressof_constexpr
#error __cpp_lib_addressof_constexpr is not defined
#elif __cpp_lib_addressof_constexpr != 201603L
#error __cpp_lib_addressof_constexpr is not 201603L
#else
STATIC_ASSERT(__cpp_lib_addressof_constexpr == 201603L);
#endif

#ifndef __cpp_lib_allocator_traits_is_always_equal
#error __cpp_lib_allocator_traits_is_always_equal is not defined
#elif __cpp_lib_allocator_traits_is_always_equal != 201411L
#error __cpp_lib_allocator_traits_is_always_equal is not 201411L
#else
STATIC_ASSERT(__cpp_lib_allocator_traits_is_always_equal == 201411L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_any
#error __cpp_lib_any is not defined
#elif __cpp_lib_any != 201606L
#error __cpp_lib_any is not 201606L
#else
STATIC_ASSERT(__cpp_lib_any == 201606L);
#endif
#else
#ifdef __cpp_lib_any
#error __cpp_lib_any is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_apply
#error __cpp_lib_apply is not defined
#elif __cpp_lib_apply != 201603L
#error __cpp_lib_apply is not 201603L
#else
STATIC_ASSERT(__cpp_lib_apply == 201603L);
#endif
#else
#ifdef __cpp_lib_apply
#error __cpp_lib_apply is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_array_constexpr
#error __cpp_lib_array_constexpr is not defined
#elif __cpp_lib_array_constexpr != 201811L
#error __cpp_lib_array_constexpr is not 201811L
#else
STATIC_ASSERT(__cpp_lib_array_constexpr == 201811L);
#endif
#elif _HAS_CXX17
#ifndef __cpp_lib_array_constexpr
#error __cpp_lib_array_constexpr is not defined
#elif __cpp_lib_array_constexpr != 201803L
#error __cpp_lib_array_constexpr is not 201803L
#else
STATIC_ASSERT(__cpp_lib_array_constexpr == 201803L);
#endif
#else
#ifdef __cpp_lib_array_constexpr
#error __cpp_lib_array_constexpr is defined
#endif
#endif

#ifndef __cpp_lib_as_const
#error __cpp_lib_as_const is not defined
#elif __cpp_lib_as_const != 201510L
#error __cpp_lib_as_const is not 201510L
#else
STATIC_ASSERT(__cpp_lib_as_const == 201510L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_assume_aligned
#error __cpp_lib_assume_aligned is not defined
#elif __cpp_lib_assume_aligned != 201811L
#error __cpp_lib_assume_aligned is not 201811L
#else
STATIC_ASSERT(__cpp_lib_assume_aligned == 201811L);
#endif
#else
#ifdef __cpp_lib_assume_aligned
#error __cpp_lib_assume_aligned is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_flag_test
#error __cpp_lib_atomic_flag_test is not defined
#elif __cpp_lib_atomic_flag_test != 201907L
#error __cpp_lib_atomic_flag_test is not 201907L
#else
STATIC_ASSERT(__cpp_lib_atomic_flag_test == 201907L);
#endif
#else
#ifdef __cpp_lib_atomic_flag_test
#error __cpp_lib_atomic_flag_test is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_float
#error __cpp_lib_atomic_float is not defined
#elif __cpp_lib_atomic_float != 201711L
#error __cpp_lib_atomic_float is not 201711L
#else
STATIC_ASSERT(__cpp_lib_atomic_float == 201711L);
#endif
#else
#ifdef __cpp_lib_atomic_float
#error __cpp_lib_atomic_float is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_atomic_is_always_lock_free
#error __cpp_lib_atomic_is_always_lock_free is not defined
#elif __cpp_lib_atomic_is_always_lock_free != 201603L
#error __cpp_lib_atomic_is_always_lock_free is not 201603L
#else
STATIC_ASSERT(__cpp_lib_atomic_is_always_lock_free == 201603L);
#endif
#else
#ifdef __cpp_lib_atomic_is_always_lock_free
#error __cpp_lib_atomic_is_always_lock_free is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_lock_free_type_aliases
#error __cpp_lib_atomic_lock_free_type_aliases is not defined
#elif __cpp_lib_atomic_lock_free_type_aliases != 201907L
#error __cpp_lib_atomic_lock_free_type_aliases is not 201907L
#else
STATIC_ASSERT(__cpp_lib_atomic_lock_free_type_aliases == 201907L);
#endif
#else
#ifdef __cpp_lib_atomic_lock_free_type_aliases
#error __cpp_lib_atomic_lock_free_type_aliases is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_ref
#error __cpp_lib_atomic_ref is not defined
#elif __cpp_lib_atomic_ref != 201806L
#error __cpp_lib_atomic_ref is not 201806L
#else
STATIC_ASSERT(__cpp_lib_atomic_ref == 201806L);
#endif
#else
#ifdef __cpp_lib_atomic_ref
#error __cpp_lib_atomic_ref is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_shared_ptr
#error __cpp_lib_atomic_shared_ptr is not defined
#elif __cpp_lib_atomic_shared_ptr != 201711L
#error __cpp_lib_atomic_shared_ptr is not 201711L
#else
STATIC_ASSERT(__cpp_lib_atomic_shared_ptr == 201711L);
#endif
#else
#ifdef __cpp_lib_atomic_shared_ptr
#error __cpp_lib_atomic_shared_ptr is defined
#endif
#endif

#ifndef __cpp_lib_atomic_value_initialization
#error __cpp_lib_atomic_value_initialization is not defined
#elif __cpp_lib_atomic_value_initialization != 201911L
#error __cpp_lib_atomic_value_initialization is not 201911L
#else
STATIC_ASSERT(__cpp_lib_atomic_value_initialization == 201911L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_wait
#error __cpp_lib_atomic_wait is not defined
#elif __cpp_lib_atomic_wait != 201907L
#error __cpp_lib_atomic_wait is not 201907L
#else
STATIC_ASSERT(__cpp_lib_atomic_wait == 201907L);
#endif
#else
#ifdef __cpp_lib_atomic_wait
#error __cpp_lib_atomic_wait is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_barrier
#error __cpp_lib_barrier is not defined
#elif __cpp_lib_barrier != 201907L
#error __cpp_lib_barrier is not 201907L
#else
STATIC_ASSERT(__cpp_lib_barrier == 201907L);
#endif
#else
#ifdef __cpp_lib_barrier
#error __cpp_lib_barrier is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bind_front
#error __cpp_lib_bind_front is not defined
#elif __cpp_lib_bind_front != 201907L
#error __cpp_lib_bind_front is not 201907L
#else
STATIC_ASSERT(__cpp_lib_bind_front == 201907L);
#endif
#else
#ifdef __cpp_lib_bind_front
#error __cpp_lib_bind_front is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bit_cast
#error __cpp_lib_bit_cast is not defined
#elif __cpp_lib_bit_cast != 201806L
#error __cpp_lib_bit_cast is not 201806L
#else
STATIC_ASSERT(__cpp_lib_bit_cast == 201806L);
#endif
#else
#ifdef __cpp_lib_bit_cast
#error __cpp_lib_bit_cast is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bitops
#error __cpp_lib_bitops is not defined
#elif __cpp_lib_bitops != 201907L
#error __cpp_lib_bitops is not 201907L
#else
STATIC_ASSERT(__cpp_lib_bitops == 201907L);
#endif
#else
#ifdef __cpp_lib_bitops
#error __cpp_lib_bitops is defined
#endif
#endif

#ifndef __cpp_lib_bool_constant
#error __cpp_lib_bool_constant is not defined
#elif __cpp_lib_bool_constant != 201505L
#error __cpp_lib_bool_constant is not 201505L
#else
STATIC_ASSERT(__cpp_lib_bool_constant == 201505L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bounded_array_traits
#error __cpp_lib_bounded_array_traits is not defined
#elif __cpp_lib_bounded_array_traits != 201902L
#error __cpp_lib_bounded_array_traits is not 201902L
#else
STATIC_ASSERT(__cpp_lib_bounded_array_traits == 201902L);
#endif
#else
#ifdef __cpp_lib_bounded_array_traits
#error __cpp_lib_bounded_array_traits is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_boyer_moore_searcher
#error __cpp_lib_boyer_moore_searcher is not defined
#elif __cpp_lib_boyer_moore_searcher != 201603L
#error __cpp_lib_boyer_moore_searcher is not 201603L
#else
STATIC_ASSERT(__cpp_lib_boyer_moore_searcher == 201603L);
#endif
#else
#ifdef __cpp_lib_boyer_moore_searcher
#error __cpp_lib_boyer_moore_searcher is defined
#endif
#endif

#if _HAS_STD_BYTE
#ifndef __cpp_lib_byte
#error __cpp_lib_byte is not defined
#elif __cpp_lib_byte != 201603L
#error __cpp_lib_byte is not 201603L
#else
STATIC_ASSERT(__cpp_lib_byte == 201603L);
#endif
#else
#ifdef __cpp_lib_byte
#error __cpp_lib_byte is defined
#endif
#endif

#if _HAS_CXX20 && defined(__cpp_char8_t)
#ifndef __cpp_lib_char8_t
#error __cpp_lib_char8_t is not defined
#elif __cpp_lib_char8_t != 201907L
#error __cpp_lib_char8_t is not 201907L
#else
STATIC_ASSERT(__cpp_lib_char8_t == 201907L);
#endif
#else
#ifdef __cpp_lib_char8_t
#error __cpp_lib_char8_t is defined
#endif
#endif

#ifndef __cpp_lib_chrono
#error __cpp_lib_chrono is not defined
#elif _HAS_CXX17
#if __cpp_lib_chrono != 201611L
#error __cpp_lib_chrono is not 201611L
#else
STATIC_ASSERT(__cpp_lib_chrono == 201611L);
#endif
#else
#if __cpp_lib_chrono != 201510L
#error __cpp_lib_chrono is not 201510L
#else
STATIC_ASSERT(__cpp_lib_chrono == 201510L);
#endif
#endif

#ifndef __cpp_lib_chrono_udls
#error __cpp_lib_chrono_udls is not defined
#elif __cpp_lib_chrono_udls != 201304L
#error __cpp_lib_chrono_udls is not 201304L
#else
STATIC_ASSERT(__cpp_lib_chrono_udls == 201304L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_clamp
#error __cpp_lib_clamp is not defined
#elif __cpp_lib_clamp != 201603L
#error __cpp_lib_clamp is not 201603L
#else
STATIC_ASSERT(__cpp_lib_clamp == 201603L);
#endif
#else
#ifdef __cpp_lib_clamp
#error __cpp_lib_clamp is defined
#endif
#endif

#ifndef __cpp_lib_complex_udls
#error __cpp_lib_complex_udls is not defined
#elif __cpp_lib_complex_udls != 201309L
#error __cpp_lib_complex_udls is not 201309L
#else
STATIC_ASSERT(__cpp_lib_complex_udls == 201309L);
#endif

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, EDG concepts support
#ifndef __cpp_lib_concepts
#error __cpp_lib_concepts is not defined
#elif __cpp_lib_concepts != 201907L
#error __cpp_lib_concepts is not 201907L
#else
STATIC_ASSERT(__cpp_lib_concepts == 201907L);
#endif
#else
#ifdef __cpp_lib_concepts
#error __cpp_lib_concepts is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_algorithms
#error __cpp_lib_constexpr_algorithms is not defined
#elif __cpp_lib_constexpr_algorithms != 201806L
#error __cpp_lib_constexpr_algorithms is not 201806L
#else
STATIC_ASSERT(__cpp_lib_constexpr_algorithms == 201806L);
#endif
#else
#ifdef __cpp_lib_constexpr_algorithms
#error __cpp_lib_constexpr_algorithms is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_complex
#error __cpp_lib_constexpr_complex is not defined
#elif __cpp_lib_constexpr_complex != 201711L
#error __cpp_lib_constexpr_complex is not 201711L
#else
STATIC_ASSERT(__cpp_lib_constexpr_complex == 201711L);
#endif
#else
#ifdef __cpp_lib_constexpr_complex
#error __cpp_lib_constexpr_complex is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_functional
#error __cpp_lib_constexpr_functional is not defined
#elif __cpp_lib_constexpr_functional != 201907L
#error __cpp_lib_constexpr_functional is not 201907L
#else
STATIC_ASSERT(__cpp_lib_constexpr_functional == 201907L);
#endif
#else
#ifdef __cpp_lib_constexpr_functional
#error __cpp_lib_constexpr_functional is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_iterator
#error __cpp_lib_constexpr_iterator is not defined
#elif __cpp_lib_constexpr_iterator != 201811L
#error __cpp_lib_constexpr_iterator is not 201811L
#else
STATIC_ASSERT(__cpp_lib_constexpr_iterator == 201811L);
#endif
#else
#ifdef __cpp_lib_constexpr_iterator
#error __cpp_lib_constexpr_iterator is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_memory
#error __cpp_lib_constexpr_memory is not defined
#elif __cpp_lib_constexpr_memory != 201811L
#error __cpp_lib_constexpr_memory is not 201811L
#else
STATIC_ASSERT(__cpp_lib_constexpr_memory == 201811L);
#endif
#else
#ifdef __cpp_lib_constexpr_memory
#error __cpp_lib_constexpr_memory is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_numeric
#error __cpp_lib_constexpr_numeric is not defined
#elif __cpp_lib_constexpr_numeric != 201911L
#error __cpp_lib_constexpr_numeric is not 201911L
#else
STATIC_ASSERT(__cpp_lib_constexpr_numeric == 201911L);
#endif
#else
#ifdef __cpp_lib_constexpr_numeric
#error __cpp_lib_constexpr_numeric is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_string_view
#error __cpp_lib_constexpr_string_view is not defined
#elif __cpp_lib_constexpr_string_view != 201811L
#error __cpp_lib_constexpr_string_view is not 201811L
#else
STATIC_ASSERT(__cpp_lib_constexpr_string_view == 201811L);
#endif
#else
#ifdef __cpp_lib_constexpr_string_view
#error __cpp_lib_constexpr_string_view is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_tuple
#error __cpp_lib_constexpr_tuple is not defined
#elif __cpp_lib_constexpr_tuple != 201811L
#error __cpp_lib_constexpr_tuple is not 201811L
#else
STATIC_ASSERT(__cpp_lib_constexpr_tuple == 201811L);
#endif
#else
#ifdef __cpp_lib_constexpr_tuple
#error __cpp_lib_constexpr_tuple is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_utility
#error __cpp_lib_constexpr_utility is not defined
#elif __cpp_lib_constexpr_utility != 201811L
#error __cpp_lib_constexpr_utility is not 201811L
#else
STATIC_ASSERT(__cpp_lib_constexpr_utility == 201811L);
#endif
#else
#ifdef __cpp_lib_constexpr_utility
#error __cpp_lib_constexpr_utility is defined
#endif
#endif

#if _HAS_CXX20 && defined(__cpp_impl_coroutine) // TRANSITION, Clang and EDG coroutine support
#if __cpp_impl_coroutine >= 201902L
#define ExpectedCppLibCoroutine 201902L
#else
#define ExpectedCppLibCoroutine 197000L // TRANSITION, VS 2019 16.8 Preview 4
#endif
#ifndef __cpp_lib_coroutine
#error __cpp_lib_coroutine is not defined
#elif __cpp_lib_coroutine != ExpectedCppLibCoroutine
#error __cpp_lib_coroutine is not ExpectedCppLibCoroutine
#else
STATIC_ASSERT(__cpp_lib_coroutine == ExpectedCppLibCoroutine);
#endif
#else
#ifdef __cpp_lib_coroutine
#error __cpp_lib_coroutine is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_destroying_delete
#error __cpp_lib_destroying_delete is not defined
#elif __cpp_lib_destroying_delete != 201806L
#error __cpp_lib_destroying_delete is not 201806L
#else
STATIC_ASSERT(__cpp_lib_destroying_delete == 201806L);
#endif
#else
#ifdef __cpp_lib_destroying_delete
#error __cpp_lib_destroying_delete is defined
#endif
#endif

#ifndef __cpp_lib_enable_shared_from_this
#error __cpp_lib_enable_shared_from_this is not defined
#elif __cpp_lib_enable_shared_from_this != 201603L
#error __cpp_lib_enable_shared_from_this is not 201603L
#else
STATIC_ASSERT(__cpp_lib_enable_shared_from_this == 201603L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_endian
#error __cpp_lib_endian is not defined
#elif __cpp_lib_endian != 201907L
#error __cpp_lib_endian is not 201907L
#else
STATIC_ASSERT(__cpp_lib_endian == 201907L);
#endif
#else
#ifdef __cpp_lib_endian
#error __cpp_lib_endian is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_erase_if
#error __cpp_lib_erase_if is not defined
#elif __cpp_lib_erase_if != 202002L
#error __cpp_lib_erase_if is not 202002L
#else
STATIC_ASSERT(__cpp_lib_erase_if == 202002L);
#endif
#else
#ifdef __cpp_lib_erase_if
#error __cpp_lib_erase_if is defined
#endif
#endif

#ifndef __cpp_lib_exchange_function
#error __cpp_lib_exchange_function is not defined
#elif __cpp_lib_exchange_function != 201304L
#error __cpp_lib_exchange_function is not 201304L
#else
STATIC_ASSERT(__cpp_lib_exchange_function == 201304L);
#endif

#if _HAS_CXX20 && !defined(_M_CEE)
#ifndef __cpp_lib_execution
#error __cpp_lib_execution is not defined
#elif __cpp_lib_execution != 201902L
#error __cpp_lib_execution is not 201902L
#else
STATIC_ASSERT(__cpp_lib_execution == 201902L);
#endif
#elif _HAS_CXX17 && !defined(_M_CEE)
#ifndef __cpp_lib_execution
#error __cpp_lib_execution is not defined
#elif __cpp_lib_execution != 201603L
#error __cpp_lib_execution is not 201603L
#else
STATIC_ASSERT(__cpp_lib_execution == 201603L);
#endif
#else
#ifdef __cpp_lib_execution
#error __cpp_lib_execution is defined
#endif
#endif

#ifndef __cpp_lib_experimental_erase_if
#error __cpp_lib_experimental_erase_if is not defined
#elif __cpp_lib_experimental_erase_if != 201411L
#error __cpp_lib_experimental_erase_if is not 201411L
#else
STATIC_ASSERT(__cpp_lib_experimental_erase_if == 201411L);
#endif

#ifndef __cpp_lib_experimental_filesystem
#error __cpp_lib_experimental_filesystem is not defined
#elif __cpp_lib_experimental_filesystem != 201406L
#error __cpp_lib_experimental_filesystem is not 201406L
#else
STATIC_ASSERT(__cpp_lib_experimental_filesystem == 201406L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_filesystem
#error __cpp_lib_filesystem is not defined
#elif __cpp_lib_filesystem != 201703L
#error __cpp_lib_filesystem is not 201703L
#else
STATIC_ASSERT(__cpp_lib_filesystem == 201703L);
#endif
#else
#ifdef __cpp_lib_filesystem
#error __cpp_lib_filesystem is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_gcd_lcm
#error __cpp_lib_gcd_lcm is not defined
#elif __cpp_lib_gcd_lcm != 201606L
#error __cpp_lib_gcd_lcm is not 201606L
#else
STATIC_ASSERT(__cpp_lib_gcd_lcm == 201606L);
#endif
#else
#ifdef __cpp_lib_gcd_lcm
#error __cpp_lib_gcd_lcm is defined
#endif
#endif

#ifndef __cpp_lib_generic_associative_lookup
#error __cpp_lib_generic_associative_lookup is not defined
#elif __cpp_lib_generic_associative_lookup != 201304L
#error __cpp_lib_generic_associative_lookup is not 201304L
#else
STATIC_ASSERT(__cpp_lib_generic_associative_lookup == 201304L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_generic_unordered_lookup
#error __cpp_lib_generic_unordered_lookup is not defined
#elif __cpp_lib_generic_unordered_lookup != 201811L
#error __cpp_lib_generic_unordered_lookup is not 201811L
#else
STATIC_ASSERT(__cpp_lib_generic_unordered_lookup == 201811L);
#endif
#else
#ifdef __cpp_lib_generic_unordered_lookup
#error __cpp_lib_generic_unordered_lookup is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_hardware_interference_size
#error __cpp_lib_hardware_interference_size is not defined
#elif __cpp_lib_hardware_interference_size != 201703L
#error __cpp_lib_hardware_interference_size is not 201703L
#else
STATIC_ASSERT(__cpp_lib_hardware_interference_size == 201703L);
#endif
#else
#ifdef __cpp_lib_hardware_interference_size
#error __cpp_lib_hardware_interference_size is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_has_unique_object_representations
#error __cpp_lib_has_unique_object_representations is not defined
#elif __cpp_lib_has_unique_object_representations != 201606L
#error __cpp_lib_has_unique_object_representations is not 201606L
#else
STATIC_ASSERT(__cpp_lib_has_unique_object_representations == 201606L);
#endif
#else
#ifdef __cpp_lib_has_unique_object_representations
#error __cpp_lib_has_unique_object_representations is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_hypot
#error __cpp_lib_hypot is not defined
#elif __cpp_lib_hypot != 201603L
#error __cpp_lib_hypot is not 201603L
#else
STATIC_ASSERT(__cpp_lib_hypot == 201603L);
#endif
#else
#ifdef __cpp_lib_hypot
#error __cpp_lib_hypot is defined
#endif
#endif

#ifndef __cpp_lib_incomplete_container_elements
#error __cpp_lib_incomplete_container_elements is not defined
#elif __cpp_lib_incomplete_container_elements != 201505L
#error __cpp_lib_incomplete_container_elements is not 201505L
#else
STATIC_ASSERT(__cpp_lib_incomplete_container_elements == 201505L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_int_pow2
#error __cpp_lib_int_pow2 is not defined
#elif __cpp_lib_int_pow2 != 202002L
#error __cpp_lib_int_pow2 is not 202002L
#else
STATIC_ASSERT(__cpp_lib_int_pow2 == 202002L);
#endif
#else
#ifdef __cpp_lib_int_pow2
#error __cpp_lib_int_pow2 is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_integer_comparison_functions
#error __cpp_lib_integer_comparison_functions is not defined
#elif __cpp_lib_integer_comparison_functions != 202002L
#error __cpp_lib_integer_comparison_functions is not 202002L
#else
STATIC_ASSERT(__cpp_lib_integer_comparison_functions == 202002L);
#endif
#else
#ifdef __cpp_lib_integer_comparison_functions
#error __cpp_lib_integer_comparison_functions is defined
#endif
#endif

#ifndef __cpp_lib_integer_sequence
#error __cpp_lib_integer_sequence is not defined
#elif __cpp_lib_integer_sequence != 201304L
#error __cpp_lib_integer_sequence is not 201304L
#else
STATIC_ASSERT(__cpp_lib_integer_sequence == 201304L);
#endif

#ifndef __cpp_lib_integral_constant_callable
#error __cpp_lib_integral_constant_callable is not defined
#elif __cpp_lib_integral_constant_callable != 201304L
#error __cpp_lib_integral_constant_callable is not 201304L
#else
STATIC_ASSERT(__cpp_lib_integral_constant_callable == 201304L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_interpolate
#error __cpp_lib_interpolate is not defined
#elif __cpp_lib_interpolate != 201902L
#error __cpp_lib_interpolate is not 201902L
#else
STATIC_ASSERT(__cpp_lib_interpolate == 201902L);
#endif
#else
#ifdef __cpp_lib_interpolate
#error __cpp_lib_interpolate is defined
#endif
#endif

#ifndef __cpp_lib_invoke
#error __cpp_lib_invoke is not defined
#elif __cpp_lib_invoke != 201411L
#error __cpp_lib_invoke is not 201411L
#else
STATIC_ASSERT(__cpp_lib_invoke == 201411L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_aggregate
#error __cpp_lib_is_aggregate is not defined
#elif __cpp_lib_is_aggregate != 201703L
#error __cpp_lib_is_aggregate is not 201703L
#else
STATIC_ASSERT(__cpp_lib_is_aggregate == 201703L);
#endif
#else
#ifdef __cpp_lib_is_aggregate
#error __cpp_lib_is_aggregate is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_is_constant_evaluated
#error __cpp_lib_is_constant_evaluated is not defined
#elif __cpp_lib_is_constant_evaluated != 201811L
#error __cpp_lib_is_constant_evaluated is not 201811L
#else
STATIC_ASSERT(__cpp_lib_is_constant_evaluated == 201811L);
#endif
#else
#ifdef __cpp_lib_is_constant_evaluated
#error __cpp_lib_is_constant_evaluated is defined
#endif
#endif

#ifndef __cpp_lib_is_final
#error __cpp_lib_is_final is not defined
#elif __cpp_lib_is_final != 201402L
#error __cpp_lib_is_final is not 201402L
#else
STATIC_ASSERT(__cpp_lib_is_final == 201402L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_invocable
#error __cpp_lib_is_invocable is not defined
#elif __cpp_lib_is_invocable != 201703L
#error __cpp_lib_is_invocable is not 201703L
#else
STATIC_ASSERT(__cpp_lib_is_invocable == 201703L);
#endif
#else
#ifdef __cpp_lib_is_invocable
#error __cpp_lib_is_invocable is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_is_nothrow_convertible
#error __cpp_lib_is_nothrow_convertible is not defined
#elif __cpp_lib_is_nothrow_convertible != 201806L
#error __cpp_lib_is_nothrow_convertible is not 201806L
#else
STATIC_ASSERT(__cpp_lib_is_nothrow_convertible == 201806L);
#endif
#else
#ifdef __cpp_lib_is_nothrow_convertible
#error __cpp_lib_is_nothrow_convertible is defined
#endif
#endif

#ifndef __cpp_lib_is_null_pointer
#error __cpp_lib_is_null_pointer is not defined
#elif __cpp_lib_is_null_pointer != 201309L
#error __cpp_lib_is_null_pointer is not 201309L
#else
STATIC_ASSERT(__cpp_lib_is_null_pointer == 201309L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_swappable
#error __cpp_lib_is_swappable is not defined
#elif __cpp_lib_is_swappable != 201603L
#error __cpp_lib_is_swappable is not 201603L
#else
STATIC_ASSERT(__cpp_lib_is_swappable == 201603L);
#endif
#else
#ifdef __cpp_lib_is_swappable
#error __cpp_lib_is_swappable is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_jthread
#error __cpp_lib_jthread is not defined
#elif __cpp_lib_jthread != 201911L
#error __cpp_lib_jthread is not 201911L
#else
STATIC_ASSERT(__cpp_lib_jthread == 201911L);
#endif
#else
#ifdef __cpp_lib_jthread
#error __cpp_lib_jthread is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_latch
#error __cpp_lib_latch is not defined
#elif __cpp_lib_latch != 201907L
#error __cpp_lib_latch is not 201907L
#else
STATIC_ASSERT(__cpp_lib_latch == 201907L);
#endif
#else
#ifdef __cpp_lib_latch
#error __cpp_lib_latch is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_launder
#error __cpp_lib_launder is not defined
#elif __cpp_lib_launder != 201606L
#error __cpp_lib_launder is not 201606L
#else
STATIC_ASSERT(__cpp_lib_launder == 201606L);
#endif
#else
#ifdef __cpp_lib_launder
#error __cpp_lib_launder is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_list_remove_return_type
#error __cpp_lib_list_remove_return_type is not defined
#elif __cpp_lib_list_remove_return_type != 201806L
#error __cpp_lib_list_remove_return_type is not 201806L
#else
STATIC_ASSERT(__cpp_lib_list_remove_return_type == 201806L);
#endif
#else
#ifdef __cpp_lib_list_remove_return_type
#error __cpp_lib_list_remove_return_type is defined
#endif
#endif

#ifndef __cpp_lib_logical_traits
#error __cpp_lib_logical_traits is not defined
#elif __cpp_lib_logical_traits != 201510L
#error __cpp_lib_logical_traits is not 201510L
#else
STATIC_ASSERT(__cpp_lib_logical_traits == 201510L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_make_from_tuple
#error __cpp_lib_make_from_tuple is not defined
#elif __cpp_lib_make_from_tuple != 201606L
#error __cpp_lib_make_from_tuple is not 201606L
#else
STATIC_ASSERT(__cpp_lib_make_from_tuple == 201606L);
#endif
#else
#ifdef __cpp_lib_make_from_tuple
#error __cpp_lib_make_from_tuple is defined
#endif
#endif

#ifndef __cpp_lib_make_reverse_iterator
#error __cpp_lib_make_reverse_iterator is not defined
#elif __cpp_lib_make_reverse_iterator != 201402L
#error __cpp_lib_make_reverse_iterator is not 201402L
#else
STATIC_ASSERT(__cpp_lib_make_reverse_iterator == 201402L);
#endif

#ifndef __cpp_lib_make_unique
#error __cpp_lib_make_unique is not defined
#elif __cpp_lib_make_unique != 201304L
#error __cpp_lib_make_unique is not 201304L
#else
STATIC_ASSERT(__cpp_lib_make_unique == 201304L);
#endif

#ifndef __cpp_lib_map_try_emplace
#error __cpp_lib_map_try_emplace is not defined
#elif __cpp_lib_map_try_emplace != 201411L
#error __cpp_lib_map_try_emplace is not 201411L
#else
STATIC_ASSERT(__cpp_lib_map_try_emplace == 201411L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_math_constants
#error __cpp_lib_math_constants is not defined
#elif __cpp_lib_math_constants != 201907L
#error __cpp_lib_math_constants is not 201907L
#else
STATIC_ASSERT(__cpp_lib_math_constants == 201907L);
#endif
#else
#ifdef __cpp_lib_math_constants
#error __cpp_lib_math_constants is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_math_special_functions
#error __cpp_lib_math_special_functions is not defined
#elif __cpp_lib_math_special_functions != 201603L
#error __cpp_lib_math_special_functions is not 201603L
#else
STATIC_ASSERT(__cpp_lib_math_special_functions == 201603L);
#endif
#else
#ifdef __cpp_lib_math_special_functions
#error __cpp_lib_math_special_functions is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_memory_resource
#error __cpp_lib_memory_resource is not defined
#elif __cpp_lib_memory_resource != 201603L
#error __cpp_lib_memory_resource is not 201603L
#else
STATIC_ASSERT(__cpp_lib_memory_resource == 201603L);
#endif
#else
#ifdef __cpp_lib_memory_resource
#error __cpp_lib_memory_resource is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_node_extract
#error __cpp_lib_node_extract is not defined
#elif __cpp_lib_node_extract != 201606L
#error __cpp_lib_node_extract is not 201606L
#else
STATIC_ASSERT(__cpp_lib_node_extract == 201606L);
#endif
#else
#ifdef __cpp_lib_node_extract
#error __cpp_lib_node_extract is defined
#endif
#endif

#ifndef __cpp_lib_nonmember_container_access
#error __cpp_lib_nonmember_container_access is not defined
#elif __cpp_lib_nonmember_container_access != 201411L
#error __cpp_lib_nonmember_container_access is not 201411L
#else
STATIC_ASSERT(__cpp_lib_nonmember_container_access == 201411L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_not_fn
#error __cpp_lib_not_fn is not defined
#elif __cpp_lib_not_fn != 201603L
#error __cpp_lib_not_fn is not 201603L
#else
STATIC_ASSERT(__cpp_lib_not_fn == 201603L);
#endif
#else
#ifdef __cpp_lib_not_fn
#error __cpp_lib_not_fn is defined
#endif
#endif

#ifndef __cpp_lib_null_iterators
#error __cpp_lib_null_iterators is not defined
#elif __cpp_lib_null_iterators != 201304L
#error __cpp_lib_null_iterators is not 201304L
#else
STATIC_ASSERT(__cpp_lib_null_iterators == 201304L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_optional
#error __cpp_lib_optional is not defined
#elif __cpp_lib_optional != 201606L
#error __cpp_lib_optional is not 201606L
#else
STATIC_ASSERT(__cpp_lib_optional == 201606L);
#endif
#else
#ifdef __cpp_lib_optional
#error __cpp_lib_optional is defined
#endif
#endif

#if _HAS_CXX17 && !defined(_M_CEE)
#ifndef __cpp_lib_parallel_algorithm
#error __cpp_lib_parallel_algorithm is not defined
#elif __cpp_lib_parallel_algorithm != 201603L
#error __cpp_lib_parallel_algorithm is not 201603L
#else
STATIC_ASSERT(__cpp_lib_parallel_algorithm == 201603L);
#endif
#else
#ifdef __cpp_lib_parallel_algorithm
#error __cpp_lib_parallel_algorithm is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_polymorphic_allocator
#error __cpp_lib_polymorphic_allocator is not defined
#elif __cpp_lib_polymorphic_allocator != 201902L
#error __cpp_lib_polymorphic_allocator is not 201902L
#else
STATIC_ASSERT(__cpp_lib_polymorphic_allocator == 201902L);
#endif
#else
#ifdef __cpp_lib_polymorphic_allocator
#error __cpp_lib_polymorphic_allocator is defined
#endif
#endif

#ifndef __cpp_lib_quoted_string_io
#error __cpp_lib_quoted_string_io is not defined
#elif __cpp_lib_quoted_string_io != 201304L
#error __cpp_lib_quoted_string_io is not 201304L
#else
STATIC_ASSERT(__cpp_lib_quoted_string_io == 201304L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_raw_memory_algorithms
#error __cpp_lib_raw_memory_algorithms is not defined
#elif __cpp_lib_raw_memory_algorithms != 201606L
#error __cpp_lib_raw_memory_algorithms is not 201606L
#else
STATIC_ASSERT(__cpp_lib_raw_memory_algorithms == 201606L);
#endif
#else
#ifdef __cpp_lib_raw_memory_algorithms
#error __cpp_lib_raw_memory_algorithms is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_remove_cvref
#error __cpp_lib_remove_cvref is not defined
#elif __cpp_lib_remove_cvref != 201711L
#error __cpp_lib_remove_cvref is not 201711L
#else
STATIC_ASSERT(__cpp_lib_remove_cvref == 201711L);
#endif
#else
#ifdef __cpp_lib_remove_cvref
#error __cpp_lib_remove_cvref is defined
#endif
#endif

#ifndef __cpp_lib_result_of_sfinae
#error __cpp_lib_result_of_sfinae is not defined
#elif __cpp_lib_result_of_sfinae != 201210L
#error __cpp_lib_result_of_sfinae is not 201210L
#else
STATIC_ASSERT(__cpp_lib_result_of_sfinae == 201210L);
#endif

#ifndef __cpp_lib_robust_nonmodifying_seq_ops
#error __cpp_lib_robust_nonmodifying_seq_ops is not defined
#elif __cpp_lib_robust_nonmodifying_seq_ops != 201304L
#error __cpp_lib_robust_nonmodifying_seq_ops is not 201304L
#else
STATIC_ASSERT(__cpp_lib_robust_nonmodifying_seq_ops == 201304L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_sample
#error __cpp_lib_sample is not defined
#elif __cpp_lib_sample != 201603L
#error __cpp_lib_sample is not 201603L
#else
STATIC_ASSERT(__cpp_lib_sample == 201603L);
#endif
#else
#ifdef __cpp_lib_sample
#error __cpp_lib_sample is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_scoped_lock
#error __cpp_lib_scoped_lock is not defined
#elif __cpp_lib_scoped_lock != 201703L
#error __cpp_lib_scoped_lock is not 201703L
#else
STATIC_ASSERT(__cpp_lib_scoped_lock == 201703L);
#endif
#else
#ifdef __cpp_lib_scoped_lock
#error __cpp_lib_scoped_lock is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_semaphore
#error __cpp_lib_semaphore is not defined
#elif __cpp_lib_semaphore != 201907L
#error __cpp_lib_semaphore is not 201907L
#else
STATIC_ASSERT(__cpp_lib_semaphore == 201907L);
#endif
#else
#ifdef __cpp_lib_semaphore
#error __cpp_lib_semaphore is defined
#endif
#endif

#ifndef __cpp_lib_shared_mutex
#error __cpp_lib_shared_mutex is not defined
#elif __cpp_lib_shared_mutex != 201505L
#error __cpp_lib_shared_mutex is not 201505L
#else
STATIC_ASSERT(__cpp_lib_shared_mutex == 201505L);
#endif

#ifndef __cpp_lib_shared_ptr_arrays
#error __cpp_lib_shared_ptr_arrays is not defined
#elif _HAS_CXX20
#if __cpp_lib_shared_ptr_arrays != 201707L
#error __cpp_lib_shared_ptr_arrays is not 201707L
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_arrays == 201707L);
#endif
#else
#if __cpp_lib_shared_ptr_arrays != 201611L
#error __cpp_lib_shared_ptr_arrays is not 201611L
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_arrays == 201611L);
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_shared_ptr_weak_type
#error __cpp_lib_shared_ptr_weak_type is not defined
#elif __cpp_lib_shared_ptr_weak_type != 201606L
#error __cpp_lib_shared_ptr_weak_type is not 201606L
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_weak_type == 201606L);
#endif
#else
#ifdef __cpp_lib_shared_ptr_weak_type
#error __cpp_lib_shared_ptr_weak_type is defined
#endif
#endif

#ifdef _M_CEE
#ifdef __cpp_lib_shared_timed_mutex
#error __cpp_lib_shared_timed_mutex is defined
#endif
#else
#ifndef __cpp_lib_shared_timed_mutex
#error __cpp_lib_shared_timed_mutex is not defined
#elif __cpp_lib_shared_timed_mutex != 201402L
#error __cpp_lib_shared_timed_mutex is not 201402L
#else
STATIC_ASSERT(__cpp_lib_shared_timed_mutex == 201402L);
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_shift
#error __cpp_lib_shift is not defined
#elif __cpp_lib_shift != 201806L
#error __cpp_lib_shift is not 201806L
#else
STATIC_ASSERT(__cpp_lib_shift == 201806L);
#endif
#else
#ifdef __cpp_lib_shift
#error __cpp_lib_shift is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_span
#error __cpp_lib_span is not defined
#elif __cpp_lib_span != 202002L
#error __cpp_lib_span is not 202002L
#else
STATIC_ASSERT(__cpp_lib_span == 202002L);
#endif
#else
#ifdef __cpp_lib_span
#error __cpp_lib_span is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_ssize
#error __cpp_lib_ssize is not defined
#elif __cpp_lib_ssize != 201902L
#error __cpp_lib_ssize is not 201902L
#else
STATIC_ASSERT(__cpp_lib_ssize == 201902L);
#endif
#else
#ifdef __cpp_lib_ssize
#error __cpp_lib_ssize is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_starts_ends_with
#error __cpp_lib_starts_ends_with is not defined
#elif __cpp_lib_starts_ends_with != 201711L
#error __cpp_lib_starts_ends_with is not 201711L
#else
STATIC_ASSERT(__cpp_lib_starts_ends_with == 201711L);
#endif
#else
#ifdef __cpp_lib_starts_ends_with
#error __cpp_lib_starts_ends_with is defined
#endif
#endif

#ifndef __cpp_lib_string_udls
#error __cpp_lib_string_udls is not defined
#elif __cpp_lib_string_udls != 201304L
#error __cpp_lib_string_udls is not 201304L
#else
STATIC_ASSERT(__cpp_lib_string_udls == 201304L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_string_view
#error __cpp_lib_string_view is not defined
#elif __cpp_lib_string_view != 201803L
#error __cpp_lib_string_view is not 201803L
#else
STATIC_ASSERT(__cpp_lib_string_view == 201803L);
#endif
#else
#ifdef __cpp_lib_string_view
#error __cpp_lib_string_view is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_to_address
#error __cpp_lib_to_address is not defined
#elif __cpp_lib_to_address != 201711L
#error __cpp_lib_to_address is not 201711L
#else
STATIC_ASSERT(__cpp_lib_to_address == 201711L);
#endif
#else
#ifdef __cpp_lib_to_address
#error __cpp_lib_to_address is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_to_array
#error __cpp_lib_to_array is not defined
#elif __cpp_lib_to_array != 201907L
#error __cpp_lib_to_array is not 201907L
#else
STATIC_ASSERT(__cpp_lib_to_array == 201907L);
#endif
#else
#ifdef __cpp_lib_to_array
#error __cpp_lib_to_array is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_to_chars
#error __cpp_lib_to_chars is not defined
#elif __cpp_lib_to_chars != 201611L
#error __cpp_lib_to_chars is not 201611L
#else
STATIC_ASSERT(__cpp_lib_to_chars == 201611L);
#endif
#else
#ifdef __cpp_lib_to_chars
#error __cpp_lib_to_chars is defined
#endif
#endif

#ifndef __cpp_lib_transformation_trait_aliases
#error __cpp_lib_transformation_trait_aliases is not defined
#elif __cpp_lib_transformation_trait_aliases != 201304L
#error __cpp_lib_transformation_trait_aliases is not 201304L
#else
STATIC_ASSERT(__cpp_lib_transformation_trait_aliases == 201304L);
#endif

#ifndef __cpp_lib_transparent_operators
#error __cpp_lib_transparent_operators is not defined
#elif __cpp_lib_transparent_operators != 201510L
#error __cpp_lib_transparent_operators is not 201510L
#else
STATIC_ASSERT(__cpp_lib_transparent_operators == 201510L);
#endif

#ifndef __cpp_lib_tuple_element_t
#error __cpp_lib_tuple_element_t is not defined
#elif __cpp_lib_tuple_element_t != 201402L
#error __cpp_lib_tuple_element_t is not 201402L
#else
STATIC_ASSERT(__cpp_lib_tuple_element_t == 201402L);
#endif

#ifndef __cpp_lib_tuples_by_type
#error __cpp_lib_tuples_by_type is not defined
#elif __cpp_lib_tuples_by_type != 201304L
#error __cpp_lib_tuples_by_type is not 201304L
#else
STATIC_ASSERT(__cpp_lib_tuples_by_type == 201304L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_type_identity
#error __cpp_lib_type_identity is not defined
#elif __cpp_lib_type_identity != 201806L
#error __cpp_lib_type_identity is not 201806L
#else
STATIC_ASSERT(__cpp_lib_type_identity == 201806L);
#endif
#else
#ifdef __cpp_lib_type_identity
#error __cpp_lib_type_identity is defined
#endif
#endif

#ifndef __cpp_lib_type_trait_variable_templates
#error __cpp_lib_type_trait_variable_templates is not defined
#elif __cpp_lib_type_trait_variable_templates != 201510L
#error __cpp_lib_type_trait_variable_templates is not 201510L
#else
STATIC_ASSERT(__cpp_lib_type_trait_variable_templates == 201510L);
#endif

#ifndef __cpp_lib_uncaught_exceptions
#error __cpp_lib_uncaught_exceptions is not defined
#elif __cpp_lib_uncaught_exceptions != 201411L
#error __cpp_lib_uncaught_exceptions is not 201411L
#else
STATIC_ASSERT(__cpp_lib_uncaught_exceptions == 201411L);
#endif

#ifndef __cpp_lib_unordered_map_try_emplace
#error __cpp_lib_unordered_map_try_emplace is not defined
#elif __cpp_lib_unordered_map_try_emplace != 201411L
#error __cpp_lib_unordered_map_try_emplace is not 201411L
#else
STATIC_ASSERT(__cpp_lib_unordered_map_try_emplace == 201411L);
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_unwrap_ref
#error __cpp_lib_unwrap_ref is not defined
#elif __cpp_lib_unwrap_ref != 201811L
#error __cpp_lib_unwrap_ref is not 201811L
#else
STATIC_ASSERT(__cpp_lib_unwrap_ref == 201811L);
#endif
#else
#ifdef __cpp_lib_unwrap_ref
#error __cpp_lib_unwrap_ref is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_variant
#error __cpp_lib_variant is not defined
#elif __cpp_lib_variant != 201606L
#error __cpp_lib_variant is not 201606L
#else
STATIC_ASSERT(__cpp_lib_variant == 201606L);
#endif
#else
#ifdef __cpp_lib_variant
#error __cpp_lib_variant is defined
#endif
#endif

#ifndef __cpp_lib_void_t
#error __cpp_lib_void_t is not defined
#elif __cpp_lib_void_t != 201411L
#error __cpp_lib_void_t is not 201411L
#else
STATIC_ASSERT(__cpp_lib_void_t == 201411L);
#endif
