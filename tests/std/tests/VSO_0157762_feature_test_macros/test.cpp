// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

int main() {} // COMPILE-ONLY

// ATTRIBUTE FEATURE-TEST MACROS

#ifdef __has_cpp_attribute
// Good
#else
#error __has_cpp_attribute is not defined
#endif
#if defined(__has_cpp_attribute)
// Good
#else
#error __has_cpp_attribute is not defined
#endif
#ifndef __has_cpp_attribute
#error __has_cpp_attribute is not defined
#endif

#if __has_cpp_attribute(carries_dependency) != 200809L
#error __has_cpp_attribute(carries_dependency) is not 200809L
#endif

#if __has_cpp_attribute(deprecated) != 201309L
#error __has_cpp_attribute(deprecated) is not 201309L
#endif

#if __has_cpp_attribute(noreturn) != 200809L
#error __has_cpp_attribute(noreturn) is not 200809L
#endif

#if defined(__clang__) || defined(__EDG__) // clang and EDG don't yet implement P1771R1
#if __has_cpp_attribute(nodiscard) != 201603L
#error __has_cpp_attribute(nodiscard) is not 201603L
#endif
#else
#if __has_cpp_attribute(nodiscard) != 201907L
#error __has_cpp_attribute(nodiscard) is not 201907L
#endif
#endif

#if _HAS_CXX17 || defined(__clang__) || defined(__EDG__) // Clang and EDG provide this in C++14 mode.
#if __has_cpp_attribute(fallthrough) != 201603L
#error __has_cpp_attribute(fallthrough) is not 201603L
#endif
#else
#if __has_cpp_attribute(fallthrough) != 0
#error __has_cpp_attribute(fallthrough) is not 0
#endif
#endif

#if _HAS_CXX17 || defined(__clang__) || defined(__EDG__) // Clang and EDG provide this in C++14 mode.
#if __has_cpp_attribute(maybe_unused) != 201603L
#error __has_cpp_attribute(maybe_unused) is not 201603L
#endif
#else
#if __has_cpp_attribute(maybe_unused) != 0
#error __has_cpp_attribute(maybe_unused) is not 0
#endif
#endif

#if __has_cpp_attribute(noreturn) != 200809L
#error __has_cpp_attribute(noreturn) is not 200809L
#endif


// CORE LANGUAGE FEATURE-TEST MACROS

#ifndef __cpp_aggregate_nsdmi
#error __cpp_aggregate_nsdmi is not defined
#elif __cpp_aggregate_nsdmi != 201304L
#error __cpp_aggregate_nsdmi is not 201304L
#else
STATIC_ASSERT(__cpp_aggregate_nsdmi == 201304L);
#endif

#ifndef __cpp_alias_templates
#error __cpp_alias_templates is not defined
#elif __cpp_alias_templates != 200704L
#error __cpp_alias_templates is not 200704L
#else
STATIC_ASSERT(__cpp_alias_templates == 200704L);
#endif

#ifndef __cpp_attributes
#error __cpp_attributes is not defined
#elif __cpp_attributes != 200809L
#error __cpp_attributes is not 200809L
#else
STATIC_ASSERT(__cpp_attributes == 200809L);
#endif

#ifndef __cpp_binary_literals
#error __cpp_binary_literals is not defined
#elif __cpp_binary_literals != 201304L
#error __cpp_binary_literals is not 201304L
#else
STATIC_ASSERT(__cpp_binary_literals == 201304L);
#endif

#ifndef __cpp_constexpr
#error __cpp_constexpr is not defined
#elif _HAS_CXX17
#if __cpp_constexpr != 201603L
#error __cpp_constexpr is not 201603L
#else
STATIC_ASSERT(__cpp_constexpr == 201603L);
#endif
#else
#if __cpp_constexpr != 201304L
#error __cpp_constexpr is not 201304L
#else
STATIC_ASSERT(__cpp_constexpr == 201304L);
#endif
#endif

#ifndef __cpp_decltype
#error __cpp_decltype is not defined
#elif __cpp_decltype != 200707L
#error __cpp_decltype is not 200707L
#else
STATIC_ASSERT(__cpp_decltype == 200707L);
#endif

#ifndef __cpp_decltype_auto
#error __cpp_decltype_auto is not defined
#elif __cpp_decltype_auto != 201304L
#error __cpp_decltype_auto is not 201304L
#else
STATIC_ASSERT(__cpp_decltype_auto == 201304L);
#endif

#ifndef __cpp_delegating_constructors
#error __cpp_delegating_constructors is not defined
#elif __cpp_delegating_constructors != 200604L
#error __cpp_delegating_constructors is not 200604L
#else
STATIC_ASSERT(__cpp_delegating_constructors == 200604L);
#endif

#ifndef __cpp_generic_lambdas
#error __cpp_generic_lambdas is not defined
#elif __cpp_generic_lambdas != 201304L
#error __cpp_generic_lambdas is not 201304L
#else
STATIC_ASSERT(__cpp_generic_lambdas == 201304L);
#endif

#ifndef __cpp_inheriting_constructors
#error __cpp_inheriting_constructors is not defined
#elif (_HAS_CXX17 || defined(__clang__)) && !defined(__EDG__) // Clang implemented this C++17 feature unconditionally.
                                                              // TRANSITION, VSO-610203
#if __cpp_inheriting_constructors != 201511L
#error __cpp_inheriting_constructors is not 201511L
#else
STATIC_ASSERT(__cpp_inheriting_constructors == 201511L);
#endif
#else
#if __cpp_inheriting_constructors != 200802L
#error __cpp_inheriting_constructors is not 200802L
#else
STATIC_ASSERT(__cpp_inheriting_constructors == 200802L);
#endif
#endif

#ifndef __cpp_init_captures
#error __cpp_init_captures is not defined
#elif __cpp_init_captures != 201304L
#error __cpp_init_captures is not 201304L
#else
STATIC_ASSERT(__cpp_init_captures == 201304L);
#endif

#ifndef __cpp_initializer_lists
#error __cpp_initializer_lists is not defined
#elif __cpp_initializer_lists != 200806L
#error __cpp_initializer_lists is not 200806L
#else
STATIC_ASSERT(__cpp_initializer_lists == 200806L);
#endif

#ifndef __cpp_lambdas
#error __cpp_lambdas is not defined
#elif __cpp_lambdas != 200907L
#error __cpp_lambdas is not 200907L
#else
STATIC_ASSERT(__cpp_lambdas == 200907L);
#endif

#ifndef __cpp_nsdmi
#error __cpp_nsdmi is not defined
#elif __cpp_nsdmi != 200809L
#error __cpp_nsdmi is not 200809L
#else
STATIC_ASSERT(__cpp_nsdmi == 200809L);
#endif

#ifndef __cpp_range_based_for
#error __cpp_range_based_for is not defined
#elif !defined(__clang__) || _HAS_CXX17 // C1XX implemented this C++17 feature unconditionally.
#if __cpp_range_based_for != 201603L
#error __cpp_range_based_for is not 201603L
#else
STATIC_ASSERT(__cpp_range_based_for == 201603L);
#endif
#else
#if __cpp_range_based_for != 200907L
#error __cpp_range_based_for is not 200907L
#else
STATIC_ASSERT(__cpp_range_based_for == 200907L);
#endif
#endif

#ifndef __cpp_raw_strings
#error __cpp_raw_strings is not defined
#elif __cpp_raw_strings != 200710L
#error __cpp_raw_strings is not 200710L
#else
STATIC_ASSERT(__cpp_raw_strings == 200710L);
#endif

#ifndef __cpp_ref_qualifiers
#error __cpp_ref_qualifiers is not defined
#elif __cpp_ref_qualifiers != 200710L
#error __cpp_ref_qualifiers is not 200710L
#else
STATIC_ASSERT(__cpp_ref_qualifiers == 200710L);
#endif

#ifndef __cpp_return_type_deduction
#error __cpp_return_type_deduction is not defined
#elif __cpp_return_type_deduction != 201304L
#error __cpp_return_type_deduction is not 201304L
#else
STATIC_ASSERT(__cpp_return_type_deduction == 201304L);
#endif

#ifndef __cpp_rvalue_references
#error __cpp_rvalue_references is not defined
#elif __cpp_rvalue_references != 200610L
#error __cpp_rvalue_references is not 200610L
#else
STATIC_ASSERT(__cpp_rvalue_references == 200610L);
#endif

#ifndef __cpp_static_assert
#error __cpp_static_assert is not defined
#elif _HAS_CXX17
#if __cpp_static_assert != 201411L
#error __cpp_static_assert is not 201411L
#else
STATIC_ASSERT(__cpp_static_assert == 201411L);
#endif
#else
#if __cpp_static_assert != 200410L
#error __cpp_static_assert is not 200410L
#else
STATIC_ASSERT(__cpp_static_assert == 200410L);
#endif
#endif

#ifndef __cpp_unicode_characters
#error __cpp_unicode_characters is not defined
#elif __cpp_unicode_characters != 200704L
#error __cpp_unicode_characters is not 200704L
#else
STATIC_ASSERT(__cpp_unicode_characters == 200704L);
#endif

#ifndef __cpp_unicode_literals
#error __cpp_unicode_literals is not defined
#elif __cpp_unicode_literals != 200710L
#error __cpp_unicode_literals is not 200710L
#else
STATIC_ASSERT(__cpp_unicode_literals == 200710L);
#endif

#ifndef __cpp_user_defined_literals
#error __cpp_user_defined_literals is not defined
#elif __cpp_user_defined_literals != 200809L
#error __cpp_user_defined_literals is not 200809L
#else
STATIC_ASSERT(__cpp_user_defined_literals == 200809L);
#endif

#ifndef __cpp_variable_templates
#error __cpp_variable_templates is not defined
#elif __cpp_variable_templates != 201304L
#error __cpp_variable_templates is not 201304L
#else
STATIC_ASSERT(__cpp_variable_templates == 201304L);
#endif

#ifndef __cpp_variadic_templates
#error __cpp_variadic_templates is not defined
#elif __cpp_variadic_templates != 200704L
#error __cpp_variadic_templates is not 200704L
#else
STATIC_ASSERT(__cpp_variadic_templates == 200704L);
#endif

#if _HAS_CXX17
#ifndef __cpp_aggregate_bases
#error __cpp_aggregate_bases is not defined
#elif __cpp_aggregate_bases != 201603L
#error __cpp_aggregate_bases is not 201603L
#else
STATIC_ASSERT(__cpp_aggregate_bases == 201603L);
#endif
#else
#ifdef __cpp_aggregate_bases
#error __cpp_aggregate_bases is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_capture_star_this
#error __cpp_capture_star_this is not defined
#elif __cpp_capture_star_this != 201603L
#error __cpp_capture_star_this is not 201603L
#else
STATIC_ASSERT(__cpp_capture_star_this == 201603L);
#endif
#else
#ifdef __cpp_capture_star_this
#error __cpp_capture_star_this is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_deduction_guides
#error __cpp_deduction_guides is not defined
#elif __cpp_deduction_guides != 201703L
#error __cpp_deduction_guides is not 201703L
#else
STATIC_ASSERT(__cpp_deduction_guides == 201703L);
#endif
#else
#ifdef __cpp_deduction_guides
#error __cpp_deduction_guides is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_fold_expressions
#error __cpp_fold_expressions is not defined
#elif __cpp_fold_expressions != 201603L
#error __cpp_fold_expressions is not 201603L
#else
STATIC_ASSERT(__cpp_fold_expressions == 201603L);
#endif
#else
#ifdef __cpp_fold_expressions
#error __cpp_fold_expressions is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_guaranteed_copy_elision
#error __cpp_guaranteed_copy_elision is not defined
#elif __cpp_guaranteed_copy_elision != 201606L
#error __cpp_guaranteed_copy_elision is not 201606L
#else
STATIC_ASSERT(__cpp_guaranteed_copy_elision == 201606L);
#endif
#else
#ifdef __cpp_guaranteed_copy_elision
#error __cpp_guaranteed_copy_elision is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_inline_variables
#error __cpp_inline_variables is not defined
#elif __cpp_inline_variables != 201606L
#error __cpp_inline_variables is not 201606L
#else
STATIC_ASSERT(__cpp_inline_variables == 201606L);
#endif
#else
#ifdef __cpp_inline_variables
#error __cpp_inline_variables is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_nontype_template_parameter_auto
#error __cpp_nontype_template_parameter_auto is not defined
#elif __cpp_nontype_template_parameter_auto != 201606L
#error __cpp_nontype_template_parameter_auto is not 201606L
#else
STATIC_ASSERT(__cpp_nontype_template_parameter_auto == 201606L);
#endif
#else
#ifdef __cpp_nontype_template_parameter_auto
#error __cpp_nontype_template_parameter_auto is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_structured_bindings
#error __cpp_structured_bindings is not defined
#elif __cpp_structured_bindings != 201606L
#error __cpp_structured_bindings is not 201606L
#else
STATIC_ASSERT(__cpp_structured_bindings == 201606L);
#endif
#else
#ifdef __cpp_structured_bindings
#error __cpp_structured_bindings is defined
#endif
#endif

#if _HAS_CXX17
#ifndef __cpp_variadic_using
#error __cpp_variadic_using is not defined
#elif __cpp_variadic_using != 201611L
#error __cpp_variadic_using is not 201611L
#else
STATIC_ASSERT(__cpp_variadic_using == 201611L);
#endif
#else
#ifdef __cpp_variadic_using
#error __cpp_variadic_using is defined
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
#error __cpp_template_template_args is not defined
#elif __cpp_template_template_args != 201611L
#error __cpp_template_template_args is not 201611L
#else
STATIC_ASSERT(__cpp_template_template_args == 201611L);
#endif
#else
#ifdef __cpp_template_template_args
#error __cpp_template_template_args is defined
#endif
#endif

#if _HAS_CXX17 || !defined(__clang__) // C1XX implemented this C++17 feature unconditionally.
#ifndef __cpp_enumerator_attributes
#error __cpp_enumerator_attributes is not defined
#elif __cpp_enumerator_attributes != 201411L
#error __cpp_enumerator_attributes is not 201411L
#else
STATIC_ASSERT(__cpp_enumerator_attributes == 201411L);
#endif
#else
#ifdef __cpp_enumerator_attributes
#error __cpp_enumerator_attributes is defined
#endif
#endif

#if _HAS_CXX17 || !defined(__clang__) // C1XX implemented this C++17 feature unconditionally.
#ifndef __cpp_namespace_attributes
#error __cpp_namespace_attributes is not defined
#elif __cpp_namespace_attributes != 201411L
#error __cpp_namespace_attributes is not 201411L
#else
STATIC_ASSERT(__cpp_namespace_attributes == 201411L);
#endif
#else
#ifdef __cpp_namespace_attributes
#error __cpp_namespace_attributes is defined
#endif
#endif

#if _HAS_CXX17 || defined(__EDG__) // EDG unconditionally reports "if constexpr" as being available.
#ifndef __cpp_if_constexpr
#error __cpp_if_constexpr is not defined
#elif __cpp_if_constexpr != 201606L
#error __cpp_if_constexpr is not 201606L
#else
STATIC_ASSERT(__cpp_if_constexpr == 201606L);
#endif
#else
#ifdef __cpp_if_constexpr
#error __cpp_if_constexpr is defined
#endif
#endif

#if _HAS_CXX17 || defined(__EDG__) // EDG unconditionally reports hex floats as being available.
#ifndef __cpp_hex_float
#error __cpp_hex_float is not defined
#elif __cpp_hex_float != 201603L
#error __cpp_hex_float is not 201603L
#else
STATIC_ASSERT(__cpp_hex_float == 201603L);
#endif
#else
#ifdef __cpp_hex_float
#error __cpp_hex_float is defined
#endif
#endif

#if _HAS_CXX20 && !defined(__clang__)
// Clang only has partial support for <=> but that does not include the feature test macro.
#ifndef __cpp_impl_three_way_comparison
#error __cpp_impl_three_way_comparison is not defined
#elif defined(__EDG__) // EDG does not yet implement P1630R1 or P1186R3 so they still report the old value.
#if __cpp_impl_three_way_comparison != 201711L
#error __cpp_impl_three_way_comparison is not 201711L
#else
STATIC_ASSERT(__cpp_impl_three_way_comparison == 201711L);
#endif
#else
#if __cpp_impl_three_way_comparison != 201907L
#error __cpp_impl_three_way_comparison is not 201907L
#else
STATIC_ASSERT(__cpp_impl_three_way_comparison == 201907L);
#endif
#endif
#else
#ifdef __cpp_impl_three_way_comparison
#error __cpp_impl_three_way_comparison is defined
#endif
#endif

#if _HAS_CXX20 && !defined(__clang__) && !defined(__EDG__)
#ifndef __cpp_nontype_template_args
#error __cpp_nontype_template_args is not defined
#elif __cpp_nontype_template_args != 201911L
#error __cpp_nontype_template_args is not 201911L
#else
STATIC_ASSERT(__cpp_nontype_template_args == 201911L);
#endif
#elif _HAS_CXX17
#ifndef __cpp_nontype_template_args
#error __cpp_nontype_template_args is not defined
#elif __cpp_nontype_template_args != 201411L
#error __cpp_nontype_template_args is not 201411L
#else
STATIC_ASSERT(__cpp_nontype_template_args == 201411L);
#endif
#else
#ifdef __cpp_nontype_template_args
#error __cpp_nontype_template_args is defined
#endif
#endif

#if _HAS_CXX20 || defined(__EDG__)
// EDG unconditionally reports "explicit(bool)" as being available.
#ifndef __cpp_conditional_explicit
#error __cpp_conditional_explicit is not defined
#elif __cpp_conditional_explicit != 201806L
#error __cpp_conditional_explicit is not 201806L
#else
STATIC_ASSERT(__cpp_conditional_explicit == 201806L);
#endif
#else
#ifdef __cpp_conditional_explicit
#error __cpp_conditional_explicit is defined
#endif
#endif

// C++98, /EHs[-]
#ifdef TEST_DISABLED_EXCEPTIONS
#ifdef __cpp_exceptions
#error __cpp_exceptions is defined
#endif
#else
#ifndef __cpp_exceptions
#error __cpp_exceptions is not defined
#elif __cpp_exceptions != 199711L
#error __cpp_exceptions is not 199711L
#else
STATIC_ASSERT(__cpp_exceptions == 199711L);
#endif
#endif

// C++98, /GR[-]
#ifdef TEST_DISABLED_RTTI
#ifdef __cpp_rtti
#error __cpp_rtti is defined
#endif
#else
#ifdef __cpp_namespace_attributes
#error __cpp_namespace_attributes is defined
#endif
#endif

// C++11, /Zc:threadSafeInit[-]
#if defined(_M_CEE_PURE) || defined(TEST_DISABLED_THREADSAFE_STATIC_INIT)
#ifdef __cpp_threadsafe_static_init
#error __cpp_threadsafe_static_init is defined
#endif
#else
#ifndef __cpp_threadsafe_static_init
#error __cpp_threadsafe_static_init is not defined
#elif __cpp_threadsafe_static_init != 200806L
#error __cpp_threadsafe_static_init is not 200806L
#else
STATIC_ASSERT(__cpp_threadsafe_static_init == 200806L);
#endif
#endif

// C++14, /Zc:sizedDealloc[-]
#if defined(TEST_DISABLED_SIZED_DEALLOCATION) || defined(__clang__) // Clang disables sized deallocation by default.
#ifdef __cpp_sized_deallocation
#error __cpp_sized_deallocation is defined
#endif
#else
#ifndef __cpp_sized_deallocation
#error __cpp_sized_deallocation is not defined
#elif __cpp_sized_deallocation != 201309L
#error __cpp_sized_deallocation is not 201309L
#else
STATIC_ASSERT(__cpp_sized_deallocation == 201309L);
#endif
#endif

// C++17, /Zc:alignedNew[-]
#if !_HAS_CXX17 || defined(TEST_DISABLED_ALIGNED_NEW)
#ifdef __cpp_aligned_new
#error __cpp_aligned_new is defined
#endif
#else
#ifndef __cpp_aligned_new
#error __cpp_aligned_new is not defined
#elif __cpp_aligned_new != 201606L
#error __cpp_aligned_new is not 201606L
#else
STATIC_ASSERT(__cpp_aligned_new == 201606L);
#endif
#endif

// C++17, /Zc:noexceptTypes[-]
#if !_HAS_CXX17 || defined(TEST_DISABLED_NOEXCEPT_FUNCTION_TYPE)
#ifdef __cpp_noexcept_function_type
#error __cpp_noexcept_function_type is defined
#endif
#else
#ifndef __cpp_noexcept_function_type
#error __cpp_noexcept_function_type is not defined
#elif __cpp_noexcept_function_type != 201510L
#error __cpp_noexcept_function_type is not 201510L
#else
STATIC_ASSERT(__cpp_noexcept_function_type == 201510L);
#endif
#endif


// LIBRARY FEATURE-TEST MACROS
#include <version>

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

#ifndef __cpp_lib_as_const
#error __cpp_lib_as_const is not defined
#elif __cpp_lib_as_const != 201510L
#error __cpp_lib_as_const is not 201510L
#else
STATIC_ASSERT(__cpp_lib_as_const == 201510L);
#endif

#ifndef __cpp_lib_atomic_value_initialization
#error __cpp_lib_atomic_value_initialization is not defined
#elif __cpp_lib_atomic_value_initialization != 201911L
#error __cpp_lib_atomic_value_initialization is not 201911L
#else
STATIC_ASSERT(__cpp_lib_atomic_value_initialization == 201911L);
#endif

#ifndef __cpp_lib_bool_constant
#error __cpp_lib_bool_constant is not defined
#elif __cpp_lib_bool_constant != 201505L
#error __cpp_lib_bool_constant is not 201505L
#else
STATIC_ASSERT(__cpp_lib_bool_constant == 201505L);
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

#ifndef __cpp_lib_complex_udls
#error __cpp_lib_complex_udls is not defined
#elif __cpp_lib_complex_udls != 201309L
#error __cpp_lib_complex_udls is not 201309L
#else
STATIC_ASSERT(__cpp_lib_complex_udls == 201309L);
#endif

#ifndef __cpp_lib_enable_shared_from_this
#error __cpp_lib_enable_shared_from_this is not defined
#elif __cpp_lib_enable_shared_from_this != 201603L
#error __cpp_lib_enable_shared_from_this is not 201603L
#else
STATIC_ASSERT(__cpp_lib_enable_shared_from_this == 201603L);
#endif

#ifndef __cpp_lib_exchange_function
#error __cpp_lib_exchange_function is not defined
#elif __cpp_lib_exchange_function != 201304L
#error __cpp_lib_exchange_function is not 201304L
#else
STATIC_ASSERT(__cpp_lib_exchange_function == 201304L);
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

#ifndef __cpp_lib_generic_associative_lookup
#error __cpp_lib_generic_associative_lookup is not defined
#elif __cpp_lib_generic_associative_lookup != 201304L
#error __cpp_lib_generic_associative_lookup is not 201304L
#else
STATIC_ASSERT(__cpp_lib_generic_associative_lookup == 201304L);
#endif

#ifndef __cpp_lib_incomplete_container_elements
#error __cpp_lib_incomplete_container_elements is not defined
#elif __cpp_lib_incomplete_container_elements != 201505L
#error __cpp_lib_incomplete_container_elements is not 201505L
#else
STATIC_ASSERT(__cpp_lib_incomplete_container_elements == 201505L);
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

#ifndef __cpp_lib_invoke
#error __cpp_lib_invoke is not defined
#elif __cpp_lib_invoke != 201411L
#error __cpp_lib_invoke is not 201411L
#else
STATIC_ASSERT(__cpp_lib_invoke == 201411L);
#endif

#ifndef __cpp_lib_is_final
#error __cpp_lib_is_final is not defined
#elif __cpp_lib_is_final != 201402L
#error __cpp_lib_is_final is not 201402L
#else
STATIC_ASSERT(__cpp_lib_is_final == 201402L);
#endif

#ifndef __cpp_lib_is_null_pointer
#error __cpp_lib_is_null_pointer is not defined
#elif __cpp_lib_is_null_pointer != 201309L
#error __cpp_lib_is_null_pointer is not 201309L
#else
STATIC_ASSERT(__cpp_lib_is_null_pointer == 201309L);
#endif

#ifndef __cpp_lib_logical_traits
#error __cpp_lib_logical_traits is not defined
#elif __cpp_lib_logical_traits != 201510L
#error __cpp_lib_logical_traits is not 201510L
#else
STATIC_ASSERT(__cpp_lib_logical_traits == 201510L);
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

#ifndef __cpp_lib_nonmember_container_access
#error __cpp_lib_nonmember_container_access is not defined
#elif __cpp_lib_nonmember_container_access != 201411L
#error __cpp_lib_nonmember_container_access is not 201411L
#else
STATIC_ASSERT(__cpp_lib_nonmember_container_access == 201411L);
#endif

#ifndef __cpp_lib_null_iterators
#error __cpp_lib_null_iterators is not defined
#elif __cpp_lib_null_iterators != 201304L
#error __cpp_lib_null_iterators is not 201304L
#else
STATIC_ASSERT(__cpp_lib_null_iterators == 201304L);
#endif

#ifndef __cpp_lib_quoted_string_io
#error __cpp_lib_quoted_string_io is not defined
#elif __cpp_lib_quoted_string_io != 201304L
#error __cpp_lib_quoted_string_io is not 201304L
#else
STATIC_ASSERT(__cpp_lib_quoted_string_io == 201304L);
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

#ifndef __cpp_lib_shared_mutex
#error __cpp_lib_shared_mutex is not defined
#elif __cpp_lib_shared_mutex != 201505L
#error __cpp_lib_shared_mutex is not 201505L
#else
STATIC_ASSERT(__cpp_lib_shared_mutex == 201505L);
#endif

#ifndef __cpp_lib_shared_ptr_arrays
#error __cpp_lib_shared_ptr_arrays is not defined
#elif __cpp_lib_shared_ptr_arrays != 201611L
#error __cpp_lib_shared_ptr_arrays is not 201611L
#else
STATIC_ASSERT(__cpp_lib_shared_ptr_arrays == 201611L);
#endif

#ifndef __cpp_lib_string_udls
#error __cpp_lib_string_udls is not defined
#elif __cpp_lib_string_udls != 201304L
#error __cpp_lib_string_udls is not 201304L
#else
STATIC_ASSERT(__cpp_lib_string_udls == 201304L);
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

#ifndef __cpp_lib_void_t
#error __cpp_lib_void_t is not defined
#elif __cpp_lib_void_t != 201411L
#error __cpp_lib_void_t is not 201411L
#else
STATIC_ASSERT(__cpp_lib_void_t == 201411L);
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

#if _HAS_CXX17
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

#if _HAS_CXX17 && !defined(_M_CEE)
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

#if _HAS_CXX20 && !defined(__EDG__) // TRANSITION, VSO-1041044
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

#if _HAS_CXX20 && (defined(__clang__) || defined(__EDG__)) // TRANSITION, VSO-1020212
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

#if _HAS_CXX20 && defined(__cpp_concepts)
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

#ifdef _M_CEE
#ifdef __cpp_lib_shared_timed_mutex
#error __cpp_lib_shared_timed_mutex is defined
#endif
#else // ^^^ _M_CEE ^^^ // vvv !_M_CEE vvv
#ifndef __cpp_lib_shared_timed_mutex
#error __cpp_lib_shared_timed_mutex is not defined
#elif __cpp_lib_shared_timed_mutex != 201402L
#error __cpp_lib_shared_timed_mutex is not 201402L
#else
STATIC_ASSERT(__cpp_lib_shared_timed_mutex == 201402L);
#endif
#endif // _M_CEE
