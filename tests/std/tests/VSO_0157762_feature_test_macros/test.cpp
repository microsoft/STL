// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {} // COMPILE-ONLY

// Always defined to varying values (C++14 versus C++17-and-newer mode).

#ifndef __cpp_constexpr
#error BOOM
#elif _HAS_CXX17
#if __cpp_constexpr != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_constexpr == 201603L);
#endif
#else
#if __cpp_constexpr != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_constexpr == 201304L);
#endif
#endif

#ifndef __cpp_inheriting_constructors
#error BOOM
#elif (_HAS_CXX17 || defined(__clang__)) && !defined(__EDG__) // Clang implemented this C++17 feature unconditionally.
                                                              // TRANSITION, VSO-610203
#if __cpp_inheriting_constructors != 201511L
#error BOOM
#else
STATIC_ASSERT(__cpp_inheriting_constructors == 201511L);
#endif
#else
#if __cpp_inheriting_constructors != 200802L
#error BOOM
#else
STATIC_ASSERT(__cpp_inheriting_constructors == 200802L);
#endif
#endif

#ifndef __cpp_static_assert
#error BOOM
#elif _HAS_CXX17
#if __cpp_static_assert != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_static_assert == 201411L);
#endif
#else
#if __cpp_static_assert != 200410L
#error BOOM
#else
STATIC_ASSERT(__cpp_static_assert == 200410L);
#endif
#endif


// Always defined to specific values.

#ifndef __cpp_aggregate_nsdmi
#error BOOM
#elif __cpp_aggregate_nsdmi != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_aggregate_nsdmi == 201304L);
#endif

#ifndef __cpp_alias_templates
#error BOOM
#elif __cpp_alias_templates != 200704L
#error BOOM
#else
STATIC_ASSERT(__cpp_alias_templates == 200704L);
#endif

#ifndef __cpp_attributes
#error BOOM
#elif __cpp_attributes != 200809L
#error BOOM
#else
STATIC_ASSERT(__cpp_attributes == 200809L);
#endif

#ifndef __cpp_binary_literals
#error BOOM
#elif __cpp_binary_literals != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_binary_literals == 201304L);
#endif

#ifndef __cpp_decltype
#error BOOM
#elif __cpp_decltype != 200707L
#error BOOM
#else
STATIC_ASSERT(__cpp_decltype == 200707L);
#endif

#ifndef __cpp_decltype_auto
#error BOOM
#elif __cpp_decltype_auto != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_decltype_auto == 201304L);
#endif

#ifndef __cpp_delegating_constructors
#error BOOM
#elif __cpp_delegating_constructors != 200604L
#error BOOM
#else
STATIC_ASSERT(__cpp_delegating_constructors == 200604L);
#endif

#if !defined(__clang__) || _HAS_CXX17 // C1XX implemented this C++17 feature unconditionally.
#ifndef __cpp_enumerator_attributes
#error BOOM
#elif __cpp_enumerator_attributes != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_enumerator_attributes == 201411L);
#endif
#else
#ifdef __cpp_enumerator_attributes
#error BOOM
#endif
#endif

#ifndef __cpp_generic_lambdas
#error BOOM
#elif __cpp_generic_lambdas != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_generic_lambdas == 201304L);
#endif

#ifndef __cpp_init_captures
#error BOOM
#elif __cpp_init_captures != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_init_captures == 201304L);
#endif

#ifndef __cpp_initializer_lists
#error BOOM
#elif __cpp_initializer_lists != 200806L
#error BOOM
#else
STATIC_ASSERT(__cpp_initializer_lists == 200806L);
#endif

#ifndef __cpp_lambdas
#error BOOM
#elif __cpp_lambdas != 200907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lambdas == 200907L);
#endif

#if !defined(__clang__) || _HAS_CXX17 // C1XX implemented this C++17 feature unconditionally.
#ifndef __cpp_namespace_attributes
#error BOOM
#elif __cpp_namespace_attributes != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_namespace_attributes == 201411L);
#endif
#else
#ifdef __cpp_namespace_attributes
#error BOOM
#endif
#endif

#ifndef __cpp_nsdmi
#error BOOM
#elif __cpp_nsdmi != 200809L
#error BOOM
#else
STATIC_ASSERT(__cpp_nsdmi == 200809L);
#endif

#ifndef __cpp_range_based_for
#error BOOM
#elif !defined(__clang__) || _HAS_CXX17 // C1XX implemented this C++17 feature unconditionally.
#if __cpp_range_based_for != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_range_based_for == 201603L);
#endif
#else
#if __cpp_range_based_for != 200907L
#error BOOM
#else
STATIC_ASSERT(__cpp_range_based_for == 200907L);
#endif
#endif

#ifndef __cpp_raw_strings
#error BOOM
#elif __cpp_raw_strings != 200710L
#error BOOM
#else
STATIC_ASSERT(__cpp_raw_strings == 200710L);
#endif

#ifndef __cpp_ref_qualifiers
#error BOOM
#elif __cpp_ref_qualifiers != 200710L
#error BOOM
#else
STATIC_ASSERT(__cpp_ref_qualifiers == 200710L);
#endif

#ifndef __cpp_return_type_deduction
#error BOOM
#elif __cpp_return_type_deduction != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_return_type_deduction == 201304L);
#endif

#ifndef __cpp_rvalue_references
#error BOOM
#elif __cpp_rvalue_references != 200610L
#error BOOM
#else
STATIC_ASSERT(__cpp_rvalue_references == 200610L);
#endif

#ifndef __cpp_unicode_characters
#error BOOM
#elif __cpp_unicode_characters != 200704L
#error BOOM
#else
STATIC_ASSERT(__cpp_unicode_characters == 200704L);
#endif

#ifndef __cpp_unicode_literals
#error BOOM
#elif __cpp_unicode_literals != 200710L
#error BOOM
#else
STATIC_ASSERT(__cpp_unicode_literals == 200710L);
#endif

#ifndef __cpp_user_defined_literals
#error BOOM
#elif __cpp_user_defined_literals != 200809L
#error BOOM
#else
STATIC_ASSERT(__cpp_user_defined_literals == 200809L);
#endif

#ifndef __cpp_variable_templates
#error BOOM
#elif __cpp_variable_templates != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_variable_templates == 201304L);
#endif

#ifndef __cpp_variadic_templates
#error BOOM
#elif __cpp_variadic_templates != 200704L
#error BOOM
#else
STATIC_ASSERT(__cpp_variadic_templates == 200704L);
#endif


// Defined in C++17-and-newer mode to specific values.

#if _HAS_CXX17
#ifndef __cpp_aggregate_bases
#error BOOM
#elif __cpp_aggregate_bases != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_aggregate_bases == 201603L);
#endif
#else
#ifdef __cpp_aggregate_bases
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_capture_star_this
#error BOOM
#elif __cpp_capture_star_this != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_capture_star_this == 201603L);
#endif
#else
#ifdef __cpp_capture_star_this
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_deduction_guides
#error BOOM
#elif __cpp_deduction_guides != 201703L
#error BOOM
#else
STATIC_ASSERT(__cpp_deduction_guides == 201703L);
#endif
#else
#ifdef __cpp_deduction_guides
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_fold_expressions
#error BOOM
#elif __cpp_fold_expressions != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_fold_expressions == 201603L);
#endif
#else
#ifdef __cpp_fold_expressions
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_guaranteed_copy_elision
#error BOOM
#elif __cpp_guaranteed_copy_elision != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_guaranteed_copy_elision == 201606L);
#endif
#else
#ifdef __cpp_guaranteed_copy_elision
#error BOOM
#endif
#endif

#if _HAS_CXX17 || defined(__EDG__) // EDG unconditionally reports hex floats as being available.
#ifndef __cpp_hex_float
#error BOOM
#elif __cpp_hex_float != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_hex_float == 201603L);
#endif
#else
#ifdef __cpp_hex_float
#error BOOM
#endif
#endif

#if _HAS_CXX17 || defined(__EDG__) // EDG unconditionally reports "if constexpr" as being available.
#ifndef __cpp_if_constexpr
#error BOOM
#elif __cpp_if_constexpr != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_if_constexpr == 201606L);
#endif
#else
#ifdef __cpp_if_constexpr
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_inline_variables
#error BOOM
#elif __cpp_inline_variables != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_inline_variables == 201606L);
#endif
#else
#ifdef __cpp_inline_variables
#error BOOM
#endif
#endif

#if _HAS_CXX20 && !defined(__clang__) && !defined(__EDG__)
#ifndef __cpp_nontype_template_args
#error BOOM
#elif __cpp_nontype_template_args != 201911L
#error BOOM
#else
STATIC_ASSERT(__cpp_nontype_template_args == 201911L);
#endif
#elif _HAS_CXX17
#ifndef __cpp_nontype_template_args
#error BOOM
#elif __cpp_nontype_template_args != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_nontype_template_args == 201411L);
#endif
#else
#ifdef __cpp_nontype_template_args
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_nontype_template_parameter_auto
#error BOOM
#elif __cpp_nontype_template_parameter_auto != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_nontype_template_parameter_auto == 201606L);
#endif
#else
#ifdef __cpp_nontype_template_parameter_auto
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_structured_bindings
#error BOOM
#elif __cpp_structured_bindings != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_structured_bindings == 201606L);
#endif
#else
#ifdef __cpp_structured_bindings
#error BOOM
#endif
#endif

#if _HAS_CXX17 && !defined(__clang__)
// https://clang.llvm.org/cxx_status.html says P0522R0 is Partial, "Despite being
// the resolution to a Defect Report, this feature is disabled by default in all
// language versions, and can be enabled explicitly with the flag
// -frelaxed-template-template-args in Clang 4 onwards. The change to the standard
// lacks a corresponding change for template partial ordering, resulting in
// ambiguity errors for reasonable and previously-valid code. This issue is
// expected to be rectified soon."
#ifndef __cpp_template_template_args
#error BOOM
#elif __cpp_template_template_args != 201611L
#error BOOM
#else
STATIC_ASSERT(__cpp_template_template_args == 201611L);
#endif
#else
#ifdef __cpp_template_template_args
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_variadic_using
#error BOOM
#elif __cpp_variadic_using != 201611L
#error BOOM
#else
STATIC_ASSERT(__cpp_variadic_using == 201611L);
#endif
#else
#ifdef __cpp_variadic_using
#error BOOM
#endif
#endif

// Defined in C++20-and-newer mode to specific values.

#if _HAS_CXX20 || defined(__EDG__)
// EDG unconditionally reports "explicit(bool)" as being available.
#ifndef __cpp_conditional_explicit
#error BOOM
#elif __cpp_conditional_explicit != 201806L
#error BOOM
#else
STATIC_ASSERT(__cpp_conditional_explicit == 201806L);
#endif
#else
#ifdef __cpp_conditional_explicit
#error BOOM
#endif
#endif

#if _HAS_CXX20 && !defined(__clang__)
// Clang only has partial support for <=> but that does not include the feature test macro.
#ifndef __cpp_impl_three_way_comparison
#error BOOM
#elif defined(__EDG__) // EDG does not yet implement P1630R1 or P1186R3 so they still report the old value.
#if __cpp_impl_three_way_comparison != 201711L
#error BOOM
#else
STATIC_ASSERT(__cpp_impl_three_way_comparison == 201711L);
#endif
#else
#if __cpp_impl_three_way_comparison != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_impl_three_way_comparison == 201907L);
#endif
#endif
#else
#ifdef __cpp_impl_three_way_comparison
#error BOOM
#endif
#endif

// Conditionally defined (various compiler options) to specific values.

// C++17, /Zc:alignedNew[-]
#if !_HAS_CXX17 || defined(TEST_DISABLED_ALIGNED_NEW)
#ifdef __cpp_aligned_new
#error BOOM
#endif
#else
#ifndef __cpp_aligned_new
#error BOOM
#elif __cpp_aligned_new != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_aligned_new == 201606L);
#endif
#endif

// C++98, /EHs[-]
#ifdef TEST_DISABLED_EXCEPTIONS
#ifdef __cpp_exceptions
#error BOOM
#endif
#else
#ifndef __cpp_exceptions
#error BOOM
#elif __cpp_exceptions != 199711L
#error BOOM
#else
STATIC_ASSERT(__cpp_exceptions == 199711L);
#endif
#endif

// C++17, /Zc:noexceptTypes[-]
#if !_HAS_CXX17 || defined(TEST_DISABLED_NOEXCEPT_FUNCTION_TYPE)
#ifdef __cpp_noexcept_function_type
#error BOOM
#endif
#else
#ifndef __cpp_noexcept_function_type
#error BOOM
#elif __cpp_noexcept_function_type != 201510L
#error BOOM
#else
STATIC_ASSERT(__cpp_noexcept_function_type == 201510L);
#endif
#endif

// C++98, /GR[-]
#ifdef TEST_DISABLED_RTTI
#ifdef __cpp_rtti
#error BOOM
#endif
#else
#ifndef __cpp_rtti
#error BOOM
#elif __cpp_rtti != 199711L
#error BOOM
#else
STATIC_ASSERT(__cpp_rtti == 199711L);
#endif
#endif

// C++14, /Zc:sizedDealloc[-]
#if defined(TEST_DISABLED_SIZED_DEALLOCATION) || defined(__clang__) // Clang disables sized deallocation by default.
#ifdef __cpp_sized_deallocation
#error BOOM
#endif
#else
#ifndef __cpp_sized_deallocation
#error BOOM
#elif __cpp_sized_deallocation != 201309L
#error BOOM
#else
STATIC_ASSERT(__cpp_sized_deallocation == 201309L);
#endif
#endif

// C++11, /Zc:threadSafeInit[-]
#if defined(_M_CEE_PURE) || defined(TEST_DISABLED_THREADSAFE_STATIC_INIT)
#ifdef __cpp_threadsafe_static_init
#error BOOM
#endif
#else
#ifndef __cpp_threadsafe_static_init
#error BOOM
#elif __cpp_threadsafe_static_init != 200806L
#error BOOM
#else
STATIC_ASSERT(__cpp_threadsafe_static_init == 200806L);
#endif
#endif


// Attributes.

#ifdef __has_cpp_attribute
// Good.
#else
#error BOOM
#endif

#if defined(__has_cpp_attribute)
// Good.
#else
#error BOOM
#endif

#ifndef __has_cpp_attribute
#error BOOM
#endif


// Always defined to specific values.

#if __has_cpp_attribute(carries_dependency) != 200809L
#error BOOM
#endif

#if __has_cpp_attribute(deprecated) != 201309L
#error BOOM
#endif

#if defined(__clang__) || defined(__EDG__) // clang and EDG don't yet implement P1771R1
#if __has_cpp_attribute(nodiscard) != 201603L
#error BOOM
#endif
#else
#if __has_cpp_attribute(nodiscard) != 201907L
#error BOOM
#endif
#endif

#if __has_cpp_attribute(noreturn) != 200809L
#error BOOM
#endif


// Always defined to varying values (C++14 versus C++17-and-newer mode).

#if _HAS_CXX17 || defined(__clang__) || defined(__EDG__) // Clang and EDG provide this in C++14 mode.
#if __has_cpp_attribute(fallthrough) != 201603L
#error BOOM
#endif
#else
#if __has_cpp_attribute(fallthrough) != 0
#error BOOM
#endif
#endif

#if _HAS_CXX17 || defined(__clang__) || defined(__EDG__) // Clang and EDG provide this in C++14 mode.
#if __has_cpp_attribute(maybe_unused) != 201603L
#error BOOM
#endif
#else
#if __has_cpp_attribute(maybe_unused) != 0
#error BOOM
#endif
#endif


// LIBRARY FEATURE-TEST MACROS

#include <version>


// Always defined to varying values (C++14 versus C++17-and-newer mode).

#ifndef __cpp_lib_chrono
#error BOOM
#elif _HAS_CXX17
#if __cpp_lib_chrono != 201611L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_chrono == 201611L);
#endif
#else
#if __cpp_lib_chrono != 201510L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_chrono == 201510L);
#endif
#endif


// Always defined to specific values.

#ifndef __cpp_lib_addressof_constexpr
#error BOOM
#elif __cpp_lib_addressof_constexpr != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_addressof_constexpr == 201603L);
#endif

#ifndef __cpp_lib_allocator_traits_is_always_equal
#error BOOM
#elif __cpp_lib_allocator_traits_is_always_equal != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_allocator_traits_is_always_equal == 201411L);
#endif

#ifndef __cpp_lib_as_const
#error BOOM
#elif __cpp_lib_as_const != 201510L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_as_const == 201510L);
#endif

#ifndef __cpp_lib_atomic_value_initialization
#error BOOM
#elif __cpp_lib_atomic_value_initialization != 201911L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_atomic_value_initialization == 201911L);
#endif

#ifndef __cpp_lib_bool_constant
#error BOOM
#elif __cpp_lib_bool_constant != 201505L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_bool_constant == 201505L);
#endif

#ifndef __cpp_lib_chrono_udls
#error BOOM
#elif __cpp_lib_chrono_udls != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_chrono_udls == 201304L);
#endif

#ifndef __cpp_lib_complex_udls
#error BOOM
#elif __cpp_lib_complex_udls != 201309L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_complex_udls == 201309L);
#endif

#ifndef __cpp_lib_enable_shared_from_this
#error BOOM
#elif __cpp_lib_enable_shared_from_this != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_enable_shared_from_this == 201603L);
#endif

#ifndef __cpp_lib_exchange_function
#error BOOM
#elif __cpp_lib_exchange_function != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_exchange_function == 201304L);
#endif

#ifndef __cpp_lib_experimental_erase_if
#error BOOM
#elif __cpp_lib_experimental_erase_if != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_experimental_erase_if == 201411L);
#endif

#ifndef __cpp_lib_experimental_filesystem
#error BOOM
#elif __cpp_lib_experimental_filesystem != 201406L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_experimental_filesystem == 201406L);
#endif

#ifndef __cpp_lib_generic_associative_lookup
#error BOOM
#elif __cpp_lib_generic_associative_lookup != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_generic_associative_lookup == 201304L);
#endif

#ifndef __cpp_lib_incomplete_container_elements
#error BOOM
#elif __cpp_lib_incomplete_container_elements != 201505L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_incomplete_container_elements == 201505L);
#endif

#ifndef __cpp_lib_integer_sequence
#error BOOM
#elif __cpp_lib_integer_sequence != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_integer_sequence == 201304L);
#endif

#ifndef __cpp_lib_integral_constant_callable
#error BOOM
#elif __cpp_lib_integral_constant_callable != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_integral_constant_callable == 201304L);
#endif

#ifndef __cpp_lib_invoke
#error BOOM
#elif __cpp_lib_invoke != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_invoke == 201411L);
#endif

#ifndef __cpp_lib_is_final
#error BOOM
#elif __cpp_lib_is_final != 201402L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_is_final == 201402L);
#endif

#ifndef __cpp_lib_is_null_pointer
#error BOOM
#elif __cpp_lib_is_null_pointer != 201309L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_is_null_pointer == 201309L);
#endif

#ifndef __cpp_lib_logical_traits
#error BOOM
#elif __cpp_lib_logical_traits != 201510L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_logical_traits == 201510L);
#endif

#ifndef __cpp_lib_make_reverse_iterator
#error BOOM
#elif __cpp_lib_make_reverse_iterator != 201402L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_make_reverse_iterator == 201402L);
#endif

#ifndef __cpp_lib_make_unique
#error BOOM
#elif __cpp_lib_make_unique != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_make_unique == 201304L);
#endif

#ifndef __cpp_lib_map_try_emplace
#error BOOM
#elif __cpp_lib_map_try_emplace != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_map_try_emplace == 201411L);
#endif

#ifndef __cpp_lib_nonmember_container_access
#error BOOM
#elif __cpp_lib_nonmember_container_access != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_nonmember_container_access == 201411L);
#endif

#ifndef __cpp_lib_null_iterators
#error BOOM
#elif __cpp_lib_null_iterators != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_null_iterators == 201304L);
#endif

#ifndef __cpp_lib_quoted_string_io
#error BOOM
#elif __cpp_lib_quoted_string_io != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_quoted_string_io == 201304L);
#endif

#ifndef __cpp_lib_result_of_sfinae
#error BOOM
#elif __cpp_lib_result_of_sfinae != 201210L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_result_of_sfinae == 201210L);
#endif

#ifndef __cpp_lib_robust_nonmodifying_seq_ops
#error BOOM
#elif __cpp_lib_robust_nonmodifying_seq_ops != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_robust_nonmodifying_seq_ops == 201304L);
#endif

#ifndef __cpp_lib_shared_mutex
#error BOOM
#elif __cpp_lib_shared_mutex != 201505L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_shared_mutex == 201505L);
#endif

#ifndef __cpp_lib_shared_ptr_arrays
#error BOOM
#elif __cpp_lib_shared_ptr_arrays != 201611L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_arrays == 201611L);
#endif

#ifdef _M_CEE
#ifdef __cpp_lib_shared_timed_mutex
#error BOOM
#endif
#else // ^^^ _M_CEE ^^^ // vvv !_M_CEE vvv
#ifndef __cpp_lib_shared_timed_mutex
#error BOOM
#elif __cpp_lib_shared_timed_mutex != 201402L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_shared_timed_mutex == 201402L);
#endif
#endif // _M_CEE

#ifndef __cpp_lib_string_udls
#error BOOM
#elif __cpp_lib_string_udls != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_string_udls == 201304L);
#endif

#ifndef __cpp_lib_transformation_trait_aliases
#error BOOM
#elif __cpp_lib_transformation_trait_aliases != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_transformation_trait_aliases == 201304L);
#endif

#ifndef __cpp_lib_transparent_operators
#error BOOM
#elif __cpp_lib_transparent_operators != 201510L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_transparent_operators == 201510L);
#endif

#ifndef __cpp_lib_tuple_element_t
#error BOOM
#elif __cpp_lib_tuple_element_t != 201402L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_tuple_element_t == 201402L);
#endif

#ifndef __cpp_lib_tuples_by_type
#error BOOM
#elif __cpp_lib_tuples_by_type != 201304L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_tuples_by_type == 201304L);
#endif

#ifndef __cpp_lib_type_trait_variable_templates
#error BOOM
#elif __cpp_lib_type_trait_variable_templates != 201510L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_type_trait_variable_templates == 201510L);
#endif

#ifndef __cpp_lib_uncaught_exceptions
#error BOOM
#elif __cpp_lib_uncaught_exceptions != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_uncaught_exceptions == 201411L);
#endif

#ifndef __cpp_lib_unordered_map_try_emplace
#error BOOM
#elif __cpp_lib_unordered_map_try_emplace != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_unordered_map_try_emplace == 201411L);
#endif

#ifndef __cpp_lib_void_t
#error BOOM
#elif __cpp_lib_void_t != 201411L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_void_t == 201411L);
#endif


// Defined in C++17-and-newer mode to specific values.

#if _HAS_CXX17
#ifndef __cpp_lib_any
#error BOOM
#elif __cpp_lib_any != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_any == 201606L);
#endif
#else
#ifdef __cpp_lib_any
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_apply
#error BOOM
#elif __cpp_lib_apply != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_apply == 201603L);
#endif
#else
#ifdef __cpp_lib_apply
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_array_constexpr
#error BOOM
#elif __cpp_lib_array_constexpr != 201803L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_array_constexpr == 201803L);
#endif
#else
#ifdef __cpp_lib_array_constexpr
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_atomic_is_always_lock_free
#error BOOM
#elif __cpp_lib_atomic_is_always_lock_free != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_atomic_is_always_lock_free == 201603L);
#endif
#else
#ifdef __cpp_lib_atomic_is_always_lock_free
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_boyer_moore_searcher
#error BOOM
#elif __cpp_lib_boyer_moore_searcher != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_boyer_moore_searcher == 201603L);
#endif
#else
#ifdef __cpp_lib_boyer_moore_searcher
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_clamp
#error BOOM
#elif __cpp_lib_clamp != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_clamp == 201603L);
#endif
#else
#ifdef __cpp_lib_clamp
#error BOOM
#endif
#endif

#if _HAS_CXX17 && !defined(_M_CEE)
#ifndef __cpp_lib_execution
#error BOOM
#elif __cpp_lib_execution != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_execution == 201603L);
#endif
#else
#ifdef __cpp_lib_execution
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_filesystem
#error BOOM
#elif __cpp_lib_filesystem != 201703L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_filesystem == 201703L);
#endif
#else
#ifdef __cpp_lib_filesystem
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_gcd_lcm
#error BOOM
#elif __cpp_lib_gcd_lcm != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_gcd_lcm == 201606L);
#endif
#else
#ifdef __cpp_lib_gcd_lcm
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_hardware_interference_size
#error BOOM
#elif __cpp_lib_hardware_interference_size != 201703L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_hardware_interference_size == 201703L);
#endif
#else
#ifdef __cpp_lib_hardware_interference_size
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_has_unique_object_representations
#error BOOM
#elif __cpp_lib_has_unique_object_representations != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_has_unique_object_representations == 201606L);
#endif
#else
#ifdef __cpp_lib_has_unique_object_representations
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_hypot
#error BOOM
#elif __cpp_lib_hypot != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_hypot == 201603L);
#endif
#else
#ifdef __cpp_lib_hypot
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_aggregate
#error BOOM
#elif __cpp_lib_is_aggregate != 201703L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_is_aggregate == 201703L);
#endif
#else
#ifdef __cpp_lib_is_aggregate
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_invocable
#error BOOM
#elif __cpp_lib_is_invocable != 201703L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_is_invocable == 201703L);
#endif
#else
#ifdef __cpp_lib_is_invocable
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_swappable
#error BOOM
#elif __cpp_lib_is_swappable != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_is_swappable == 201603L);
#endif
#else
#ifdef __cpp_lib_is_swappable
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_launder
#error BOOM
#elif __cpp_lib_launder != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_launder == 201606L);
#endif
#else
#ifdef __cpp_lib_launder
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_make_from_tuple
#error BOOM
#elif __cpp_lib_make_from_tuple != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_make_from_tuple == 201606L);
#endif
#else
#ifdef __cpp_lib_make_from_tuple
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_math_special_functions
#error BOOM
#elif __cpp_lib_math_special_functions != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_math_special_functions == 201603L);
#endif
#else
#ifdef __cpp_lib_math_special_functions
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_memory_resource
#error BOOM
#elif __cpp_lib_memory_resource != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_memory_resource == 201603L);
#endif
#else
#ifdef __cpp_lib_memory_resource
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_node_extract
#error BOOM
#elif __cpp_lib_node_extract != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_node_extract == 201606L);
#endif
#else
#ifdef __cpp_lib_node_extract
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_not_fn
#error BOOM
#elif __cpp_lib_not_fn != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_not_fn == 201603L);
#endif
#else
#ifdef __cpp_lib_not_fn
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_optional
#error BOOM
#elif __cpp_lib_optional != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_optional == 201606L);
#endif
#else
#ifdef __cpp_lib_optional
#error BOOM
#endif
#endif

#if _HAS_CXX17 && !defined(_M_CEE)
#ifndef __cpp_lib_parallel_algorithm
#error BOOM
#elif __cpp_lib_parallel_algorithm != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_parallel_algorithm == 201603L);
#endif
#else
#ifdef __cpp_lib_parallel_algorithm
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_raw_memory_algorithms
#error BOOM
#elif __cpp_lib_raw_memory_algorithms != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_raw_memory_algorithms == 201606L);
#endif
#else
#ifdef __cpp_lib_raw_memory_algorithms
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_sample
#error BOOM
#elif __cpp_lib_sample != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_sample == 201603L);
#endif
#else
#ifdef __cpp_lib_sample
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_scoped_lock
#error BOOM
#elif __cpp_lib_scoped_lock != 201703L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_scoped_lock == 201703L);
#endif
#else
#ifdef __cpp_lib_scoped_lock
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_shared_ptr_weak_type
#error BOOM
#elif __cpp_lib_shared_ptr_weak_type != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_weak_type == 201606L);
#endif
#else
#ifdef __cpp_lib_shared_ptr_weak_type
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_string_view
#error BOOM
#elif __cpp_lib_string_view != 201803L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_string_view == 201803L);
#endif
#else
#ifdef __cpp_lib_string_view
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_to_chars
#error BOOM
#elif __cpp_lib_to_chars != 201611L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_to_chars == 201611L);
#endif
#else
#ifdef __cpp_lib_to_chars
#error BOOM
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_variant
#error BOOM
#elif __cpp_lib_variant != 201606L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_variant == 201606L);
#endif
#else
#ifdef __cpp_lib_variant
#error BOOM
#endif
#endif


// Conditionally defined in C++17-and-newer mode to specific values.

#if _HAS_STD_BYTE
#ifndef __cpp_lib_byte
#error BOOM
#elif __cpp_lib_byte != 201603L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_byte == 201603L);
#endif
#else
#ifdef __cpp_lib_byte
#error BOOM
#endif
#endif


// Defined in C++20-and-newer mode to specific values.

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_float
#error BOOM
#elif __cpp_lib_atomic_float != 201711L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_atomic_float == 201711L);
#endif
#else
#ifdef __cpp_lib_atomic_float
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bind_front
#error BOOM
#elif __cpp_lib_bind_front != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_bind_front == 201907L);
#endif
#else
#ifdef __cpp_lib_bind_front
#error BOOM
#endif
#endif

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, VSO-1041044
#ifndef __cpp_lib_bit_cast
#error BOOM
#elif __cpp_lib_bit_cast != 201806L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_bit_cast == 201806L);
#endif
#else
#ifdef __cpp_lib_bit_cast
#error BOOM
#endif
#endif

#if _HAS_CXX20 && (defined(__clang__) || defined(__EDG__)) // TRANSITION, VSO-1020212
#ifndef __cpp_lib_bitops
#error BOOM
#elif __cpp_lib_bitops != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_bitops == 201907L);
#endif
#else
#ifdef __cpp_lib_bitops
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bounded_array_traits
#error BOOM
#elif __cpp_lib_bounded_array_traits != 201902L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_bounded_array_traits == 201902L);
#endif
#else
#ifdef __cpp_lib_bounded_array_traits
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_algorithms
#error BOOM
#elif __cpp_lib_constexpr_algorithms != 201806L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_constexpr_algorithms == 201806L);
#endif
#else
#ifdef __cpp_lib_constexpr_algorithms
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_memory
#error BOOM
#elif __cpp_lib_constexpr_memory != 201811L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_constexpr_memory == 201811L);
#endif
#else
#ifdef __cpp_lib_constexpr_memory
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_numeric
#error BOOM
#elif __cpp_lib_constexpr_numeric != 201911L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_constexpr_numeric == 201911L);
#endif
#else
#ifdef __cpp_lib_constexpr_numeric
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_endian
#error BOOM
#elif __cpp_lib_endian != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_endian == 201907L);
#endif
#else
#ifdef __cpp_lib_endian
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_erase_if
#error BOOM
#elif __cpp_lib_erase_if != 202002L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_erase_if == 202002L);
#endif
#else
#ifdef __cpp_lib_erase_if
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_generic_unordered_lookup
#error BOOM
#elif __cpp_lib_generic_unordered_lookup != 201811L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_generic_unordered_lookup == 201811L);
#endif
#else
#ifdef __cpp_lib_generic_unordered_lookup
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_int_pow2
#error BOOM
#elif __cpp_lib_int_pow2 != 202002L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_int_pow2 == 202002L);
#endif
#else
#ifdef __cpp_lib_int_pow2
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_is_constant_evaluated
#error BOOM
#elif __cpp_lib_is_constant_evaluated != 201811L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_is_constant_evaluated == 201811L);
#endif
#else
#ifdef __cpp_lib_is_constant_evaluated
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_is_nothrow_convertible
#error BOOM
#elif __cpp_lib_is_nothrow_convertible != 201806L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_is_nothrow_convertible == 201806L);
#endif
#else
#ifdef __cpp_lib_is_nothrow_convertible
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_list_remove_return_type
#error BOOM
#elif __cpp_lib_list_remove_return_type != 201806L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_list_remove_return_type == 201806L);
#endif
#else
#ifdef __cpp_lib_list_remove_return_type
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_math_constants
#error BOOM
#elif __cpp_lib_math_constants != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_math_constants == 201907L);
#endif
#else
#ifdef __cpp_lib_math_constants
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_remove_cvref
#error BOOM
#elif __cpp_lib_remove_cvref != 201711L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_remove_cvref == 201711L);
#endif
#else
#ifdef __cpp_lib_remove_cvref
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_shift
#error BOOM
#elif __cpp_lib_shift != 201806L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_shift == 201806L);
#endif
#else
#ifdef __cpp_lib_shift
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_span
#error BOOM
#elif __cpp_lib_span != 202002L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_span == 202002L);
#endif
#else
#ifdef __cpp_lib_span
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_ssize
#error BOOM
#elif __cpp_lib_ssize != 201902L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_ssize == 201902L);
#endif
#else
#ifdef __cpp_lib_ssize
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_starts_ends_with
#error BOOM
#elif __cpp_lib_starts_ends_with != 201711L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_starts_ends_with == 201711L);
#endif
#else
#ifdef __cpp_lib_starts_ends_with
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_to_address
#error BOOM
#elif __cpp_lib_to_address != 201711L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_to_address == 201711L);
#endif
#else
#ifdef __cpp_lib_to_address
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_to_array
#error BOOM
#elif __cpp_lib_to_array != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_to_array == 201907L);
#endif
#else
#ifdef __cpp_lib_to_array
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_type_identity
#error BOOM
#elif __cpp_lib_type_identity != 201806L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_type_identity == 201806L);
#endif
#else
#ifdef __cpp_lib_type_identity
#error BOOM
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_unwrap_ref
#error BOOM
#elif __cpp_lib_unwrap_ref != 201811L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_unwrap_ref == 201811L);
#endif
#else
#ifdef __cpp_lib_unwrap_ref
#error BOOM
#endif
#endif


// Conditionally defined in C++20-and-newer mode to specific values.

#if _HAS_CXX20 && defined(__cpp_char8_t)
#ifndef __cpp_lib_char8_t
#error BOOM
#elif __cpp_lib_char8_t != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_char8_t == 201907L);
#endif
#else
#ifdef __cpp_lib_char8_t
#error BOOM
#endif
#endif

#if _HAS_CXX20 && defined(__cpp_concepts)
#ifndef __cpp_lib_concepts
#error BOOM
#elif __cpp_lib_concepts != 201907L
#error BOOM
#else
STATIC_ASSERT(__cpp_lib_concepts == 201907L);
#endif
#else
#ifdef __cpp_lib_concepts
#error BOOM
#endif
#endif
