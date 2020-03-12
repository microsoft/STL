// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {} // COMPILE-ONLY

// ATTRIBUTES.

#ifdef __has_cpp_attribute
// Good.
#else
#error Expected __has_cpp_attribute to be defined.
#endif
#if defined(__has_cpp_attribute)
// Good.
#else
#error Expected __has_cpp_attribute to be defined.
#endif
#ifndef __has_cpp_attribute
#error Expected __has_cpp_attribute to be defined.
#endif

#if __has_cpp_attribute(carries_dependency) != 200809L
#error Expected has_cpp_attribute(carries_dependency) to equal 200809L.
#endif
#if __has_cpp_attribute(deprecated) != 201309L
#error Expected has_cpp_attribute(deprecated) to equal 201309L.
#endif

#if defined(__clang__) || defined(__EDG__) // clang and EDG don't yet implement P1771R1
#if __has_cpp_attribute(nodiscard) != 201603L
#error Expected has_cpp_attribute(nodiscard) to equal 201603L.
#endif
#else
#if __has_cpp_attribute(nodiscard) != 201907L
#error Expected has_cpp_attribute(nodiscard) to equal 201907L.
#endif
#endif

#if __has_cpp_attribute(noreturn) != 200809L
#error Expected has_cpp_attribute(noreturn) to equal 200809L.
#endif

//// LANGUAGE FEATURE-TEST MACROS

#ifndef __cpp_aggregate_nsdmi
#error Expected __cpp_aggregate_nsdmi to be defined.
#elif __cpp_aggregate_nsdmi != 201304L
#error Expected cpp_aggregate_nsdmi to equal 201304L.
#else
STATIC_ASSERT(__cpp_aggregate_nsdmi == 201304L);
#endif

#ifndef __cpp_alias_templates
#error Expected __cpp_alias_templates to be defined.
#elif __cpp_alias_templates != 200704L
#error Expected cpp_alias_templates to equal 200704L.
#else
STATIC_ASSERT(__cpp_alias_templates == 200704L);
#endif

#ifndef __cpp_attributes
#error Expected __cpp_attributes to be defined.
#elif __cpp_attributes != 200809L
#error Expected cpp_attributes to equal 200809L.
#else
STATIC_ASSERT(__cpp_attributes == 200809L);
#endif

#ifndef __cpp_binary_literals
#error Expected __cpp_binary_literals to be defined.
#elif __cpp_binary_literals != 201304L
#error Expected cpp_binary_literals to equal 201304L.
#else
STATIC_ASSERT(__cpp_binary_literals == 201304L);
#endif

#ifndef __cpp_constexpr
#error Expected __cpp_constexpr to be defined.
#elif _HAS_CXX17
#if __cpp_constexpr != 201603L
#error Expected cpp_constexpr to equal 201603L.
#else
STATIC_ASSERT(__cpp_constexpr == 201603L);
#endif
#else
#if __cpp_constexpr != 201304L
#error Expected cpp_constexpr to equal 201304L.
#else
STATIC_ASSERT(__cpp_constexpr == 201304L);
#endif
#endif

#ifndef __cpp_decltype
#error Expected __cpp_decltype to be defined.
#elif __cpp_decltype != 200707L
#error Expected cpp_decltype to equal 200707L.
#else
STATIC_ASSERT(__cpp_decltype == 200707L);
#endif

#ifndef __cpp_decltype_auto
#error Expected __cpp_decltype_auto to be defined.
#elif __cpp_decltype_auto != 201304L
#error Expected cpp_decltype_auto to equal 201304L.
#else
STATIC_ASSERT(__cpp_decltype_auto == 201304L);
#endif

#ifndef __cpp_delegating_constructors
#error Expected __cpp_delegating_constructors to be defined.
#elif __cpp_delegating_constructors != 200604L
#error Expected cpp_delegating_constructors to equal 200604L.
#else
STATIC_ASSERT(__cpp_delegating_constructors == 200604L);
#endif

#ifndef __cpp_generic_lambdas
#error Expected __cpp_generic_lambdas to be defined.
#elif __cpp_generic_lambdas != 201304L
#error Expected cpp_generic_lambdas to equal 201304L.
#else
STATIC_ASSERT(__cpp_generic_lambdas == 201304L);
#endif

#ifndef __cpp_inheriting_constructors
#error Expected __cpp_inheriting_constructors to be defined.
#elif (_HAS_CXX17 || defined(__clang__)) && !defined(__EDG__) // Clang implemented this C++17 feature unconditionally.
                                                              // TRANSITION, VSO-610203
#if __cpp_inheriting_constructors != 201511L
#error Expected cpp_inheriting_constructors to equal 201511L.
#else
STATIC_ASSERT(__cpp_inheriting_constructors == 201511L);
#endif
#else
#if __cpp_inheriting_constructors != 200802L
#error Expected cpp_inheriting_constructors to equal 200802L.
#else
STATIC_ASSERT(__cpp_inheriting_constructors == 200802L);
#endif
#endif

#ifndef __cpp_init_captures
#error Expected __cpp_init_captures to be defined.
#elif __cpp_init_captures != 201304L
#error Expected cpp_init_captures to equal 201304L.
#else
STATIC_ASSERT(__cpp_init_captures == 201304L);
#endif

#ifndef __cpp_initializer_lists
#error Expected __cpp_initializer_lists to be defined.
#elif __cpp_initializer_lists != 200806L
#error Expected cpp_initializer_lists to equal 200806L.
#else
STATIC_ASSERT(__cpp_initializer_lists == 200806L);
#endif

#ifndef __cpp_lambdas
#error Expected __cpp_lambdas to be defined.
#elif __cpp_lambdas != 200907L
#error Expected cpp_lambdas to equal 200907L.
#else
STATIC_ASSERT(__cpp_lambdas == 200907L);
#endif

#ifndef __cpp_nsdmi
#error Expected __cpp_nsdmi to be defined.
#elif __cpp_nsdmi != 200809L
#error Expected cpp_nsdmi to equal 200809L.
#else
STATIC_ASSERT(__cpp_nsdmi == 200809L);
#endif

#ifndef __cpp_range_based_for
#error Expected __cpp_range_based_for to be defined.
#elif !defined(__clang__) || _HAS_CXX17 // C1XX implemented this C++17 feature unconditionally.
#if __cpp_range_based_for != 201603L
#error Expected cpp_range_based_for to equal 201603L.
#else
STATIC_ASSERT(__cpp_range_based_for == 201603L);
#endif
#else
#if __cpp_range_based_for != 200907L
#error Expected cpp_range_based_for to equal 200907L.
#else
STATIC_ASSERT(__cpp_range_based_for == 200907L);
#endif
#endif

#ifndef __cpp_raw_strings
#error Expected __cpp_raw_strings to be defined.
#elif __cpp_raw_strings != 200710L
#error Expected cpp_raw_strings to equal 200710L.
#else
STATIC_ASSERT(__cpp_raw_strings == 200710L);
#endif

#ifndef __cpp_ref_qualifiers
#error Expected __cpp_ref_qualifiers to be defined.
#elif __cpp_ref_qualifiers != 200710L
#error Expected cpp_ref_qualifiers to equal 200710L.
#else
STATIC_ASSERT(__cpp_ref_qualifiers == 200710L);
#endif

#ifndef __cpp_return_type_deduction
#error Expected __cpp_return_type_deduction to be defined.
#elif __cpp_return_type_deduction != 201304L
#error Expected cpp_return_type_deduction to equal 201304L.
#else
STATIC_ASSERT(__cpp_return_type_deduction == 201304L);
#endif

#ifndef __cpp_rvalue_references
#error Expected __cpp_rvalue_references to be defined.
#elif __cpp_rvalue_references != 200610L
#error Expected cpp_rvalue_references to equal 200610L.
#else
STATIC_ASSERT(__cpp_rvalue_references == 200610L);
#endif

#ifndef __cpp_static_assert
#error Expected __cpp_static_assert to be defined.
#elif _HAS_CXX17
#if __cpp_static_assert != 201411L
#error Expected cpp_static_assert to equal 201411L.
#else
STATIC_ASSERT(__cpp_static_assert == 201411L);
#endif
#else
#if __cpp_static_assert != 200410L
#error Expected cpp_static_assert to equal 200410L.
#else
STATIC_ASSERT(__cpp_static_assert == 200410L);
#endif
#endif

#ifndef __cpp_unicode_characters
#error Expected __cpp_unicode_characters to be defined.
#elif __cpp_unicode_characters != 200704L
#error Expected cpp_unicode_characters to equal 200704L.
#else
STATIC_ASSERT(__cpp_unicode_characters == 200704L);
#endif

#ifndef __cpp_unicode_literals
#error Expected __cpp_unicode_literals to be defined.
#elif __cpp_unicode_literals != 200710L
#error Expected cpp_unicode_literals to equal 200710L.
#else
STATIC_ASSERT(__cpp_unicode_literals == 200710L);
#endif

#ifndef __cpp_user_defined_literals
#error Expected __cpp_user_defined_literals to be defined.
#elif __cpp_user_defined_literals != 200809L
#error Expected cpp_user_defined_literals to equal 200809L.
#else
STATIC_ASSERT(__cpp_user_defined_literals == 200809L);
#endif

#ifndef __cpp_variable_templates
#error Expected __cpp_variable_templates to be defined.
#elif __cpp_variable_templates != 201304L
#error Expected cpp_variable_templates to equal 201304L.
#else
STATIC_ASSERT(__cpp_variable_templates == 201304L);
#endif

#ifndef __cpp_variadic_templates
#error Expected __cpp_variadic_templates to be defined.
#elif __cpp_variadic_templates != 200704L
#error Expected cpp_variadic_templates to equal 200704L.
#else
STATIC_ASSERT(__cpp_variadic_templates == 200704L);
#endif

// C++98, /GR[-]
#ifdef TEST_DISABLED_RTTI
#ifdef __cpp_rtti
#error Expected __cpp_rtti to NOT be defined.
#endif
#else
#ifndef __cpp_rtti
#error Expected __cpp_rtti to be defined.
#elif __cpp_rtti != 199711L
#error Expected cpp_rtti to equal 199711L.
#else
STATIC_ASSERT(__cpp_rtti == 199711L);
#endif
#endif

// C++11, /Zc:threadSafeInit[-]
#if defined(_M_CEE_PURE) || defined(TEST_DISABLED_THREADSAFE_STATIC_INIT)
#ifdef __cpp_threadsafe_static_init
#error Expected __cpp_threadsafe_static_init to NOT be defined.
#endif
#else
#ifndef __cpp_threadsafe_static_init
#error Expected __cpp_threadsafe_static_init to be defined.
#elif __cpp_threadsafe_static_init != 200806L
#error Expected cpp_threadsafe_static_init to equal 200806L.
#else
STATIC_ASSERT(__cpp_threadsafe_static_init == 200806L);
#endif
#endif

// C++14, /Zc:sizedDealloc[-]
#if defined(TEST_DISABLED_SIZED_DEALLOCATION) || defined(__clang__) // Clang disables sized deallocation by default.
#ifdef __cpp_sized_deallocation
#error Expected __cpp_sized_deallocation to NOT be defined.
#endif
#else
#ifndef __cpp_sized_deallocation
#error Expected __cpp_sized_deallocation to be defined.
#elif __cpp_sized_deallocation != 201309L
#error Expected cpp_sized_deallocation to equal 201309L.
#else
STATIC_ASSERT(__cpp_sized_deallocation == 201309L);
#endif
#endif

// C++17, /Zc:alignedNew[-]
#if !_HAS_CXX17 || defined(TEST_DISABLED_ALIGNED_NEW)
#ifdef __cpp_aligned_new
#error Expected __cpp_aligned_new to NOT be defined.
#endif
#else
#ifndef __cpp_aligned_new
#error Expected __cpp_aligned_new to be defined.
#elif __cpp_aligned_new != 201606L
#error Expected cpp_aligned_new to equal 201606L.
#else
STATIC_ASSERT(__cpp_aligned_new == 201606L);
#endif
#endif

// C++17, /Zc:noexceptTypes[-]
#if !_HAS_CXX17 || defined(TEST_DISABLED_NOEXCEPT_FUNCTION_TYPE)
#ifdef __cpp_noexcept_function_type
#error Expected __cpp_noexcept_function_type to NOT be defined.
#endif
#else
#ifndef __cpp_noexcept_function_type
#error Expected __cpp_noexcept_function_type to be defined.
#elif __cpp_noexcept_function_type != 201510L
#error Expected cpp_noexcept_function_type to equal 201510L.
#else
STATIC_ASSERT(__cpp_noexcept_function_type == 201510L);
#endif
#endif

// C++98, /EHs[-]
#ifdef TEST_DISABLED_EXCEPTIONS
#ifdef __cpp_exceptions
#error Expected __cpp_exceptions to NOT be defined.
#endif
#else
#ifndef __cpp_exceptions
#error Expected __cpp_exceptions to be defined.
#elif __cpp_exceptions != 199711L
#error Expected cpp_exceptions to equal 199711L.
#else
STATIC_ASSERT(__cpp_exceptions == 199711L);
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_aggregate_bases
#error Expected __cpp_aggregate_bases to be defined.
#elif __cpp_aggregate_bases != 201603L
#error Expected cpp_aggregate_bases to equal 201603L.
#else
STATIC_ASSERT(__cpp_aggregate_bases == 201603L);
#endif
#else
#ifdef __cpp_aggregate_bases
#error Expected __cpp_aggregate_bases to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_capture_star_this
#error Expected __cpp_capture_star_this to be defined.
#elif __cpp_capture_star_this != 201603L
#error Expected cpp_capture_star_this to equal 201603L.
#else
STATIC_ASSERT(__cpp_capture_star_this == 201603L);
#endif
#else
#ifdef __cpp_capture_star_this
#error Expected __cpp_capture_star_this to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_deduction_guides
#error Expected __cpp_deduction_guides to be defined.
#elif __cpp_deduction_guides != 201703L
#error Expected cpp_deduction_guides to equal 201703L.
#else
STATIC_ASSERT(__cpp_deduction_guides == 201703L);
#endif
#else
#ifdef __cpp_deduction_guides
#error Expected __cpp_deduction_guides to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_fold_expressions
#error Expected __cpp_fold_expressions to be defined.
#elif __cpp_fold_expressions != 201603L
#error Expected cpp_fold_expressions to equal 201603L.
#else
STATIC_ASSERT(__cpp_fold_expressions == 201603L);
#endif
#else
#ifdef __cpp_fold_expressions
#error Expected __cpp_fold_expressions to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_guaranteed_copy_elision
#error Expected __cpp_guaranteed_copy_elision to be defined.
#elif __cpp_guaranteed_copy_elision != 201606L
#error Expected cpp_guaranteed_copy_elision to equal 201606L.
#else
STATIC_ASSERT(__cpp_guaranteed_copy_elision == 201606L);
#endif
#else
#ifdef __cpp_guaranteed_copy_elision
#error Expected __cpp_guaranteed_copy_elision to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_inline_variables
#error Expected __cpp_inline_variables to be defined.
#elif __cpp_inline_variables != 201606L
#error Expected cpp_inline_variables to equal 201606L.
#else
STATIC_ASSERT(__cpp_inline_variables == 201606L);
#endif
#else
#ifdef __cpp_inline_variables
#error Expected __cpp_inline_variables to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_nontype_template_parameter_auto
#error Expected __cpp_nontype_template_parameter_auto to be defined.
#elif __cpp_nontype_template_parameter_auto != 201606L
#error Expected cpp_nontype_template_parameter_auto to equal 201606L.
#else
STATIC_ASSERT(__cpp_nontype_template_parameter_auto == 201606L);
#endif
#else
#ifdef __cpp_nontype_template_parameter_auto
#error Expected __cpp_nontype_template_parameter_auto to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_structured_bindings
#error Expected __cpp_structured_bindings to be defined.
#elif __cpp_structured_bindings != 201606L
#error Expected cpp_structured_bindings to equal 201606L.
#else
STATIC_ASSERT(__cpp_structured_bindings == 201606L);
#endif
#else
#ifdef __cpp_structured_bindings
#error Expected __cpp_structured_bindings to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_variadic_using
#error Expected __cpp_variadic_using to be defined.
#elif __cpp_variadic_using != 201611L
#error Expected cpp_variadic_using to equal 201611L.
#else
STATIC_ASSERT(__cpp_variadic_using == 201611L);
#endif
#else
#ifdef __cpp_variadic_using
#error Expected __cpp_variadic_using to NOT be defined.
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
#error Expected __cpp_template_template_args to be defined.
#elif __cpp_template_template_args != 201611L
#error Expected cpp_template_template_args to equal 201611L.
#else
STATIC_ASSERT(__cpp_template_template_args == 201611L);
#endif
#else
#ifdef __cpp_template_template_args
#error Expected __cpp_template_template_args to NOT be defined.
#endif
#endif

#if _HAS_CXX17 || !defined(__clang__) // C1XX implemented this C++17 feature unconditionally.
#ifndef __cpp_namespace_attributes
#error Expected __cpp_namespace_attributes to be defined.
#elif __cpp_namespace_attributes != 201411L
#error Expected cpp_namespace_attributes to equal 201411L.
#else
STATIC_ASSERT(__cpp_namespace_attributes == 201411L);
#endif
#else
#ifdef __cpp_namespace_attributes
#error Expected __cpp_namespace_attributes to NOT be defined.
#endif
#endif

#if _HAS_CXX17 || !defined(__clang__) // C1XX implemented this C++17 feature unconditionally.
#ifndef __cpp_enumerator_attributes
#error Expected __cpp_enumerator_attributes to be defined.
#elif __cpp_enumerator_attributes != 201411L
#error Expected cpp_enumerator_attributes to equal 201411L.
#else
STATIC_ASSERT(__cpp_enumerator_attributes == 201411L);
#endif
#else
#ifdef __cpp_enumerator_attributes
#error Expected __cpp_enumerator_attributes to NOT be defined.
#endif
#endif

#if _HAS_CXX17 || defined(__EDG__) // EDG unconditionally reports "if constexpr" as being available.
#ifndef __cpp_if_constexpr
#error Expected __cpp_if_constexpr to be defined.
#elif __cpp_if_constexpr != 201606L
#error Expected cpp_if_constexpr to equal 201606L.
#else
STATIC_ASSERT(__cpp_if_constexpr == 201606L);
#endif
#else
#ifdef __cpp_if_constexpr
#error Expected __cpp_if_constexpr to NOT be defined.
#endif
#endif

#if _HAS_CXX17 || defined(__EDG__) // EDG unconditionally reports hex floats as being available.
#ifndef __cpp_hex_float
#error Expected __cpp_hex_float to be defined.
#elif __cpp_hex_float != 201603L
#error Expected cpp_hex_float to equal 201603L.
#else
STATIC_ASSERT(__cpp_hex_float == 201603L);
#endif
#else
#ifdef __cpp_hex_float
#error Expected __cpp_hex_float to NOT be defined.
#endif
#endif

#if _HAS_CXX20 && !defined(__clang__)
// Clang only has partial support for <=> but that does not include the feature test macro.
#ifndef __cpp_impl_three_way_comparison
#error Expected __cpp_impl_three_way_comparison to be defined.
#elif defined(__EDG__) // EDG does not yet implement P1630R1 or P1186R3 so they still report the old value.
#if __cpp_impl_three_way_comparison != 201711L
#error Expected cpp_impl_three_way_comparison to equal 201711L.
#else
STATIC_ASSERT(__cpp_impl_three_way_comparison == 201711L);
#endif
#else
#if __cpp_impl_three_way_comparison != 201907L
#error Expected cpp_impl_three_way_comparison to equal 201907L.
#else
STATIC_ASSERT(__cpp_impl_three_way_comparison == 201907L);
#endif
#endif
#else
#ifdef __cpp_impl_three_way_comparison
#error Expected __cpp_impl_three_way_comparison to NOT be defined.
#endif
#endif

#if _HAS_CXX20 && !defined(__clang__) && !defined(__EDG__)
#ifndef __cpp_nontype_template_args
#error Expected __cpp_nontype_template_args to be defined.
#elif __cpp_nontype_template_args != 201911L
#error Expected cpp_nontype_template_args to equal 201911L.
#else
STATIC_ASSERT(__cpp_nontype_template_args == 201911L);
#endif
#elif _HAS_CXX17
#ifndef __cpp_nontype_template_args
#error Expected __cpp_nontype_template_args to be defined.
#elif __cpp_nontype_template_args != 201411L
#error Expected cpp_nontype_template_args to equal 201411L.
#else
STATIC_ASSERT(__cpp_nontype_template_args == 201411L);
#endif
#else
#ifdef __cpp_nontype_template_args
#error Expected __cpp_nontype_template_args to NOT be defined.
#endif
#endif

#if _HAS_CXX20 || defined(__EDG__)
// EDG unconditionally reports "explicit(bool)" as being available.
#ifndef __cpp_conditional_explicit
#error Expected __cpp_conditional_explicit to be defined.
#elif __cpp_conditional_explicit != 201806L
#error Expected cpp_conditional_explicit to equal 201806L.
#else
STATIC_ASSERT(__cpp_conditional_explicit == 201806L);
#endif
#else
#ifdef __cpp_conditional_explicit
#error Expected __cpp_conditional_explicit to NOT be defined.
#endif
#endif

// LIBRARY FEATURE-TEST MACROS
#include <version>

#ifndef __cpp_lib_addressof_constexpr
#error Expected __cpp_lib_addressof_constexpr to be defined.
#elif __cpp_lib_addressof_constexpr != 201603L
#error Expected cpp_lib_addressof_constexpr to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_addressof_constexpr == 201603L);
#endif

#ifndef __cpp_lib_allocator_traits_is_always_equal
#error Expected __cpp_lib_allocator_traits_is_always_equal to be defined.
#elif __cpp_lib_allocator_traits_is_always_equal != 201411L
#error Expected cpp_lib_allocator_traits_is_always_equal to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_allocator_traits_is_always_equal == 201411L);
#endif

#ifndef __cpp_lib_as_const
#error Expected __cpp_lib_as_const to be defined.
#elif __cpp_lib_as_const != 201510L
#error Expected cpp_lib_as_const to equal 201510L.
#else
STATIC_ASSERT(__cpp_lib_as_const == 201510L);
#endif

#ifndef __cpp_lib_atomic_value_initialization
#error Expected __cpp_lib_atomic_value_initialization to be defined.
#elif __cpp_lib_atomic_value_initialization != 201911L
#error Expected cpp_lib_atomic_value_initialization to equal 201911L.
#else
STATIC_ASSERT(__cpp_lib_atomic_value_initialization == 201911L);
#endif

#ifndef __cpp_lib_bool_constant
#error Expected __cpp_lib_bool_constant to be defined.
#elif __cpp_lib_bool_constant != 201505L
#error Expected cpp_lib_bool_constant to equal 201505L.
#else
STATIC_ASSERT(__cpp_lib_bool_constant == 201505L);
#endif

#ifndef __cpp_lib_chrono
#error Expected __cpp_lib_chrono to be defined.
#elif _HAS_CXX17
#if __cpp_lib_chrono != 201611L
#error Expected cpp_lib_chrono to equal 201611L.
#else
STATIC_ASSERT(__cpp_lib_chrono == 201611L);
#endif
#else
#if __cpp_lib_chrono != 201510L
#error Expected cpp_lib_chrono to equal 201510L.
#else
STATIC_ASSERT(__cpp_lib_chrono == 201510L);
#endif
#endif

#ifndef __cpp_lib_chrono_udls
#error Expected __cpp_lib_chrono_udls to be defined.
#elif __cpp_lib_chrono_udls != 201304L
#error Expected cpp_lib_chrono_udls to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_chrono_udls == 201304L);
#endif

#ifndef __cpp_lib_complex_udls
#error Expected __cpp_lib_complex_udls to be defined.
#elif __cpp_lib_complex_udls != 201309L
#error Expected cpp_lib_complex_udls to equal 201309L.
#else
STATIC_ASSERT(__cpp_lib_complex_udls == 201309L);
#endif

#ifndef __cpp_lib_enable_shared_from_this
#error Expected __cpp_lib_enable_shared_from_this to be defined.
#elif __cpp_lib_enable_shared_from_this != 201603L
#error Expected cpp_lib_enable_shared_from_this to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_enable_shared_from_this == 201603L);
#endif

#ifndef __cpp_lib_exchange_function
#error Expected __cpp_lib_exchange_function to be defined.
#elif __cpp_lib_exchange_function != 201304L
#error Expected cpp_lib_exchange_function to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_exchange_function == 201304L);
#endif

#ifndef __cpp_lib_experimental_erase_if
#error Expected __cpp_lib_experimental_erase_if to be defined.
#elif __cpp_lib_experimental_erase_if != 201411L
#error Expected cpp_lib_experimental_erase_if to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_experimental_erase_if == 201411L);
#endif

#ifndef __cpp_lib_experimental_filesystem
#error Expected __cpp_lib_experimental_filesystem to be defined.
#elif __cpp_lib_experimental_filesystem != 201406L
#error Expected cpp_lib_experimental_filesystem to equal 201406L.
#else
STATIC_ASSERT(__cpp_lib_experimental_filesystem == 201406L);
#endif

#ifndef __cpp_lib_generic_associative_lookup
#error Expected __cpp_lib_generic_associative_lookup to be defined.
#elif __cpp_lib_generic_associative_lookup != 201304L
#error Expected cpp_lib_generic_associative_lookup to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_generic_associative_lookup == 201304L);
#endif

#ifndef __cpp_lib_incomplete_container_elements
#error Expected __cpp_lib_incomplete_container_elements to be defined.
#elif __cpp_lib_incomplete_container_elements != 201505L
#error Expected cpp_lib_incomplete_container_elements to equal 201505L.
#else
STATIC_ASSERT(__cpp_lib_incomplete_container_elements == 201505L);
#endif

#ifndef __cpp_lib_integer_sequence
#error Expected __cpp_lib_integer_sequence to be defined.
#elif __cpp_lib_integer_sequence != 201304L
#error Expected cpp_lib_integer_sequence to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_integer_sequence == 201304L);
#endif

#ifndef __cpp_lib_integral_constant_callable
#error Expected __cpp_lib_integral_constant_callable to be defined.
#elif __cpp_lib_integral_constant_callable != 201304L
#error Expected cpp_lib_integral_constant_callable to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_integral_constant_callable == 201304L);
#endif

#ifndef __cpp_lib_invoke
#error Expected __cpp_lib_invoke to be defined.
#elif __cpp_lib_invoke != 201411L
#error Expected cpp_lib_invoke to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_invoke == 201411L);
#endif

#ifndef __cpp_lib_is_final
#error Expected __cpp_lib_is_final to be defined.
#elif __cpp_lib_is_final != 201402L
#error Expected cpp_lib_is_final to equal 201402L.
#else
STATIC_ASSERT(__cpp_lib_is_final == 201402L);
#endif

#ifndef __cpp_lib_is_null_pointer
#error Expected __cpp_lib_is_null_pointer to be defined.
#elif __cpp_lib_is_null_pointer != 201309L
#error Expected cpp_lib_is_null_pointer to equal 201309L.
#else
STATIC_ASSERT(__cpp_lib_is_null_pointer == 201309L);
#endif

#ifndef __cpp_lib_logical_traits
#error Expected __cpp_lib_logical_traits to be defined.
#elif __cpp_lib_logical_traits != 201510L
#error Expected cpp_lib_logical_traits to equal 201510L.
#else
STATIC_ASSERT(__cpp_lib_logical_traits == 201510L);
#endif

#ifndef __cpp_lib_make_reverse_iterator
#error Expected __cpp_lib_make_reverse_iterator to be defined.
#elif __cpp_lib_make_reverse_iterator != 201402L
#error Expected cpp_lib_make_reverse_iterator to equal 201402L.
#else
STATIC_ASSERT(__cpp_lib_make_reverse_iterator == 201402L);
#endif

#ifndef __cpp_lib_make_unique
#error Expected __cpp_lib_make_unique to be defined.
#elif __cpp_lib_make_unique != 201304L
#error Expected cpp_lib_make_unique to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_make_unique == 201304L);
#endif

#ifndef __cpp_lib_map_try_emplace
#error Expected __cpp_lib_map_try_emplace to be defined.
#elif __cpp_lib_map_try_emplace != 201411L
#error Expected cpp_lib_map_try_emplace to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_map_try_emplace == 201411L);
#endif

#ifndef __cpp_lib_nonmember_container_access
#error Expected __cpp_lib_nonmember_container_access to be defined.
#elif __cpp_lib_nonmember_container_access != 201411L
#error Expected cpp_lib_nonmember_container_access to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_nonmember_container_access == 201411L);
#endif

#ifndef __cpp_lib_null_iterators
#error Expected __cpp_lib_null_iterators to be defined.
#elif __cpp_lib_null_iterators != 201304L
#error Expected cpp_lib_null_iterators to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_null_iterators == 201304L);
#endif

#ifndef __cpp_lib_quoted_string_io
#error Expected __cpp_lib_quoted_string_io to be defined.
#elif __cpp_lib_quoted_string_io != 201304L
#error Expected cpp_lib_quoted_string_io to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_quoted_string_io == 201304L);
#endif

#ifndef __cpp_lib_result_of_sfinae
#error Expected __cpp_lib_result_of_sfinae to be defined.
#elif __cpp_lib_result_of_sfinae != 201210L
#error Expected cpp_lib_result_of_sfinae to equal 201210L.
#else
STATIC_ASSERT(__cpp_lib_result_of_sfinae == 201210L);
#endif

#ifndef __cpp_lib_robust_nonmodifying_seq_ops
#error Expected __cpp_lib_robust_nonmodifying_seq_ops to be defined.
#elif __cpp_lib_robust_nonmodifying_seq_ops != 201304L
#error Expected cpp_lib_robust_nonmodifying_seq_ops to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_robust_nonmodifying_seq_ops == 201304L);
#endif

#ifndef __cpp_lib_shared_mutex
#error Expected __cpp_lib_shared_mutex to be defined.
#elif __cpp_lib_shared_mutex != 201505L
#error Expected cpp_lib_shared_mutex to equal 201505L.
#else
STATIC_ASSERT(__cpp_lib_shared_mutex == 201505L);
#endif

#ifndef __cpp_lib_shared_ptr_arrays
#error Expected __cpp_lib_shared_ptr_arrays to be defined.
#elif __cpp_lib_shared_ptr_arrays != 201611L
#error Expected cpp_lib_shared_ptr_arrays to equal 201611L.
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_arrays == 201611L);
#endif

#ifndef __cpp_lib_string_udls
#error Expected __cpp_lib_string_udls to be defined.
#elif __cpp_lib_string_udls != 201304L
#error Expected cpp_lib_string_udls to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_string_udls == 201304L);
#endif

#ifndef __cpp_lib_transformation_trait_aliases
#error Expected __cpp_lib_transformation_trait_aliases to be defined.
#elif __cpp_lib_transformation_trait_aliases != 201304L
#error Expected cpp_lib_transformation_trait_aliases to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_transformation_trait_aliases == 201304L);
#endif

#ifndef __cpp_lib_transparent_operators
#error Expected __cpp_lib_transparent_operators to be defined.
#elif __cpp_lib_transparent_operators != 201510L
#error Expected cpp_lib_transparent_operators to equal 201510L.
#else
STATIC_ASSERT(__cpp_lib_transparent_operators == 201510L);
#endif

#ifndef __cpp_lib_tuple_element_t
#error Expected __cpp_lib_tuple_element_t to be defined.
#elif __cpp_lib_tuple_element_t != 201402L
#error Expected cpp_lib_tuple_element_t to equal 201402L.
#else
STATIC_ASSERT(__cpp_lib_tuple_element_t == 201402L);
#endif

#ifndef __cpp_lib_tuples_by_type
#error Expected __cpp_lib_tuples_by_type to be defined.
#elif __cpp_lib_tuples_by_type != 201304L
#error Expected cpp_lib_tuples_by_type to equal 201304L.
#else
STATIC_ASSERT(__cpp_lib_tuples_by_type == 201304L);
#endif

#ifndef __cpp_lib_type_trait_variable_templates
#error Expected __cpp_lib_type_trait_variable_templates to be defined.
#elif __cpp_lib_type_trait_variable_templates != 201510L
#error Expected cpp_lib_type_trait_variable_templates to equal 201510L.
#else
STATIC_ASSERT(__cpp_lib_type_trait_variable_templates == 201510L);
#endif

#ifndef __cpp_lib_uncaught_exceptions
#error Expected __cpp_lib_uncaught_exceptions to be defined.
#elif __cpp_lib_uncaught_exceptions != 201411L
#error Expected cpp_lib_uncaught_exceptions to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_uncaught_exceptions == 201411L);
#endif

#ifndef __cpp_lib_unordered_map_try_emplace
#error Expected __cpp_lib_unordered_map_try_emplace to be defined.
#elif __cpp_lib_unordered_map_try_emplace != 201411L
#error Expected cpp_lib_unordered_map_try_emplace to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_unordered_map_try_emplace == 201411L);
#endif

#ifndef __cpp_lib_void_t
#error Expected __cpp_lib_void_t to be defined.
#elif __cpp_lib_void_t != 201411L
#error Expected cpp_lib_void_t to equal 201411L.
#else
STATIC_ASSERT(__cpp_lib_void_t == 201411L);
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_any
#error Expected __cpp_lib_any to be defined.
#elif __cpp_lib_any != 201606L
#error Expected cpp_lib_any to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_any == 201606L);
#endif
#else
#ifdef __cpp_lib_any
#error Expected __cpp_lib_any to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_apply
#error Expected __cpp_lib_apply to be defined.
#elif __cpp_lib_apply != 201603L
#error Expected cpp_lib_apply to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_apply == 201603L);
#endif
#else
#ifdef __cpp_lib_apply
#error Expected __cpp_lib_apply to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_array_constexpr
#error Expected __cpp_lib_array_constexpr to be defined.
#elif __cpp_lib_array_constexpr != 201803L
#error Expected cpp_lib_array_constexpr to equal 201803L.
#else
STATIC_ASSERT(__cpp_lib_array_constexpr == 201803L);
#endif
#else
#ifdef __cpp_lib_array_constexpr
#error Expected __cpp_lib_array_constexpr to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_atomic_is_always_lock_free
#error Expected __cpp_lib_atomic_is_always_lock_free to be defined.
#elif __cpp_lib_atomic_is_always_lock_free != 201603L
#error Expected cpp_lib_atomic_is_always_lock_free to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_atomic_is_always_lock_free == 201603L);
#endif
#else
#ifdef __cpp_lib_atomic_is_always_lock_free
#error Expected __cpp_lib_atomic_is_always_lock_free to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_boyer_moore_searcher
#error Expected __cpp_lib_boyer_moore_searcher to be defined.
#elif __cpp_lib_boyer_moore_searcher != 201603L
#error Expected cpp_lib_boyer_moore_searcher to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_boyer_moore_searcher == 201603L);
#endif
#else
#ifdef __cpp_lib_boyer_moore_searcher
#error Expected __cpp_lib_boyer_moore_searcher to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_clamp
#error Expected __cpp_lib_clamp to be defined.
#elif __cpp_lib_clamp != 201603L
#error Expected cpp_lib_clamp to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_clamp == 201603L);
#endif
#else
#ifdef __cpp_lib_clamp
#error Expected __cpp_lib_clamp to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_filesystem
#error Expected __cpp_lib_filesystem to be defined.
#elif __cpp_lib_filesystem != 201703L
#error Expected cpp_lib_filesystem to equal 201703L.
#else
STATIC_ASSERT(__cpp_lib_filesystem == 201703L);
#endif
#else
#ifdef __cpp_lib_filesystem
#error Expected __cpp_lib_filesystem to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_gcd_lcm
#error Expected __cpp_lib_gcd_lcm to be defined.
#elif __cpp_lib_gcd_lcm != 201606L
#error Expected cpp_lib_gcd_lcm to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_gcd_lcm == 201606L);
#endif
#else
#ifdef __cpp_lib_gcd_lcm
#error Expected __cpp_lib_gcd_lcm to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_hardware_interference_size
#error Expected __cpp_lib_hardware_interference_size to be defined.
#elif __cpp_lib_hardware_interference_size != 201703L
#error Expected cpp_lib_hardware_interference_size to equal 201703L.
#else
STATIC_ASSERT(__cpp_lib_hardware_interference_size == 201703L);
#endif
#else
#ifdef __cpp_lib_hardware_interference_size
#error Expected __cpp_lib_hardware_interference_size to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_has_unique_object_representations
#error Expected __cpp_lib_has_unique_object_representations to be defined.
#elif __cpp_lib_has_unique_object_representations != 201606L
#error Expected cpp_lib_has_unique_object_representations to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_has_unique_object_representations == 201606L);
#endif
#else
#ifdef __cpp_lib_has_unique_object_representations
#error Expected __cpp_lib_has_unique_object_representations to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_hypot
#error Expected __cpp_lib_hypot to be defined.
#elif __cpp_lib_hypot != 201603L
#error Expected cpp_lib_hypot to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_hypot == 201603L);
#endif
#else
#ifdef __cpp_lib_hypot
#error Expected __cpp_lib_hypot to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_aggregate
#error Expected __cpp_lib_is_aggregate to be defined.
#elif __cpp_lib_is_aggregate != 201703L
#error Expected cpp_lib_is_aggregate to equal 201703L.
#else
STATIC_ASSERT(__cpp_lib_is_aggregate == 201703L);
#endif
#else
#ifdef __cpp_lib_is_aggregate
#error Expected __cpp_lib_is_aggregate to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_invocable
#error Expected __cpp_lib_is_invocable to be defined.
#elif __cpp_lib_is_invocable != 201703L
#error Expected cpp_lib_is_invocable to equal 201703L.
#else
STATIC_ASSERT(__cpp_lib_is_invocable == 201703L);
#endif
#else
#ifdef __cpp_lib_is_invocable
#error Expected __cpp_lib_is_invocable to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_is_swappable
#error Expected __cpp_lib_is_swappable to be defined.
#elif __cpp_lib_is_swappable != 201603L
#error Expected cpp_lib_is_swappable to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_is_swappable == 201603L);
#endif
#else
#ifdef __cpp_lib_is_swappable
#error Expected __cpp_lib_is_swappable to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_launder
#error Expected __cpp_lib_launder to be defined.
#elif __cpp_lib_launder != 201606L
#error Expected cpp_lib_launder to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_launder == 201606L);
#endif
#else
#ifdef __cpp_lib_launder
#error Expected __cpp_lib_launder to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_make_from_tuple
#error Expected __cpp_lib_make_from_tuple to be defined.
#elif __cpp_lib_make_from_tuple != 201606L
#error Expected cpp_lib_make_from_tuple to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_make_from_tuple == 201606L);
#endif
#else
#ifdef __cpp_lib_make_from_tuple
#error Expected __cpp_lib_make_from_tuple to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_math_special_functions
#error Expected __cpp_lib_math_special_functions to be defined.
#elif __cpp_lib_math_special_functions != 201603L
#error Expected cpp_lib_math_special_functions to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_math_special_functions == 201603L);
#endif
#else
#ifdef __cpp_lib_math_special_functions
#error Expected __cpp_lib_math_special_functions to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_memory_resource
#error Expected __cpp_lib_memory_resource to be defined.
#elif __cpp_lib_memory_resource != 201603L
#error Expected cpp_lib_memory_resource to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_memory_resource == 201603L);
#endif
#else
#ifdef __cpp_lib_memory_resource
#error Expected __cpp_lib_memory_resource to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_node_extract
#error Expected __cpp_lib_node_extract to be defined.
#elif __cpp_lib_node_extract != 201606L
#error Expected cpp_lib_node_extract to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_node_extract == 201606L);
#endif
#else
#ifdef __cpp_lib_node_extract
#error Expected __cpp_lib_node_extract to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_not_fn
#error Expected __cpp_lib_not_fn to be defined.
#elif __cpp_lib_not_fn != 201603L
#error Expected cpp_lib_not_fn to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_not_fn == 201603L);
#endif
#else
#ifdef __cpp_lib_not_fn
#error Expected __cpp_lib_not_fn to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_optional
#error Expected __cpp_lib_optional to be defined.
#elif __cpp_lib_optional != 201606L
#error Expected cpp_lib_optional to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_optional == 201606L);
#endif
#else
#ifdef __cpp_lib_optional
#error Expected __cpp_lib_optional to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_raw_memory_algorithms
#error Expected __cpp_lib_raw_memory_algorithms to be defined.
#elif __cpp_lib_raw_memory_algorithms != 201606L
#error Expected cpp_lib_raw_memory_algorithms to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_raw_memory_algorithms == 201606L);
#endif
#else
#ifdef __cpp_lib_raw_memory_algorithms
#error Expected __cpp_lib_raw_memory_algorithms to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_sample
#error Expected __cpp_lib_sample to be defined.
#elif __cpp_lib_sample != 201603L
#error Expected cpp_lib_sample to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_sample == 201603L);
#endif
#else
#ifdef __cpp_lib_sample
#error Expected __cpp_lib_sample to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_scoped_lock
#error Expected __cpp_lib_scoped_lock to be defined.
#elif __cpp_lib_scoped_lock != 201703L
#error Expected cpp_lib_scoped_lock to equal 201703L.
#else
STATIC_ASSERT(__cpp_lib_scoped_lock == 201703L);
#endif
#else
#ifdef __cpp_lib_scoped_lock
#error Expected __cpp_lib_scoped_lock to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_shared_ptr_weak_type
#error Expected __cpp_lib_shared_ptr_weak_type to be defined.
#elif __cpp_lib_shared_ptr_weak_type != 201606L
#error Expected cpp_lib_shared_ptr_weak_type to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_weak_type == 201606L);
#endif
#else
#ifdef __cpp_lib_shared_ptr_weak_type
#error Expected __cpp_lib_shared_ptr_weak_type to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_string_view
#error Expected __cpp_lib_string_view to be defined.
#elif __cpp_lib_string_view != 201803L
#error Expected cpp_lib_string_view to equal 201803L.
#else
STATIC_ASSERT(__cpp_lib_string_view == 201803L);
#endif
#else
#ifdef __cpp_lib_string_view
#error Expected __cpp_lib_string_view to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_to_chars
#error Expected __cpp_lib_to_chars to be defined.
#elif __cpp_lib_to_chars != 201611L
#error Expected cpp_lib_to_chars to equal 201611L.
#else
STATIC_ASSERT(__cpp_lib_to_chars == 201611L);
#endif
#else
#ifdef __cpp_lib_to_chars
#error Expected __cpp_lib_to_chars to NOT be defined.
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_lib_variant
#error Expected __cpp_lib_variant to be defined.
#elif __cpp_lib_variant != 201606L
#error Expected cpp_lib_variant to equal 201606L.
#else
STATIC_ASSERT(__cpp_lib_variant == 201606L);
#endif
#else
#ifdef __cpp_lib_variant
#error Expected __cpp_lib_variant to NOT be defined.
#endif
#endif

#if _HAS_CXX17 && !defined(_M_CEE)
#ifndef __cpp_lib_execution
#error Expected __cpp_lib_execution to be defined.
#elif __cpp_lib_execution != 201603L
#error Expected cpp_lib_execution to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_execution == 201603L);
#endif
#else
#ifdef __cpp_lib_execution
#error Expected __cpp_lib_execution to NOT be defined.
#endif
#endif

#if _HAS_CXX17 && !defined(_M_CEE)
#ifndef __cpp_lib_parallel_algorithm
#error Expected __cpp_lib_parallel_algorithm to be defined.
#elif __cpp_lib_parallel_algorithm != 201603L
#error Expected cpp_lib_parallel_algorithm to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_parallel_algorithm == 201603L);
#endif
#else
#ifdef __cpp_lib_parallel_algorithm
#error Expected __cpp_lib_parallel_algorithm to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_atomic_float
#error Expected __cpp_lib_atomic_float to be defined.
#elif __cpp_lib_atomic_float != 201711L
#error Expected cpp_lib_atomic_float to equal 201711L.
#else
STATIC_ASSERT(__cpp_lib_atomic_float == 201711L);
#endif
#else
#ifdef __cpp_lib_atomic_float
#error Expected __cpp_lib_atomic_float to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bind_front
#error Expected __cpp_lib_bind_front to be defined.
#elif __cpp_lib_bind_front != 201907L
#error Expected cpp_lib_bind_front to equal 201907L.
#else
STATIC_ASSERT(__cpp_lib_bind_front == 201907L);
#endif
#else
#ifdef __cpp_lib_bind_front
#error Expected __cpp_lib_bind_front to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_bounded_array_traits
#error Expected __cpp_lib_bounded_array_traits to be defined.
#elif __cpp_lib_bounded_array_traits != 201902L
#error Expected cpp_lib_bounded_array_traits to equal 201902L.
#else
STATIC_ASSERT(__cpp_lib_bounded_array_traits == 201902L);
#endif
#else
#ifdef __cpp_lib_bounded_array_traits
#error Expected __cpp_lib_bounded_array_traits to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_algorithms
#error Expected __cpp_lib_constexpr_algorithms to be defined.
#elif __cpp_lib_constexpr_algorithms != 201806L
#error Expected cpp_lib_constexpr_algorithms to equal 201806L.
#else
STATIC_ASSERT(__cpp_lib_constexpr_algorithms == 201806L);
#endif
#else
#ifdef __cpp_lib_constexpr_algorithms
#error Expected __cpp_lib_constexpr_algorithms to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_memory
#error Expected __cpp_lib_constexpr_memory to be defined.
#elif __cpp_lib_constexpr_memory != 201811L
#error Expected cpp_lib_constexpr_memory to equal 201811L.
#else
STATIC_ASSERT(__cpp_lib_constexpr_memory == 201811L);
#endif
#else
#ifdef __cpp_lib_constexpr_memory
#error Expected __cpp_lib_constexpr_memory to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_constexpr_numeric
#error Expected __cpp_lib_constexpr_numeric to be defined.
#elif __cpp_lib_constexpr_numeric != 201911L
#error Expected cpp_lib_constexpr_numeric to equal 201911L.
#else
STATIC_ASSERT(__cpp_lib_constexpr_numeric == 201911L);
#endif
#else
#ifdef __cpp_lib_constexpr_numeric
#error Expected __cpp_lib_constexpr_numeric to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_endian
#error Expected __cpp_lib_endian to be defined.
#elif __cpp_lib_endian != 201907L
#error Expected cpp_lib_endian to equal 201907L.
#else
STATIC_ASSERT(__cpp_lib_endian == 201907L);
#endif
#else
#ifdef __cpp_lib_endian
#error Expected __cpp_lib_endian to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_erase_if
#error Expected __cpp_lib_erase_if to be defined.
#elif __cpp_lib_erase_if != 202002L
#error Expected cpp_lib_erase_if to equal 202002L.
#else
STATIC_ASSERT(__cpp_lib_erase_if == 202002L);
#endif
#else
#ifdef __cpp_lib_erase_if
#error Expected __cpp_lib_erase_if to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_generic_unordered_lookup
#error Expected __cpp_lib_generic_unordered_lookup to be defined.
#elif __cpp_lib_generic_unordered_lookup != 201811L
#error Expected cpp_lib_generic_unordered_lookup to equal 201811L.
#else
STATIC_ASSERT(__cpp_lib_generic_unordered_lookup == 201811L);
#endif
#else
#ifdef __cpp_lib_generic_unordered_lookup
#error Expected __cpp_lib_generic_unordered_lookup to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_int_pow2
#error Expected __cpp_lib_int_pow2 to be defined.
#elif __cpp_lib_int_pow2 != 202002L
#error Expected cpp_lib_int_pow2 to equal 202002L.
#else
STATIC_ASSERT(__cpp_lib_int_pow2 == 202002L);
#endif
#else
#ifdef __cpp_lib_int_pow2
#error Expected __cpp_lib_int_pow2 to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_is_constant_evaluated
#error Expected __cpp_lib_is_constant_evaluated to be defined.
#elif __cpp_lib_is_constant_evaluated != 201811L
#error Expected cpp_lib_is_constant_evaluated to equal 201811L.
#else
STATIC_ASSERT(__cpp_lib_is_constant_evaluated == 201811L);
#endif
#else
#ifdef __cpp_lib_is_constant_evaluated
#error Expected __cpp_lib_is_constant_evaluated to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_is_nothrow_convertible
#error Expected __cpp_lib_is_nothrow_convertible to be defined.
#elif __cpp_lib_is_nothrow_convertible != 201806L
#error Expected cpp_lib_is_nothrow_convertible to equal 201806L.
#else
STATIC_ASSERT(__cpp_lib_is_nothrow_convertible == 201806L);
#endif
#else
#ifdef __cpp_lib_is_nothrow_convertible
#error Expected __cpp_lib_is_nothrow_convertible to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_list_remove_return_type
#error Expected __cpp_lib_list_remove_return_type to be defined.
#elif __cpp_lib_list_remove_return_type != 201806L
#error Expected cpp_lib_list_remove_return_type to equal 201806L.
#else
STATIC_ASSERT(__cpp_lib_list_remove_return_type == 201806L);
#endif
#else
#ifdef __cpp_lib_list_remove_return_type
#error Expected __cpp_lib_list_remove_return_type to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_math_constants
#error Expected __cpp_lib_math_constants to be defined.
#elif __cpp_lib_math_constants != 201907L
#error Expected cpp_lib_math_constants to equal 201907L.
#else
STATIC_ASSERT(__cpp_lib_math_constants == 201907L);
#endif
#else
#ifdef __cpp_lib_math_constants
#error Expected __cpp_lib_math_constants to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_remove_cvref
#error Expected __cpp_lib_remove_cvref to be defined.
#elif __cpp_lib_remove_cvref != 201711L
#error Expected cpp_lib_remove_cvref to equal 201711L.
#else
STATIC_ASSERT(__cpp_lib_remove_cvref == 201711L);
#endif
#else
#ifdef __cpp_lib_remove_cvref
#error Expected __cpp_lib_remove_cvref to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_shift
#error Expected __cpp_lib_shift to be defined.
#elif __cpp_lib_shift != 201806L
#error Expected cpp_lib_shift to equal 201806L.
#else
STATIC_ASSERT(__cpp_lib_shift == 201806L);
#endif
#else
#ifdef __cpp_lib_shift
#error Expected __cpp_lib_shift to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_span
#error Expected __cpp_lib_span to be defined.
#elif __cpp_lib_span != 202002L
#error Expected cpp_lib_span to equal 202002L.
#else
STATIC_ASSERT(__cpp_lib_span == 202002L);
#endif
#else
#ifdef __cpp_lib_span
#error Expected __cpp_lib_span to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_ssize
#error Expected __cpp_lib_ssize to be defined.
#elif __cpp_lib_ssize != 201902L
#error Expected cpp_lib_ssize to equal 201902L.
#else
STATIC_ASSERT(__cpp_lib_ssize == 201902L);
#endif
#else
#ifdef __cpp_lib_ssize
#error Expected __cpp_lib_ssize to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_starts_ends_with
#error Expected __cpp_lib_starts_ends_with to be defined.
#elif __cpp_lib_starts_ends_with != 201711L
#error Expected cpp_lib_starts_ends_with to equal 201711L.
#else
STATIC_ASSERT(__cpp_lib_starts_ends_with == 201711L);
#endif
#else
#ifdef __cpp_lib_starts_ends_with
#error Expected __cpp_lib_starts_ends_with to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_to_address
#error Expected __cpp_lib_to_address to be defined.
#elif __cpp_lib_to_address != 201711L
#error Expected cpp_lib_to_address to equal 201711L.
#else
STATIC_ASSERT(__cpp_lib_to_address == 201711L);
#endif
#else
#ifdef __cpp_lib_to_address
#error Expected __cpp_lib_to_address to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_to_array
#error Expected __cpp_lib_to_array to be defined.
#elif __cpp_lib_to_array != 201907L
#error Expected cpp_lib_to_array to equal 201907L.
#else
STATIC_ASSERT(__cpp_lib_to_array == 201907L);
#endif
#else
#ifdef __cpp_lib_to_array
#error Expected __cpp_lib_to_array to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_type_identity
#error Expected __cpp_lib_type_identity to be defined.
#elif __cpp_lib_type_identity != 201806L
#error Expected cpp_lib_type_identity to equal 201806L.
#else
STATIC_ASSERT(__cpp_lib_type_identity == 201806L);
#endif
#else
#ifdef __cpp_lib_type_identity
#error Expected __cpp_lib_type_identity to NOT be defined.
#endif
#endif

#if _HAS_CXX20
#ifndef __cpp_lib_unwrap_ref
#error Expected __cpp_lib_unwrap_ref to be defined.
#elif __cpp_lib_unwrap_ref != 201811L
#error Expected cpp_lib_unwrap_ref to equal 201811L.
#else
STATIC_ASSERT(__cpp_lib_unwrap_ref == 201811L);
#endif
#else
#ifdef __cpp_lib_unwrap_ref
#error Expected __cpp_lib_unwrap_ref to NOT be defined.
#endif
#endif

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, VSO-1041044
#ifndef __cpp_lib_bit_cast
#error Expected __cpp_lib_bit_cast to be defined.
#elif __cpp_lib_bit_cast != 201806L
#error Expected cpp_lib_bit_cast to equal 201806L.
#else
STATIC_ASSERT(__cpp_lib_bit_cast == 201806L);
#endif
#else
#ifdef __cpp_lib_bit_cast
#error Expected __cpp_lib_bit_cast to NOT be defined.
#endif
#endif

#if _HAS_CXX20 && (defined(__clang__) || defined(__EDG__)) // TRANSITION, VSO-1020212
#ifndef __cpp_lib_bitops
#error Expected __cpp_lib_bitops to be defined.
#elif __cpp_lib_bitops != 201907L
#error Expected cpp_lib_bitops to equal 201907L.
#else
STATIC_ASSERT(__cpp_lib_bitops == 201907L);
#endif
#else
#ifdef __cpp_lib_bitops
#error Expected __cpp_lib_bitops to NOT be defined.
#endif
#endif

#if _HAS_CXX20 && defined(__cpp_char8_t)
#ifndef __cpp_lib_char8_t
#error Expected __cpp_lib_char8_t to be defined.
#elif __cpp_lib_char8_t != 201907L
#error Expected cpp_lib_char8_t to equal 201907L.
#else
STATIC_ASSERT(__cpp_lib_char8_t == 201907L);
#endif
#else
#ifdef __cpp_lib_char8_t
#error Expected __cpp_lib_char8_t to NOT be defined.
#endif
#endif

#if _HAS_CXX20 && defined(__cpp_concepts)
#ifndef __cpp_lib_concepts
#error Expected __cpp_lib_concepts to be defined.
#elif __cpp_lib_concepts != 201907L
#error Expected cpp_lib_concepts to equal 201907L.
#else
STATIC_ASSERT(__cpp_lib_concepts == 201907L);
#endif
#else
#ifdef __cpp_lib_concepts
#error Expected __cpp_lib_concepts to NOT be defined.
#endif
#endif

#if _HAS_STD_BYTE
#ifndef __cpp_lib_byte
#error Expected __cpp_lib_byte to be defined.
#elif __cpp_lib_byte != 201603L
#error Expected cpp_lib_byte to equal 201603L.
#else
STATIC_ASSERT(__cpp_lib_byte == 201603L);
#endif
#else
#ifdef __cpp_lib_byte
#error Expected __cpp_lib_byte to NOT be defined.
#endif
#endif

#ifdef _M_CEE
#ifdef __cpp_lib_shared_timed_mutex
#error Expected __cpp_lib_shared_timed_mutex to NOT be defined.
#endif
#else // ^^^ _M_CEE ^^^ // vvv !_M_CEE vvv
#ifndef __cpp_lib_shared_timed_mutex
#error Expected __cpp_lib_shared_timed_mutex to be defined.
#elif __cpp_lib_shared_timed_mutex != 201402L
#error Expected cpp_lib_shared_timed_mutex to equal 201402L.
#else
STATIC_ASSERT(__cpp_lib_shared_timed_mutex == 201402L);
#endif
#endif // _M_CEE
