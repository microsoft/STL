// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

// LIBRARY FEATURE-TEST MACROS
#if _HAS_CXX23
#ifdef __cpp_lib_adaptor_iterator_pair_constructor
STATIC_ASSERT(__cpp_lib_adaptor_iterator_pair_constructor == 202106L);
#else
#error __cpp_lib_adaptor_iterator_pair_constructor is not defined
#endif
#else
#ifdef __cpp_lib_adaptor_iterator_pair_constructor
#error __cpp_lib_adaptor_iterator_pair_constructor is defined
#endif
#endif

#ifdef __cpp_lib_addressof_constexpr
STATIC_ASSERT(__cpp_lib_addressof_constexpr == 201603L);
#else
#error __cpp_lib_addressof_constexpr is not defined
#endif

#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_algorithm_iterator_requirements
STATIC_ASSERT(__cpp_lib_algorithm_iterator_requirements == 202207L);
#else
#error __cpp_lib_algorithm_iterator_requirements is not defined
#endif
#else
#ifdef __cpp_lib_algorithm_iterator_requirements
#error __cpp_lib_algorithm_iterator_requirements is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_allocate_at_least
STATIC_ASSERT(__cpp_lib_allocate_at_least == 202302L);
#else
#error __cpp_lib_allocate_at_least is not defined
#endif
#else
#ifdef __cpp_lib_allocate_at_least
#error __cpp_lib_allocate_at_least is defined
#endif
#endif

#ifdef __cpp_lib_allocator_traits_is_always_equal
STATIC_ASSERT(__cpp_lib_allocator_traits_is_always_equal == 201411L);
#else
#error __cpp_lib_allocator_traits_is_always_equal is not defined
#endif

#if _HAS_CXX17 && _HAS_STATIC_RTTI
#ifdef __cpp_lib_any
STATIC_ASSERT(__cpp_lib_any == 201606L);
#else
#error __cpp_lib_any is not defined
#endif
#else
#ifdef __cpp_lib_any
#error __cpp_lib_any is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_apply
STATIC_ASSERT(__cpp_lib_apply == 201603L);
#else
#error __cpp_lib_apply is not defined
#endif
#else
#ifdef __cpp_lib_apply
#error __cpp_lib_apply is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_array_constexpr
STATIC_ASSERT(__cpp_lib_array_constexpr == 201811L);
#else
#error __cpp_lib_array_constexpr is not defined
#endif
#elif _HAS_CXX17
#ifdef __cpp_lib_array_constexpr
STATIC_ASSERT(__cpp_lib_array_constexpr == 201803L);
#else
#error __cpp_lib_array_constexpr is not defined
#endif
#else
#ifdef __cpp_lib_array_constexpr
#error __cpp_lib_array_constexpr is defined
#endif
#endif

#ifdef __cpp_lib_as_const
STATIC_ASSERT(__cpp_lib_as_const == 201510L);
#else
#error __cpp_lib_as_const is not defined
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_associative_heterogeneous_erasure
STATIC_ASSERT(__cpp_lib_associative_heterogeneous_erasure == 202110L);
#else
#error __cpp_lib_associative_heterogeneous_erasure is not defined
#endif
#else
#ifdef __cpp_lib_associative_heterogeneous_erasure
#error __cpp_lib_associative_heterogeneous_erasure is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_assume_aligned
STATIC_ASSERT(__cpp_lib_assume_aligned == 201811L);
#else
#error __cpp_lib_assume_aligned is not defined
#endif
#else
#ifdef __cpp_lib_assume_aligned
#error __cpp_lib_assume_aligned is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_atomic_flag_test
STATIC_ASSERT(__cpp_lib_atomic_flag_test == 201907L);
#else
#error __cpp_lib_atomic_flag_test is not defined
#endif
#else
#ifdef __cpp_lib_atomic_flag_test
#error __cpp_lib_atomic_flag_test is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_atomic_float
STATIC_ASSERT(__cpp_lib_atomic_float == 201711L);
#else
#error __cpp_lib_atomic_float is not defined
#endif
#else
#ifdef __cpp_lib_atomic_float
#error __cpp_lib_atomic_float is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_atomic_is_always_lock_free
STATIC_ASSERT(__cpp_lib_atomic_is_always_lock_free == 201603L);
#else
#error __cpp_lib_atomic_is_always_lock_free is not defined
#endif
#else
#ifdef __cpp_lib_atomic_is_always_lock_free
#error __cpp_lib_atomic_is_always_lock_free is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_atomic_lock_free_type_aliases
STATIC_ASSERT(__cpp_lib_atomic_lock_free_type_aliases == 201907L);
#else
#error __cpp_lib_atomic_lock_free_type_aliases is not defined
#endif
#else
#ifdef __cpp_lib_atomic_lock_free_type_aliases
#error __cpp_lib_atomic_lock_free_type_aliases is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_atomic_ref
STATIC_ASSERT(__cpp_lib_atomic_ref == 201806L);
#else
#error __cpp_lib_atomic_ref is not defined
#endif
#else
#ifdef __cpp_lib_atomic_ref
#error __cpp_lib_atomic_ref is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_atomic_shared_ptr
STATIC_ASSERT(__cpp_lib_atomic_shared_ptr == 201711L);
#else
#error __cpp_lib_atomic_shared_ptr is not defined
#endif
#else
#ifdef __cpp_lib_atomic_shared_ptr
#error __cpp_lib_atomic_shared_ptr is defined
#endif
#endif

#ifdef __cpp_lib_atomic_value_initialization
STATIC_ASSERT(__cpp_lib_atomic_value_initialization == 201911L);
#else
#error __cpp_lib_atomic_value_initialization is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_atomic_wait
STATIC_ASSERT(__cpp_lib_atomic_wait == 201907L);
#else
#error __cpp_lib_atomic_wait is not defined
#endif
#else
#ifdef __cpp_lib_atomic_wait
#error __cpp_lib_atomic_wait is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_barrier
STATIC_ASSERT(__cpp_lib_barrier == 202302L);
#else
#error __cpp_lib_barrier is not defined
#endif
#else
#ifdef __cpp_lib_barrier
#error __cpp_lib_barrier is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_bind_back
STATIC_ASSERT(__cpp_lib_bind_back == 202202L);
#else
#error __cpp_lib_bind_back is not defined
#endif
#else
#ifdef __cpp_lib_bind_back
#error __cpp_lib_bind_back is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_bind_front
STATIC_ASSERT(__cpp_lib_bind_front == 201907L);
#else
#error __cpp_lib_bind_front is not defined
#endif
#else
#ifdef __cpp_lib_bind_front
#error __cpp_lib_bind_front is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_bit_cast
STATIC_ASSERT(__cpp_lib_bit_cast == 201806L);
#else
#error __cpp_lib_bit_cast is not defined
#endif
#else
#ifdef __cpp_lib_bit_cast
#error __cpp_lib_bit_cast is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_bitops
STATIC_ASSERT(__cpp_lib_bitops == 201907L);
#else
#error __cpp_lib_bitops is not defined
#endif
#else
#ifdef __cpp_lib_bitops
#error __cpp_lib_bitops is defined
#endif
#endif

#ifdef __cpp_lib_bool_constant
STATIC_ASSERT(__cpp_lib_bool_constant == 201505L);
#else
#error __cpp_lib_bool_constant is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_bounded_array_traits
STATIC_ASSERT(__cpp_lib_bounded_array_traits == 201902L);
#else
#error __cpp_lib_bounded_array_traits is not defined
#endif
#else
#ifdef __cpp_lib_bounded_array_traits
#error __cpp_lib_bounded_array_traits is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_boyer_moore_searcher
STATIC_ASSERT(__cpp_lib_boyer_moore_searcher == 201603L);
#else
#error __cpp_lib_boyer_moore_searcher is not defined
#endif
#else
#ifdef __cpp_lib_boyer_moore_searcher
#error __cpp_lib_boyer_moore_searcher is defined
#endif
#endif

#if _HAS_STD_BYTE
#ifdef __cpp_lib_byte
STATIC_ASSERT(__cpp_lib_byte == 201603L);
#else
#error __cpp_lib_byte is not defined
#endif
#else
#ifdef __cpp_lib_byte
#error __cpp_lib_byte is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_byteswap
STATIC_ASSERT(__cpp_lib_byteswap == 202110L);
#else
#error __cpp_lib_byteswap is not defined
#endif
#else
#ifdef __cpp_lib_byteswap
#error __cpp_lib_byteswap is defined
#endif
#endif

#if defined(__cpp_char8_t)
#ifdef __cpp_lib_char8_t
STATIC_ASSERT(__cpp_lib_char8_t == 201907L);
#else
#error __cpp_lib_char8_t is not defined
#endif
#else
#ifdef __cpp_lib_char8_t
#error __cpp_lib_char8_t is defined
#endif
#endif

#ifndef __cpp_lib_chrono
#error __cpp_lib_chrono is not defined
#elif defined(__cpp_lib_concepts)
STATIC_ASSERT(__cpp_lib_chrono == 201907L);
#elif _HAS_CXX17
STATIC_ASSERT(__cpp_lib_chrono == 201611L);
#else
STATIC_ASSERT(__cpp_lib_chrono == 201510L);
#endif

#ifdef __cpp_lib_chrono_udls
STATIC_ASSERT(__cpp_lib_chrono_udls == 201304L);
#else
#error __cpp_lib_chrono_udls is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_clamp
STATIC_ASSERT(__cpp_lib_clamp == 201603L);
#else
#error __cpp_lib_clamp is not defined
#endif
#else
#ifdef __cpp_lib_clamp
#error __cpp_lib_clamp is defined
#endif
#endif

#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_common_reference
STATIC_ASSERT(__cpp_lib_common_reference == 202302L);
#else
#error __cpp_lib_common_reference is not defined
#endif
#else
#ifdef __cpp_lib_common_reference
#error __cpp_lib_common_reference is defined
#endif
#endif

#if _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_common_reference_wrapper
STATIC_ASSERT(__cpp_lib_common_reference_wrapper == 202302L);
#else
#error __cpp_lib_common_reference_wrapper is not defined
#endif
#else
#ifdef __cpp_lib_common_reference_wrapper
#error __cpp_lib_common_reference_wrapper is defined
#endif
#endif

#ifdef __cpp_lib_complex_udls
STATIC_ASSERT(__cpp_lib_complex_udls == 201309L);
#else
#error __cpp_lib_complex_udls is not defined
#endif

#if _HAS_CXX23 && !defined(__EDG__) // TRANSITION, GH-395
#ifdef __cpp_lib_concepts
STATIC_ASSERT(__cpp_lib_concepts == 202207L);
#else
#error __cpp_lib_concepts is not defined
#endif
#elif _HAS_CXX20 && !defined(__EDG__) // TRANSITION, GH-395
#ifdef __cpp_lib_concepts
STATIC_ASSERT(__cpp_lib_concepts == 202002L);
#else
#error __cpp_lib_concepts is not defined
#endif
#else
#ifdef __cpp_lib_concepts
#error __cpp_lib_concepts is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_algorithms
STATIC_ASSERT(__cpp_lib_constexpr_algorithms == 201806L);
#else
#error __cpp_lib_constexpr_algorithms is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_algorithms
#error __cpp_lib_constexpr_algorithms is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_constexpr_bitset
STATIC_ASSERT(__cpp_lib_constexpr_bitset == 202207L);
#else
#error __cpp_lib_constexpr_bitset is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_bitset
#error __cpp_lib_constexpr_bitset is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_constexpr_charconv
STATIC_ASSERT(__cpp_lib_constexpr_charconv == 202207L);
#else
#error __cpp_lib_constexpr_charconv is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_charconv
#error __cpp_lib_constexpr_charconv is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_complex
STATIC_ASSERT(__cpp_lib_constexpr_complex == 201711L);
#else
#error __cpp_lib_constexpr_complex is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_complex
#error __cpp_lib_constexpr_complex is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_dynamic_alloc
STATIC_ASSERT(__cpp_lib_constexpr_dynamic_alloc == 201907L);
#else
#error __cpp_lib_constexpr_dynamic_alloc is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_dynamic_alloc
#error __cpp_lib_constexpr_dynamic_alloc is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_functional
STATIC_ASSERT(__cpp_lib_constexpr_functional == 201907L);
#else
#error __cpp_lib_constexpr_functional is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_functional
#error __cpp_lib_constexpr_functional is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_iterator
STATIC_ASSERT(__cpp_lib_constexpr_iterator == 201811L);
#else
#error __cpp_lib_constexpr_iterator is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_iterator
#error __cpp_lib_constexpr_iterator is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_constexpr_memory
STATIC_ASSERT(__cpp_lib_constexpr_memory == 202202L);
#else
#error __cpp_lib_constexpr_memory is not defined
#endif
#elif _HAS_CXX20
#ifdef __cpp_lib_constexpr_memory
STATIC_ASSERT(__cpp_lib_constexpr_memory == 201811L);
#else
#error __cpp_lib_constexpr_memory is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_memory
#error __cpp_lib_constexpr_memory is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_numeric
STATIC_ASSERT(__cpp_lib_constexpr_numeric == 201911L);
#else
#error __cpp_lib_constexpr_numeric is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_numeric
#error __cpp_lib_constexpr_numeric is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_string
STATIC_ASSERT(__cpp_lib_constexpr_string == 201907L);
#else
#error __cpp_lib_constexpr_string is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_string
#error __cpp_lib_constexpr_string is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_string_view
STATIC_ASSERT(__cpp_lib_constexpr_string_view == 201811L);
#else
#error __cpp_lib_constexpr_string_view is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_string_view
#error __cpp_lib_constexpr_string_view is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_tuple
STATIC_ASSERT(__cpp_lib_constexpr_tuple == 201811L);
#else
#error __cpp_lib_constexpr_tuple is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_tuple
#error __cpp_lib_constexpr_tuple is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_constexpr_typeinfo
STATIC_ASSERT(__cpp_lib_constexpr_typeinfo == 202106L);
#else
#error __cpp_lib_constexpr_typeinfo is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_typeinfo
#error __cpp_lib_constexpr_typeinfo is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_utility
STATIC_ASSERT(__cpp_lib_constexpr_utility == 201811L);
#else
#error __cpp_lib_constexpr_utility is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_utility
#error __cpp_lib_constexpr_utility is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_constexpr_vector
STATIC_ASSERT(__cpp_lib_constexpr_vector == 201907L);
#else
#error __cpp_lib_constexpr_vector is not defined
#endif
#else
#ifdef __cpp_lib_constexpr_vector
#error __cpp_lib_constexpr_vector is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_containers_ranges
STATIC_ASSERT(__cpp_lib_containers_ranges == 202202L);
#else
#error __cpp_lib_containers_ranges is not defined
#endif
#else
#ifdef __cpp_lib_containers_ranges
#error __cpp_lib_containers_ranges is defined
#endif
#endif

#ifdef __cpp_impl_coroutine
#ifdef __cpp_lib_coroutine
STATIC_ASSERT(__cpp_lib_coroutine == 201902L);
#else
#error __cpp_lib_coroutine is not defined
#endif
#else
#ifdef __cpp_lib_coroutine
#error __cpp_lib_coroutine is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_destroying_delete
STATIC_ASSERT(__cpp_lib_destroying_delete == 201806L);
#else
#error __cpp_lib_destroying_delete is not defined
#endif
#else
#ifdef __cpp_lib_destroying_delete
#error __cpp_lib_destroying_delete is defined
#endif
#endif

#ifdef __cpp_lib_enable_shared_from_this
STATIC_ASSERT(__cpp_lib_enable_shared_from_this == 201603L);
#else
#error __cpp_lib_enable_shared_from_this is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_endian
STATIC_ASSERT(__cpp_lib_endian == 201907L);
#else
#error __cpp_lib_endian is not defined
#endif
#else
#ifdef __cpp_lib_endian
#error __cpp_lib_endian is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_erase_if
STATIC_ASSERT(__cpp_lib_erase_if == 202002L);
#else
#error __cpp_lib_erase_if is not defined
#endif
#else
#ifdef __cpp_lib_erase_if
#error __cpp_lib_erase_if is defined
#endif
#endif

#ifdef __cpp_lib_exchange_function
STATIC_ASSERT(__cpp_lib_exchange_function == 201304L);
#else
#error __cpp_lib_exchange_function is not defined
#endif

#if _HAS_CXX20 && !defined(_M_CEE_PURE)
#ifdef __cpp_lib_execution
STATIC_ASSERT(__cpp_lib_execution == 201902L);
#else
#error __cpp_lib_execution is not defined
#endif
#elif _HAS_CXX17 && !defined(_M_CEE_PURE)
#ifdef __cpp_lib_execution
STATIC_ASSERT(__cpp_lib_execution == 201603L);
#else
#error __cpp_lib_execution is not defined
#endif
#else
#ifdef __cpp_lib_execution
#error __cpp_lib_execution is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_expected
STATIC_ASSERT(__cpp_lib_expected == 202211L);
#else
#error __cpp_lib_expected is not defined
#endif
#else
#ifdef __cpp_lib_expected
#error __cpp_lib_expected is defined
#endif
#endif

#ifdef __cpp_lib_experimental_erase_if
STATIC_ASSERT(__cpp_lib_experimental_erase_if == 201411L);
#else
#error __cpp_lib_experimental_erase_if is not defined
#endif

#ifdef __cpp_lib_experimental_filesystem
STATIC_ASSERT(__cpp_lib_experimental_filesystem == 201406L);
#else
#error __cpp_lib_experimental_filesystem is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_filesystem
STATIC_ASSERT(__cpp_lib_filesystem == 201703L);
#else
#error __cpp_lib_filesystem is not defined
#endif
#else
#ifdef __cpp_lib_filesystem
#error __cpp_lib_filesystem is defined
#endif
#endif

#ifdef __cpp_lib_concepts
#ifdef __cpp_lib_format
STATIC_ASSERT(__cpp_lib_format == 202207L);
#else
#error __cpp_lib_format is not defined
#endif
#else
#ifdef __cpp_lib_format
#error __cpp_lib_format is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_formatters
STATIC_ASSERT(__cpp_lib_formatters == 202302L);
#else
#error __cpp_lib_formatters is not defined
#endif
#else
#ifdef __cpp_lib_formatters
#error __cpp_lib_formatters is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_forward_like
STATIC_ASSERT(__cpp_lib_forward_like == 202207L);
#else
#error __cpp_lib_forward_like is not defined
#endif
#else
#ifdef __cpp_lib_forward_like
#error __cpp_lib_forward_like is defined
#endif
#endif

#ifdef __cpp_lib_freestanding_char_traits
STATIC_ASSERT(__cpp_lib_freestanding_char_traits == 202306L);
#else
#error __cpp_lib_freestanding_char_traits is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_freestanding_charconv
STATIC_ASSERT(__cpp_lib_freestanding_charconv == 202306L);
#else
#error __cpp_lib_freestanding_charconv is not defined
#endif
#else
#ifdef __cpp_lib_freestanding_charconv
#error __cpp_lib_freestanding_charconv is defined
#endif
#endif

#ifdef __cpp_lib_freestanding_cstdlib
STATIC_ASSERT(__cpp_lib_freestanding_cstdlib == 202306L);
#else
#error __cpp_lib_freestanding_cstdlib is not defined
#endif

#ifdef __cpp_lib_freestanding_cstring
STATIC_ASSERT(__cpp_lib_freestanding_cstring == 202306L);
#else
#error __cpp_lib_freestanding_cstring is not defined
#endif

#ifdef __cpp_lib_freestanding_cwchar
STATIC_ASSERT(__cpp_lib_freestanding_cwchar == 202306L);
#else
#error __cpp_lib_freestanding_cwchar is not defined
#endif

#ifdef __cpp_lib_freestanding_errc
STATIC_ASSERT(__cpp_lib_freestanding_errc == 202306L);
#else
#error __cpp_lib_freestanding_errc is not defined
#endif

#ifdef __cpp_lib_freestanding_feature_test_macros
STATIC_ASSERT(__cpp_lib_freestanding_feature_test_macros == 202306L);
#else
#error __cpp_lib_freestanding_feature_test_macros is not defined
#endif

#ifdef __cpp_lib_freestanding_functional
STATIC_ASSERT(__cpp_lib_freestanding_functional == 202306L);
#else
#error __cpp_lib_freestanding_functional is not defined
#endif

#ifdef __cpp_lib_freestanding_iterator
STATIC_ASSERT(__cpp_lib_freestanding_iterator == 202306L);
#else
#error __cpp_lib_freestanding_iterator is not defined
#endif

#ifdef __cpp_lib_freestanding_memory
STATIC_ASSERT(__cpp_lib_freestanding_memory == 202306L);
#else
#error __cpp_lib_freestanding_memory is not defined
#endif

#ifdef __cpp_lib_freestanding_operator_new
STATIC_ASSERT(__cpp_lib_freestanding_operator_new == 202306L);
#else
#error __cpp_lib_freestanding_operator_new is not defined
#endif

#ifdef __cpp_lib_concepts
#ifdef __cpp_lib_freestanding_ranges
STATIC_ASSERT(__cpp_lib_freestanding_ranges == 202306L);
#else
#error __cpp_lib_freestanding_ranges is not defined
#endif
#else
#ifdef __cpp_lib_freestanding_ranges
#error __cpp_lib_freestanding_ranges is defined
#endif
#endif

#ifdef __cpp_lib_freestanding_ratio
STATIC_ASSERT(__cpp_lib_freestanding_ratio == 202306L);
#else
#error __cpp_lib_freestanding_ratio is not defined
#endif

#ifdef __cpp_lib_freestanding_tuple
STATIC_ASSERT(__cpp_lib_freestanding_tuple == 202306L);
#else
#error __cpp_lib_freestanding_tuple is not defined
#endif

#ifdef __cpp_lib_freestanding_utility
STATIC_ASSERT(__cpp_lib_freestanding_utility == 202306L);
#else
#error __cpp_lib_freestanding_utility is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_gcd_lcm
STATIC_ASSERT(__cpp_lib_gcd_lcm == 201606L);
#else
#error __cpp_lib_gcd_lcm is not defined
#endif
#else
#ifdef __cpp_lib_gcd_lcm
#error __cpp_lib_gcd_lcm is defined
#endif
#endif

#ifdef __cpp_lib_generic_associative_lookup
STATIC_ASSERT(__cpp_lib_generic_associative_lookup == 201304L);
#else
#error __cpp_lib_generic_associative_lookup is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_generic_unordered_lookup
STATIC_ASSERT(__cpp_lib_generic_unordered_lookup == 201811L);
#else
#error __cpp_lib_generic_unordered_lookup is not defined
#endif
#else
#ifdef __cpp_lib_generic_unordered_lookup
#error __cpp_lib_generic_unordered_lookup is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_hardware_interference_size
STATIC_ASSERT(__cpp_lib_hardware_interference_size == 201703L);
#else
#error __cpp_lib_hardware_interference_size is not defined
#endif
#else
#ifdef __cpp_lib_hardware_interference_size
#error __cpp_lib_hardware_interference_size is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_has_unique_object_representations
STATIC_ASSERT(__cpp_lib_has_unique_object_representations == 201606L);
#else
#error __cpp_lib_has_unique_object_representations is not defined
#endif
#else
#ifdef __cpp_lib_has_unique_object_representations
#error __cpp_lib_has_unique_object_representations is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_hypot
STATIC_ASSERT(__cpp_lib_hypot == 201603L);
#else
#error __cpp_lib_hypot is not defined
#endif
#else
#ifdef __cpp_lib_hypot
#error __cpp_lib_hypot is defined
#endif
#endif

#ifdef __cpp_lib_incomplete_container_elements
STATIC_ASSERT(__cpp_lib_incomplete_container_elements == 201505L);
#else
#error __cpp_lib_incomplete_container_elements is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_int_pow2
STATIC_ASSERT(__cpp_lib_int_pow2 == 202002L);
#else
#error __cpp_lib_int_pow2 is not defined
#endif
#else
#ifdef __cpp_lib_int_pow2
#error __cpp_lib_int_pow2 is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_integer_comparison_functions
STATIC_ASSERT(__cpp_lib_integer_comparison_functions == 202002L);
#else
#error __cpp_lib_integer_comparison_functions is not defined
#endif
#else
#ifdef __cpp_lib_integer_comparison_functions
#error __cpp_lib_integer_comparison_functions is defined
#endif
#endif

#ifdef __cpp_lib_integer_sequence
STATIC_ASSERT(__cpp_lib_integer_sequence == 201304L);
#else
#error __cpp_lib_integer_sequence is not defined
#endif

#ifdef __cpp_lib_integral_constant_callable
STATIC_ASSERT(__cpp_lib_integral_constant_callable == 201304L);
#else
#error __cpp_lib_integral_constant_callable is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_interpolate
STATIC_ASSERT(__cpp_lib_interpolate == 201902L);
#else
#error __cpp_lib_interpolate is not defined
#endif
#else
#ifdef __cpp_lib_interpolate
#error __cpp_lib_interpolate is defined
#endif
#endif

#ifdef __cpp_lib_invoke
STATIC_ASSERT(__cpp_lib_invoke == 201411L);
#else
#error __cpp_lib_invoke is not defined
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_invoke_r
STATIC_ASSERT(__cpp_lib_invoke_r == 202106L);
#else
#error __cpp_lib_invoke_r is not defined
#endif
#else
#ifdef __cpp_lib_invoke_r
#error __cpp_lib_invoke_r is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_ios_noreplace
STATIC_ASSERT(__cpp_lib_ios_noreplace == 202207L);
#else
#error __cpp_lib_ios_noreplace is not defined
#endif
#else
#ifdef __cpp_lib_ios_noreplace
#error __cpp_lib_ios_noreplace is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_is_aggregate
STATIC_ASSERT(__cpp_lib_is_aggregate == 201703L);
#else
#error __cpp_lib_is_aggregate is not defined
#endif
#else
#ifdef __cpp_lib_is_aggregate
#error __cpp_lib_is_aggregate is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_is_constant_evaluated
STATIC_ASSERT(__cpp_lib_is_constant_evaluated == 201811L);
#else
#error __cpp_lib_is_constant_evaluated is not defined
#endif
#else
#ifdef __cpp_lib_is_constant_evaluated
#error __cpp_lib_is_constant_evaluated is defined
#endif
#endif

#ifdef __cpp_lib_is_final
STATIC_ASSERT(__cpp_lib_is_final == 201402L);
#else
#error __cpp_lib_is_final is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_is_invocable
STATIC_ASSERT(__cpp_lib_is_invocable == 201703L);
#else
#error __cpp_lib_is_invocable is not defined
#endif
#else
#ifdef __cpp_lib_is_invocable
#error __cpp_lib_is_invocable is defined
#endif
#endif

#if _HAS_CXX20
#ifndef __clang__ // TRANSITION, LLVM-48860
#ifdef __cpp_lib_is_layout_compatible
STATIC_ASSERT(__cpp_lib_is_layout_compatible == 201907L);
#else
#error __cpp_lib_is_layout_compatible is not defined
#endif
#else
#ifdef __cpp_lib_is_layout_compatible
#error __cpp_lib_is_layout_compatible is defined
#endif
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_is_nothrow_convertible
STATIC_ASSERT(__cpp_lib_is_nothrow_convertible == 201806L);
#else
#error __cpp_lib_is_nothrow_convertible is not defined
#endif
#else
#ifdef __cpp_lib_is_nothrow_convertible
#error __cpp_lib_is_nothrow_convertible is defined
#endif
#endif

#ifdef __cpp_lib_is_null_pointer
STATIC_ASSERT(__cpp_lib_is_null_pointer == 201309L);
#else
#error __cpp_lib_is_null_pointer is not defined
#endif

#if _HAS_CXX20
#ifndef __clang__ // TRANSITION, LLVM-48860
#ifdef __cpp_lib_is_pointer_interconvertible
STATIC_ASSERT(__cpp_lib_is_pointer_interconvertible == 201907L);
#else
#error __cpp_lib_is_pointer_interconvertible is not defined
#endif
#else
#ifdef __cpp_lib_is_pointer_interconvertible
#error __cpp_lib_is_pointer_interconvertible is defined
#endif
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_is_scoped_enum
STATIC_ASSERT(__cpp_lib_is_scoped_enum == 202011L);
#else
#error __cpp_lib_is_scoped_enum is not defined
#endif
#else
#ifdef __cpp_lib_is_scoped_enum
#error __cpp_lib_is_scoped_enum is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_is_swappable
STATIC_ASSERT(__cpp_lib_is_swappable == 201603L);
#else
#error __cpp_lib_is_swappable is not defined
#endif
#else
#ifdef __cpp_lib_is_swappable
#error __cpp_lib_is_swappable is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_jthread
STATIC_ASSERT(__cpp_lib_jthread == 201911L);
#else
#error __cpp_lib_jthread is not defined
#endif
#else
#ifdef __cpp_lib_jthread
#error __cpp_lib_jthread is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_latch
STATIC_ASSERT(__cpp_lib_latch == 201907L);
#else
#error __cpp_lib_latch is not defined
#endif
#else
#ifdef __cpp_lib_latch
#error __cpp_lib_latch is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_launder
STATIC_ASSERT(__cpp_lib_launder == 201606L);
#else
#error __cpp_lib_launder is not defined
#endif
#else
#ifdef __cpp_lib_launder
#error __cpp_lib_launder is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_list_remove_return_type
STATIC_ASSERT(__cpp_lib_list_remove_return_type == 201806L);
#else
#error __cpp_lib_list_remove_return_type is not defined
#endif
#else
#ifdef __cpp_lib_list_remove_return_type
#error __cpp_lib_list_remove_return_type is defined
#endif
#endif

#ifdef __cpp_lib_logical_traits
STATIC_ASSERT(__cpp_lib_logical_traits == 201510L);
#else
#error __cpp_lib_logical_traits is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_make_from_tuple
STATIC_ASSERT(__cpp_lib_make_from_tuple == 201606L);
#else
#error __cpp_lib_make_from_tuple is not defined
#endif
#else
#ifdef __cpp_lib_make_from_tuple
#error __cpp_lib_make_from_tuple is defined
#endif
#endif

#ifdef __cpp_lib_make_reverse_iterator
STATIC_ASSERT(__cpp_lib_make_reverse_iterator == 201402L);
#else
#error __cpp_lib_make_reverse_iterator is not defined
#endif

#ifdef __cpp_lib_make_unique
STATIC_ASSERT(__cpp_lib_make_unique == 201304L);
#else
#error __cpp_lib_make_unique is not defined
#endif

#ifdef __cpp_lib_map_try_emplace
STATIC_ASSERT(__cpp_lib_map_try_emplace == 201411L);
#else
#error __cpp_lib_map_try_emplace is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_math_constants
STATIC_ASSERT(__cpp_lib_math_constants == 201907L);
#else
#error __cpp_lib_math_constants is not defined
#endif
#else
#ifdef __cpp_lib_math_constants
#error __cpp_lib_math_constants is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_math_special_functions
STATIC_ASSERT(__cpp_lib_math_special_functions == 201603L);
#else
#error __cpp_lib_math_special_functions is not defined
#endif
#else
#ifdef __cpp_lib_math_special_functions
#error __cpp_lib_math_special_functions is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_mdspan
STATIC_ASSERT(__cpp_lib_mdspan == 202207L);
#else
#error __cpp_lib_mdspan is not defined
#endif
#else
#ifdef __cpp_lib_mdspan
#error __cpp_lib_mdspan is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_memory_resource
STATIC_ASSERT(__cpp_lib_memory_resource == 201603L);
#else
#error __cpp_lib_memory_resource is not defined
#endif
#else
#ifdef __cpp_lib_memory_resource
#error __cpp_lib_memory_resource is defined
#endif
#endif

#if _HAS_CXX20 && !defined(__clang__) && !defined(__EDG__) // TRANSITION, Clang and EDG support for modules
#ifdef __cpp_lib_modules
STATIC_ASSERT(__cpp_lib_modules == 202207L);
#else
#error __cpp_lib_modules is not defined
#endif
#else
#ifdef __cpp_lib_modules
#error __cpp_lib_modules is defined
#endif
#endif

#ifdef __cpp_lib_concepts
#ifdef __cpp_lib_move_iterator_concept
STATIC_ASSERT(__cpp_lib_move_iterator_concept == 202207L);
#else
#error __cpp_lib_move_iterator_concept is not defined
#endif
#else
#ifdef __cpp_lib_move_iterator_concept
#error __cpp_lib_move_iterator_concept is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_move_only_function
STATIC_ASSERT(__cpp_lib_move_only_function == 202110L);
#else
#error __cpp_lib_move_only_function is not defined
#endif
#else
#ifdef __cpp_lib_move_only_function
#error __cpp_lib_move_only_function is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_node_extract
STATIC_ASSERT(__cpp_lib_node_extract == 201606L);
#else
#error __cpp_lib_node_extract is not defined
#endif
#else
#ifdef __cpp_lib_node_extract
#error __cpp_lib_node_extract is defined
#endif
#endif

#ifdef __cpp_lib_nonmember_container_access
STATIC_ASSERT(__cpp_lib_nonmember_container_access == 201411L);
#else
#error __cpp_lib_nonmember_container_access is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_not_fn
STATIC_ASSERT(__cpp_lib_not_fn == 201603L);
#else
#error __cpp_lib_not_fn is not defined
#endif
#else
#ifdef __cpp_lib_not_fn
#error __cpp_lib_not_fn is defined
#endif
#endif

#ifdef __cpp_lib_null_iterators
STATIC_ASSERT(__cpp_lib_null_iterators == 201304L);
#else
#error __cpp_lib_null_iterators is not defined
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_optional
STATIC_ASSERT(__cpp_lib_optional == 202110L);
#else
#error __cpp_lib_optional is not defined
#endif
#elif _HAS_CXX20
#ifdef __cpp_lib_optional
STATIC_ASSERT(__cpp_lib_optional == 202106L);
#else
#error __cpp_lib_optional is not defined
#endif
#elif _HAS_CXX17
#ifdef __cpp_lib_optional
STATIC_ASSERT(__cpp_lib_optional == 201606L);
#else
#error __cpp_lib_optional is not defined
#endif
#else
#ifdef __cpp_lib_optional
#error __cpp_lib_optional is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_out_ptr
STATIC_ASSERT(__cpp_lib_out_ptr == 202106L);
#else
#error __cpp_lib_out_ptr is not defined
#endif
#else
#ifdef __cpp_lib_out_ptr
#error __cpp_lib_out_ptr is defined
#endif
#endif

#if _HAS_CXX17 && !defined(_M_CEE_PURE)
#ifdef __cpp_lib_parallel_algorithm
STATIC_ASSERT(__cpp_lib_parallel_algorithm == 201603L);
#else
#error __cpp_lib_parallel_algorithm is not defined
#endif
#else
#ifdef __cpp_lib_parallel_algorithm
#error __cpp_lib_parallel_algorithm is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_polymorphic_allocator
STATIC_ASSERT(__cpp_lib_polymorphic_allocator == 201902L);
#else
#error __cpp_lib_polymorphic_allocator is not defined
#endif
#else
#ifdef __cpp_lib_polymorphic_allocator
#error __cpp_lib_polymorphic_allocator is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_print
STATIC_ASSERT(__cpp_lib_print == 202207L);
#else
#error __cpp_lib_print is not defined
#endif
#else
#ifdef __cpp_lib_print
#error __cpp_lib_print is defined
#endif
#endif

#ifdef __cpp_lib_quoted_string_io
STATIC_ASSERT(__cpp_lib_quoted_string_io == 201304L);
#else
#error __cpp_lib_quoted_string_io is not defined
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges
STATIC_ASSERT(__cpp_lib_ranges == 202302L);
#else
#error __cpp_lib_ranges is not defined
#endif
#elif _HAS_CXX20 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges
STATIC_ASSERT(__cpp_lib_ranges == 202110L);
#else
#error __cpp_lib_ranges is not defined
#endif
#else
#ifdef __cpp_lib_ranges
#error __cpp_lib_ranges is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_as_const
STATIC_ASSERT(__cpp_lib_ranges_as_const == 202207L);
#else
#error __cpp_lib_ranges_as_const is not defined
#endif
#else
#ifdef __cpp_lib_ranges_as_const
#error __cpp_lib_ranges_as_const is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_as_rvalue
STATIC_ASSERT(__cpp_lib_ranges_as_rvalue == 202207L);
#else
#error __cpp_lib_ranges_as_rvalue is not defined
#endif
#else
#ifdef __cpp_lib_ranges_as_rvalue
#error __cpp_lib_ranges_as_rvalue is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_cartesian_product
STATIC_ASSERT(__cpp_lib_ranges_cartesian_product == 202207L);
#else
#error __cpp_lib_ranges_cartesian_product is not defined
#endif
#else
#ifdef __cpp_lib_ranges_cartesian_product
#error __cpp_lib_ranges_cartesian_product is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_chunk
STATIC_ASSERT(__cpp_lib_ranges_chunk == 202202L);
#else
#error __cpp_lib_ranges_chunk is not defined
#endif
#else
#ifdef __cpp_lib_ranges_chunk
#error __cpp_lib_ranges_chunk is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_chunk_by
STATIC_ASSERT(__cpp_lib_ranges_chunk_by == 202202L);
#else
#error __cpp_lib_ranges_chunk_by is not defined
#endif
#else
#ifdef __cpp_lib_ranges_chunk_by
#error __cpp_lib_ranges_chunk_by is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_contains
STATIC_ASSERT(__cpp_lib_ranges_contains == 202207L);
#else
#error __cpp_lib_ranges_contains is not defined
#endif
#else
#ifdef __cpp_lib_ranges_contains
#error __cpp_lib_ranges_contains is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_enumerate
STATIC_ASSERT(__cpp_lib_ranges_enumerate == 202302L);
#else
#error __cpp_lib_ranges_enumerate is not defined
#endif
#else
#ifdef __cpp_lib_ranges_enumerate
#error __cpp_lib_ranges_enumerate is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_find_last
STATIC_ASSERT(__cpp_lib_ranges_find_last == 202207L);
#else
#error __cpp_lib_ranges_find_last is not defined
#endif
#else
#ifdef __cpp_lib_ranges_find_last
#error __cpp_lib_ranges_find_last is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_fold
STATIC_ASSERT(__cpp_lib_ranges_fold == 202207L);
#else
#error __cpp_lib_ranges_fold is not defined
#endif
#else
#ifdef __cpp_lib_ranges_fold
#error __cpp_lib_ranges_fold is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_iota
STATIC_ASSERT(__cpp_lib_ranges_iota == 202202L);
#else
#error __cpp_lib_ranges_iota is not defined
#endif
#else
#ifdef __cpp_lib_ranges_iota
#error __cpp_lib_ranges_iota is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_join_with
STATIC_ASSERT(__cpp_lib_ranges_join_with == 202202L);
#else
#error __cpp_lib_ranges_join_with is not defined
#endif
#else
#ifdef __cpp_lib_ranges_join_with
#error __cpp_lib_ranges_join_with is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_repeat
STATIC_ASSERT(__cpp_lib_ranges_repeat == 202207L);
#else
#error __cpp_lib_ranges_repeat is not defined
#endif
#else
#ifdef __cpp_lib_ranges_repeat
#error __cpp_lib_ranges_repeat is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_slide
STATIC_ASSERT(__cpp_lib_ranges_slide == 202202L);
#else
#error __cpp_lib_ranges_slide is not defined
#endif
#else
#ifdef __cpp_lib_ranges_slide
#error __cpp_lib_ranges_slide is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_starts_ends_with
STATIC_ASSERT(__cpp_lib_ranges_starts_ends_with == 202106L);
#else
#error __cpp_lib_ranges_starts_ends_with is not defined
#endif
#else
#ifdef __cpp_lib_ranges_starts_ends_with
#error __cpp_lib_ranges_starts_ends_with is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_stride
STATIC_ASSERT(__cpp_lib_ranges_stride == 202207L);
#else
#error __cpp_lib_ranges_stride is not defined
#endif
#else
#ifdef __cpp_lib_ranges_stride
#error __cpp_lib_ranges_stride is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_to_container
STATIC_ASSERT(__cpp_lib_ranges_to_container == 202202L);
#else
#error __cpp_lib_ranges_to_container is not defined
#endif
#else
#ifdef __cpp_lib_ranges_to_container
#error __cpp_lib_ranges_to_container is defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_ranges_zip
STATIC_ASSERT(__cpp_lib_ranges_zip == 202110L);
#else
#error __cpp_lib_ranges_zip is not defined
#endif
#else
#ifdef __cpp_lib_ranges_zip
#error __cpp_lib_ranges_zip is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_raw_memory_algorithms
STATIC_ASSERT(__cpp_lib_raw_memory_algorithms == 201606L);
#else
#error __cpp_lib_raw_memory_algorithms is not defined
#endif
#else
#ifdef __cpp_lib_raw_memory_algorithms
#error __cpp_lib_raw_memory_algorithms is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_remove_cvref
STATIC_ASSERT(__cpp_lib_remove_cvref == 201711L);
#else
#error __cpp_lib_remove_cvref is not defined
#endif
#else
#ifdef __cpp_lib_remove_cvref
#error __cpp_lib_remove_cvref is defined
#endif
#endif

#ifdef __cpp_lib_result_of_sfinae
STATIC_ASSERT(__cpp_lib_result_of_sfinae == 201210L);
#else
#error __cpp_lib_result_of_sfinae is not defined
#endif

#ifdef __cpp_lib_robust_nonmodifying_seq_ops
STATIC_ASSERT(__cpp_lib_robust_nonmodifying_seq_ops == 201304L);
#else
#error __cpp_lib_robust_nonmodifying_seq_ops is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_sample
STATIC_ASSERT(__cpp_lib_sample == 201603L);
#else
#error __cpp_lib_sample is not defined
#endif
#else
#ifdef __cpp_lib_sample
#error __cpp_lib_sample is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_scoped_lock
STATIC_ASSERT(__cpp_lib_scoped_lock == 201703L);
#else
#error __cpp_lib_scoped_lock is not defined
#endif
#else
#ifdef __cpp_lib_scoped_lock
#error __cpp_lib_scoped_lock is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_semaphore
STATIC_ASSERT(__cpp_lib_semaphore == 201907L);
#else
#error __cpp_lib_semaphore is not defined
#endif
#else
#ifdef __cpp_lib_semaphore
#error __cpp_lib_semaphore is defined
#endif
#endif

#ifdef __cpp_lib_shared_mutex
STATIC_ASSERT(__cpp_lib_shared_mutex == 201505L);
#else
#error __cpp_lib_shared_mutex is not defined
#endif

#ifndef __cpp_lib_shared_ptr_arrays
#error __cpp_lib_shared_ptr_arrays is not defined
#elif _HAS_CXX20
STATIC_ASSERT(__cpp_lib_shared_ptr_arrays == 201707L);
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_arrays == 201611L);
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_shared_ptr_weak_type
STATIC_ASSERT(__cpp_lib_shared_ptr_weak_type == 201606L);
#else
#error __cpp_lib_shared_ptr_weak_type is not defined
#endif
#else
#ifdef __cpp_lib_shared_ptr_weak_type
#error __cpp_lib_shared_ptr_weak_type is defined
#endif
#endif

#ifdef _M_CEE_PURE
#ifdef __cpp_lib_shared_timed_mutex
#error __cpp_lib_shared_timed_mutex is defined
#endif
#else
#ifdef __cpp_lib_shared_timed_mutex
STATIC_ASSERT(__cpp_lib_shared_timed_mutex == 201402L);
#else
#error __cpp_lib_shared_timed_mutex is not defined
#endif
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_shift
STATIC_ASSERT(__cpp_lib_shift == 202202L);
#else
#error __cpp_lib_shift is not defined
#endif
#elif _HAS_CXX20
#ifdef __cpp_lib_shift
STATIC_ASSERT(__cpp_lib_shift == 201806L);
#else
#error __cpp_lib_shift is not defined
#endif
#else
#ifdef __cpp_lib_shift
#error __cpp_lib_shift is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_smart_ptr_for_overwrite
STATIC_ASSERT(__cpp_lib_smart_ptr_for_overwrite == 202002L);
#else
#error __cpp_lib_smart_ptr_for_overwrite is not defined
#endif
#else
#ifdef __cpp_lib_smart_ptr_for_overwrite
#error __cpp_lib_smart_ptr_for_overwrite is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_source_location
STATIC_ASSERT(__cpp_lib_source_location == 201907L);
#else
#error __cpp_lib_source_location is not defined
#endif
#else
#ifdef __cpp_lib_source_location
#error __cpp_lib_source_location is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_span
STATIC_ASSERT(__cpp_lib_span == 202002L);
#else
#error __cpp_lib_span is not defined
#endif
#else
#ifdef __cpp_lib_span
#error __cpp_lib_span is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_spanstream
STATIC_ASSERT(__cpp_lib_spanstream == 202106L);
#else
#error __cpp_lib_spanstream is not defined
#endif
#else
#ifdef __cpp_lib_spanstream
#error __cpp_lib_spanstream is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_ssize
STATIC_ASSERT(__cpp_lib_ssize == 201902L);
#else
#error __cpp_lib_ssize is not defined
#endif
#else
#ifdef __cpp_lib_ssize
#error __cpp_lib_ssize is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_stacktrace
STATIC_ASSERT(__cpp_lib_stacktrace == 202011L);
#else
#error __cpp_lib_stacktrace is not defined
#endif
#else
#ifdef __cpp_lib_stacktrace
#error __cpp_lib_stacktrace is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_starts_ends_with
STATIC_ASSERT(__cpp_lib_starts_ends_with == 201711L);
#else
#error __cpp_lib_starts_ends_with is not defined
#endif
#else
#ifdef __cpp_lib_starts_ends_with
#error __cpp_lib_starts_ends_with is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_stdatomic_h
STATIC_ASSERT(__cpp_lib_stdatomic_h == 202011L);
#else
#error __cpp_lib_stdatomic_h is not defined
#endif
#else
#ifdef __cpp_lib_stdatomic_h
#error __cpp_lib_stdatomic_h is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_string_contains
STATIC_ASSERT(__cpp_lib_string_contains == 202011L);
#else
#error __cpp_lib_string_contains is not defined
#endif
#else
#ifdef __cpp_lib_string_contains
#error __cpp_lib_string_contains is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_string_resize_and_overwrite
STATIC_ASSERT(__cpp_lib_string_resize_and_overwrite == 202110L);
#else
#error __cpp_lib_string_resize_and_overwrite is not defined
#endif
#else
#ifdef __cpp_lib_string_resize_and_overwrite
#error __cpp_lib_string_resize_and_overwrite is defined
#endif
#endif

#ifdef __cpp_lib_string_udls
STATIC_ASSERT(__cpp_lib_string_udls == 201304L);
#else
#error __cpp_lib_string_udls is not defined
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_string_view
STATIC_ASSERT(__cpp_lib_string_view == 201803L);
#else
#error __cpp_lib_string_view is not defined
#endif
#else
#ifdef __cpp_lib_string_view
#error __cpp_lib_string_view is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_syncbuf
STATIC_ASSERT(__cpp_lib_syncbuf == 201803L);
#else
#error __cpp_lib_syncbuf is not defined
#endif
#else
#ifdef __cpp_lib_syncbuf
#error __cpp_lib_syncbuf is defined
#endif
#endif

#ifdef __cpp_lib_concepts
#ifdef __cpp_lib_three_way_comparison
STATIC_ASSERT(__cpp_lib_three_way_comparison == 201907L);
#else
#error __cpp_lib_three_way_comparison is not defined
#endif
#else
#ifdef __cpp_lib_three_way_comparison
#error __cpp_lib_three_way_comparison is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_to_address
STATIC_ASSERT(__cpp_lib_to_address == 201711L);
#else
#error __cpp_lib_to_address is not defined
#endif
#else
#ifdef __cpp_lib_to_address
#error __cpp_lib_to_address is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_to_array
STATIC_ASSERT(__cpp_lib_to_array == 201907L);
#else
#error __cpp_lib_to_array is not defined
#endif
#else
#ifdef __cpp_lib_to_array
#error __cpp_lib_to_array is defined
#endif
#endif

#if _HAS_CXX17
#ifdef __cpp_lib_to_chars
STATIC_ASSERT(__cpp_lib_to_chars == 201611L);
#else
#error __cpp_lib_to_chars is not defined
#endif
#else
#ifdef __cpp_lib_to_chars
#error __cpp_lib_to_chars is defined
#endif
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_to_underlying
STATIC_ASSERT(__cpp_lib_to_underlying == 202102L);
#else
#error __cpp_lib_to_underlying is not defined
#endif
#else
#ifdef __cpp_lib_to_underlying
#error __cpp_lib_to_underlying is defined
#endif
#endif

#ifdef __cpp_lib_transformation_trait_aliases
STATIC_ASSERT(__cpp_lib_transformation_trait_aliases == 201304L);
#else
#error __cpp_lib_transformation_trait_aliases is not defined
#endif

#ifdef __cpp_lib_transparent_operators
STATIC_ASSERT(__cpp_lib_transparent_operators == 201510L);
#else
#error __cpp_lib_transparent_operators is not defined
#endif

#ifdef __cpp_lib_tuple_element_t
STATIC_ASSERT(__cpp_lib_tuple_element_t == 201402L);
#else
#error __cpp_lib_tuple_element_t is not defined
#endif

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
#ifdef __cpp_lib_tuple_like
STATIC_ASSERT(__cpp_lib_tuple_like == 202207L);
#else
#error __cpp_lib_tuple_like is not defined
#endif
#else
#ifdef __cpp_lib_tuple_like
#error __cpp_lib_tuple_like is defined
#endif
#endif

#ifdef __cpp_lib_tuples_by_type
STATIC_ASSERT(__cpp_lib_tuples_by_type == 201304L);
#else
#error __cpp_lib_tuples_by_type is not defined
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_type_identity
STATIC_ASSERT(__cpp_lib_type_identity == 201806L);
#else
#error __cpp_lib_type_identity is not defined
#endif
#else
#ifdef __cpp_lib_type_identity
#error __cpp_lib_type_identity is defined
#endif
#endif

#ifdef __cpp_lib_type_trait_variable_templates
STATIC_ASSERT(__cpp_lib_type_trait_variable_templates == 201510L);
#else
#error __cpp_lib_type_trait_variable_templates is not defined
#endif

#ifdef __cpp_lib_uncaught_exceptions
STATIC_ASSERT(__cpp_lib_uncaught_exceptions == 201411L);
#else
#error __cpp_lib_uncaught_exceptions is not defined
#endif

#ifdef __cpp_lib_unordered_map_try_emplace
STATIC_ASSERT(__cpp_lib_unordered_map_try_emplace == 201411L);
#else
#error __cpp_lib_unordered_map_try_emplace is not defined
#endif

#if _HAS_CXX23
#ifdef __cpp_lib_unreachable
STATIC_ASSERT(__cpp_lib_unreachable == 202202L);
#else
#error __cpp_lib_unreachable is not defined
#endif
#else
#ifdef __cpp_lib_unreachable
#error __cpp_lib_unreachable is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_unwrap_ref
STATIC_ASSERT(__cpp_lib_unwrap_ref == 201811L);
#else
#error __cpp_lib_unwrap_ref is not defined
#endif
#else
#ifdef __cpp_lib_unwrap_ref
#error __cpp_lib_unwrap_ref is defined
#endif
#endif

#if _HAS_CXX20
#ifdef __cpp_lib_variant
STATIC_ASSERT(__cpp_lib_variant == 202106L);
#else
#error __cpp_lib_variant is not defined
#endif
#elif _HAS_CXX17
#ifdef __cpp_lib_variant
STATIC_ASSERT(__cpp_lib_variant == 202102L);
#else
#error __cpp_lib_variant is not defined
#endif
#else
#ifdef __cpp_lib_variant
#error __cpp_lib_variant is defined
#endif
#endif

#ifdef __cpp_lib_void_t
STATIC_ASSERT(__cpp_lib_void_t == 201411L);
#else
#error __cpp_lib_void_t is not defined
#endif
