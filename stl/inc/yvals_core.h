// yvals_core.h internal header (core)

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _YVALS_CORE_H_
#define _YVALS_CORE_H_
#ifndef _STL_COMPILER_PREPROCESSOR
// All STL headers avoid exposing their contents when included by various
// non-C++-compiler tools to avoid breaking builds when we use newer language
// features in the headers than such tools understand.
#if defined(RC_INVOKED) || defined(Q_MOC_RUN) || defined(__midl)
#define _STL_COMPILER_PREPROCESSOR 0
#else
#define _STL_COMPILER_PREPROCESSOR 1
#endif
#endif // _STL_COMPILER_PREPROCESSOR

#if _STL_COMPILER_PREPROCESSOR
// Implemented unconditionally:
// N3911 void_t
// N4089 Safe Conversions In unique_ptr<T[]>
// N4169 invoke()
// N4258 noexcept Cleanups
// N4259 uncaught_exceptions()
// N4277 Trivially Copyable reference_wrapper
// N4279 insert_or_assign()/try_emplace() For map/unordered_map
// N4280 size(), empty(), data()
// N4366 Precisely Constraining unique_ptr Assignment
// N4387 Improving pair And tuple
// N4389 bool_constant
// N4508 shared_mutex (Untimed)
// N4510 Supporting Incomplete Types In vector/list/forward_list
// P0006R0 Variable Templates For Type Traits (is_same_v, etc.)
// P0007R1 as_const()
// P0013R1 Logical Operator Type Traits (conjunction, etc.)
// P0033R1 Rewording enable_shared_from_this
// P0063R3 C11 Standard Library
// P0074R0 owner_less<>
// P0092R1 <chrono> floor(), ceil(), round(), abs()
// P0340R3 SFINAE-Friendly underlying_type
// P0414R2 shared_ptr<T[]>, shared_ptr<T[N]>
// P0418R2 atomic compare_exchange memory_order Requirements
// P0435R1 Overhauling common_type
// P0497R0 Fixing shared_ptr For Arrays
// P0513R0 Poisoning hash
// P0516R0 Marking shared_future Copying As noexcept
// P0517R0 Constructing future_error From future_errc
// P0548R1 Tweaking common_type And duration
// P0558R1 Resolving atomic<T> Named Base Class Inconsistencies
// P0599R1 noexcept hash
// P0738R2 istream_iterator Cleanup
// P0771R1 noexcept For std::function's Move Constructor
// P0777R1 Avoiding Unnecessary decay
// P0809R0 Comparing Unordered Containers
// P0941R2 Feature-Test Macros
// P0972R0 noexcept For <chrono> zero(), min(), max()
// P1164R1 Making create_directory() Intuitive
// P1902R1 Missing Feature-Test Macros 2017-2019

// _HAS_CXX17 directly controls:
// P0005R4 not_fn()
// P0024R2 Parallel Algorithms
// P0025R1 clamp()
// P0030R1 hypot(x, y, z)
// P0031R0 constexpr For <array> (Again) And <iterator>
// P0032R3 Homogeneous Interface For variant/any/optional
// P0040R3 Extending Memory Management Tools
// P0067R5 Elementary String Conversions
// P0083R3 Splicing Maps And Sets
// P0084R2 Emplace Return Type
// P0088R3 <variant>
// P0137R1 launder()
// P0152R1 atomic::is_always_lock_free
// P0154R1 hardware_destructive_interference_size, etc.
// P0156R2 scoped_lock
// P0163R0 shared_ptr::weak_type
// P0185R1 is_swappable, is_nothrow_swappable
// P0209R2 make_from_tuple()
// P0218R1 <filesystem>
// P0220R1 <any>, <memory_resource>, <optional>, <string_view>, apply(), sample(), Boyer-Moore search()
// P0226R1 Mathematical Special Functions
// P0253R1 Fixing Searcher Return Types
// P0254R2 Integrating string_view And std::string
// P0258R2 has_unique_object_representations
// P0272R1 Non-const basic_string::data()
// P0295R0 gcd(), lcm()
// P0307R2 Making Optional Greater Equal Again
// P0336R1 Renaming Parallel Execution Policies
// P0337R0 Deleting polymorphic_allocator Assignment
// P0358R1 Fixes For not_fn()
// P0393R3 Making Variant Greater Equal
// P0394R4 Parallel Algorithms Should terminate() For Exceptions
// P0403R1 UDLs For <string_view> ("meow"sv, etc.)
// P0426R1 constexpr For char_traits
// P0433R2 Deduction Guides For The STL
// P0452R1 Unifying <numeric> Parallel Algorithms
// P0504R0 Revisiting in_place_t/in_place_type_t<T>/in_place_index_t<I>
// P0505R0 constexpr For <chrono> (Again)
// P0508R0 Clarifying insert_return_type
// P0510R0 Rejecting variants Of Nothing, Arrays, References, And Incomplete Types
// P0602R4 Propagating Copy/Move Triviality In variant/optional
// P0604R0 invoke_result, is_invocable, is_nothrow_invocable
// P0607R0 Inline Variables For The STL
// P0682R1 Repairing Elementary String Conversions
// P0739R0 Improving Class Template Argument Deduction For The STL
// P0858R0 Constexpr Iterator Requirements

// _HAS_CXX17 indirectly controls:
// N4190 Removing auto_ptr, random_shuffle(), And Old <functional> Stuff
// P0003R5 Removing Dynamic Exception Specifications
// P0004R1 Removing Deprecated Iostreams Aliases
// P0298R3 std::byte
// P0302R1 Removing Allocator Support In std::function
// LWG-2385 function::assign allocator argument doesn't make sense
// LWG-2921 packaged_task and type-erased allocators
// LWG-2976 Dangling uses_allocator specialization for packaged_task
// The non-Standard std::tr1 namespace and TR1-only machinery
// Enforcement of matching allocator value_types

// _HAS_CXX17 and _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS control:
// P0174R2 Deprecating Vestigial Library Parts
// P0521R0 Deprecating shared_ptr::unique()
// P0618R0 Deprecating <codecvt>
// Other C++17 deprecation warnings

// _HAS_CXX20 directly controls:
// P0020R6 atomic<float>, atomic<double>, atomic<long double>
// P0318R1 unwrap_reference, unwrap_ref_decay
// P0325R4 to_array()
// P0356R5 bind_front()
// P0439R0 enum class memory_order
// P0457R2 starts_with()/ends_with() For basic_string/basic_string_view
// P0458R2 contains() For Ordered And Unordered Associative Containers
// P0463R1 endian
// P0482R6 Library Support For char8_t
//     (mbrtoc8 and c8rtomb not yet implemented)
// P0487R1 Fixing operator>>(basic_istream&, CharT*)
// P0550R2 remove_cvref
// P0553R4 <bit> Rotating And Counting Functions
// P0556R3 <bit> ispow2(), ceil2(), floor2(), log2p1()
//            (log2p1() is called bit_length() as of D1956)
// P0595R2 is_constant_evaluated()
// P0616R0 Using move() In <numeric>
// P0631R8 <numbers> Math Constants
// P0646R1 list/forward_list remove()/remove_if()/unique() Return size_type
// P0653R2 to_address()
// P0655R1 visit<R>()
// P0758R1 is_nothrow_convertible
// P0768R1 Library Support For The Spaceship Comparison Operator <=>
//     (partially implemented)
// P0769R2 shift_left(), shift_right()
// P0811R3 midpoint(), lerp()
//     (partially implemented, lerp() not yet constexpr)
// P0887R1 type_identity
// P0896R4 Ranges
//     (partially implemented)
// P0898R3 Standard Library Concepts
// P0919R3 Heterogeneous Lookup For Unordered Containers
// P0966R1 string::reserve() Should Not Shrink
// P1209R0 erase_if(), erase()
// P1227R2 Signed std::ssize(), Unsigned span::size()
//     (partially implemented)
// P1357R1 is_bounded_array, is_unbounded_array
// P1456R1 Move-Only Views
// P1612R1 Relocating endian To <bit>
// P1651R0 bind_front() Should Not Unwrap reference_wrapper
// P1690R1 Refining Heterogeneous Lookup For Unordered Containers
// P1754R1 Rename Concepts To standard_case
// P1870R1 safe_range
// P1959R0 Removing weak_equality And strong_equality
// P????R? directory_entry::clear_cache()

// _HAS_CXX20 and _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS control:
// P0767R1 Deprecating is_pod
// Other C++20 deprecation warnings

// Parallel Algorithms Notes
// C++ allows an implementation to implement parallel algorithms as calls to the serial algorithms.
// This implementation parallelizes several common algorithm calls, but not all.
//
// The following algorithms are parallelized.
// * adjacent_difference
// * adjacent_find
// * all_of
// * any_of
// * count
// * count_if
// * equal
// * exclusive_scan
// * find
// * find_end
// * find_first_of
// * find_if
// * find_if_not
// * for_each
// * for_each_n
// * inclusive_scan
// * is_heap
// * is_heap_until
// * is_partitioned
// * is_sorted
// * is_sorted_until
// * mismatch
// * none_of
// * partition
// * reduce
// * remove
// * remove_if
// * replace
// * replace_if
// * search
// * search_n
// * set_difference
// * set_intersection
// * sort
// * stable_sort
// * transform
// * transform_exclusive_scan
// * transform_inclusive_scan
// * transform_reduce
//
// The following are not presently parallelized:
//
// No apparent parallelism performance improvement on target hardware; all algorithms which
// merely copy or permute elements with no branches are typically memory bandwidth limited.
// * copy
// * copy_n
// * fill
// * fill_n
// * move
// * reverse
// * reverse_copy
// * rotate
// * rotate_copy
// * shift_left
// * shift_right
// * swap_ranges
//
// Confusion over user parallelism requirements exists; likely in the above category anyway.
// * generate
// * generate_n
//
// Effective parallelism suspected to be infeasible.
// * partial_sort
// * partial_sort_copy
//
// Not yet evaluated; parallelism may be implemented in a future release and is suspected to be beneficial.
// * copy_if
// * includes
// * inplace_merge
// * lexicographical_compare
// * max_element
// * merge
// * min_element
// * minmax_element
// * nth_element
// * partition_copy
// * remove_copy
// * remove_copy_if
// * replace_copy
// * replace_copy_if
// * set_symmetric_difference
// * set_union
// * stable_partition
// * unique
// * unique_copy

#include <vcruntime.h>
#include <xkeycheck.h> // The _HAS_CXX tags must be defined before including this.

#ifndef _STL_WARNING_LEVEL
#if defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
#define _STL_WARNING_LEVEL 4
#else // defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
#define _STL_WARNING_LEVEL 3
#endif // defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
#endif // _STL_WARNING_LEVEL

#if _STL_WARNING_LEVEL < 3
#error _STL_WARNING_LEVEL cannot be less than 3.
#endif // _STL_WARNING_LEVEL < 3

#if _STL_WARNING_LEVEL > 4
#error _STL_WARNING_LEVEL cannot be greater than 4.
#endif // _STL_WARNING_LEVEL > 4

// _HAS_NODISCARD (in vcruntime.h) controls:
// [[nodiscard]] attributes on STL functions

// Controls whether the STL uses "if constexpr" internally
#ifndef _HAS_IF_CONSTEXPR
#ifdef __CUDACC__
#define _HAS_IF_CONSTEXPR 0
#else // __CUDACC__
#define _HAS_IF_CONSTEXPR 1
#endif // __CUDACC__
#endif // _HAS_IF_CONSTEXPR

// Controls whether the STL uses "conditional explicit" internally
#ifndef _HAS_CONDITIONAL_EXPLICIT
#ifdef __cpp_conditional_explicit
#define _HAS_CONDITIONAL_EXPLICIT 1
#elif defined(__CUDACC__)
#define _HAS_CONDITIONAL_EXPLICIT 0 // TRANSITION
#elif defined(__clang__)
#define _HAS_CONDITIONAL_EXPLICIT 0 // TRANSITION, LLVM-42694
#else // vvv C1XX or non-CUDA EDG vvv
#define _HAS_CONDITIONAL_EXPLICIT 1
#endif // ^^^ C1XX or non-CUDA EDG ^^^
#endif // _HAS_CONDITIONAL_EXPLICIT

// warning C4577: 'noexcept' used with no exception handling mode specified;
// termination on exception is not guaranteed. Specify /EHsc (/Wall)
#if _HAS_EXCEPTIONS
#define _STL_DISABLED_WARNING_C4577
#else // _HAS_EXCEPTIONS
#define _STL_DISABLED_WARNING_C4577 4577
#endif // _HAS_EXCEPTIONS

// warning C4984: 'if constexpr' is a C++17 language extension
#if !_HAS_CXX17 && _HAS_IF_CONSTEXPR
#define _STL_DISABLED_WARNING_C4984 4984
#else // !_HAS_CXX17 && _HAS_IF_CONSTEXPR
#define _STL_DISABLED_WARNING_C4984
#endif // !_HAS_CXX17 && _HAS_IF_CONSTEXPR

// warning C5053: support for 'explicit(<expr>)' in C++17 and earlier is a vendor extension
#if !_HAS_CXX20 && _HAS_CONDITIONAL_EXPLICIT
#define _STL_DISABLED_WARNING_C5053 5053
#else // !_HAS_CXX20 && _HAS_CONDITIONAL_EXPLICIT
#define _STL_DISABLED_WARNING_C5053
#endif // !_HAS_CXX20 && _HAS_CONDITIONAL_EXPLICIT

#ifndef _STL_EXTRA_DISABLED_WARNINGS
#define _STL_EXTRA_DISABLED_WARNINGS
#endif // _STL_EXTRA_DISABLED_WARNINGS

// warning C4180: qualifier applied to function type has no meaning; ignored
// warning C4412: function signature contains type 'meow'; C++ objects are unsafe to pass between pure code
//                and mixed or native. (/Wall)
// warning C4455: literal suffix identifiers that do not start with an underscore are reserved
// warning C4472: 'meow' is a native enum: add an access specifier (private/public)
//                to declare a managed enum (/Wall)
// warning C4494: Ignoring __declspec(allocator) because the function return type is not a pointer or reference
// warning C4514: unreferenced inline function has been removed (/Wall)
// warning C4571: Informational: catch(...) semantics changed since Visual C++ 7.1;
//                structured exceptions (SEH) are no longer caught (/Wall)
// warning C4574: 'MACRO' is defined to be '0': did you mean to use '#if MACRO'? (/Wall)
// warning C4582: 'union': constructor is not implicitly called (/Wall)
// warning C4583: 'union': destructor is not implicitly called (/Wall)
// warning C4587: behavior change: constructor is no longer implicitly called (/Wall)
// warning C4588: behavior change: destructor is no longer implicitly called (/Wall)
// warning C4619: #pragma warning: there is no warning number 'number' (/Wall)
// warning C4623: default constructor was implicitly defined as deleted (/Wall)
// warning C4625: copy constructor was implicitly defined as deleted (/Wall)
// warning C4626: assignment operator was implicitly defined as deleted (/Wall)
// warning C4643: Forward declaring 'meow' in namespace std is not permitted by the C++ Standard. (/Wall)
// warning C4702: unreachable code
// warning C4793: function compiled as native
// warning C4820: 'N' bytes padding added after data member 'meow' (/Wall)
// warning C4988: variable declared outside class/function scope (/Wall /d1WarnOnGlobals)
// warning C5026: move constructor was implicitly defined as deleted (/Wall)
// warning C5027: move assignment operator was implicitly defined as deleted (/Wall)
// warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified (/Wall)

#ifndef _STL_DISABLED_WARNINGS
// clang-format off
#define _STL_DISABLED_WARNINGS                        \
    4180 4412 4455 4472 4494 4514 4571 4574 4582 4583 \
    4587 4588 4619 4623 4625 4626 4643 4702 4793 4820 \
    4988 5026 5027 5045                               \
    _STL_DISABLED_WARNING_C4577                       \
    _STL_DISABLED_WARNING_C4984                       \
    _STL_DISABLED_WARNING_C5053                       \
    _STL_EXTRA_DISABLED_WARNINGS
// clang-format on
#endif // _STL_DISABLED_WARNINGS

// warning: constexpr if is a C++17 extension [-Wc++17-extensions]
// warning: user-defined literal suffixes not starting with '_' are reserved [-Wuser-defined-literals]
// warning: unknown pragma ignored [-Wunknown-pragmas]
#ifndef _STL_DISABLE_CLANG_WARNINGS
#ifdef __clang__
// clang-format off
#define _STL_DISABLE_CLANG_WARNINGS                                 \
    _Pragma("clang diagnostic push")                                \
    _Pragma("clang diagnostic ignored \"-Wc++17-extensions\"")      \
    _Pragma("clang diagnostic ignored \"-Wuser-defined-literals\"") \
    _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")
// clang-format on
#else // __clang__
#define _STL_DISABLE_CLANG_WARNINGS
#endif // __clang__
#endif // _STL_DISABLE_CLANG_WARNINGS

#ifndef _STL_RESTORE_CLANG_WARNINGS
#ifdef __clang__
#define _STL_RESTORE_CLANG_WARNINGS _Pragma("clang diagnostic pop")
#else // __clang__
#define _STL_RESTORE_CLANG_WARNINGS
#endif // __clang__
#endif // _STL_RESTORE_CLANG_WARNINGS

// clang-format off
#ifndef _STL_DISABLE_DEPRECATED_WARNING
#ifdef __clang__
#define _STL_DISABLE_DEPRECATED_WARNING \
    _Pragma("clang diagnostic push")    \
    _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#else // __clang__
#define _STL_DISABLE_DEPRECATED_WARNING \
    __pragma(warning(push))             \
    __pragma(warning(disable : 4996)) // was declared deprecated
#endif // __clang__
#endif // _STL_DISABLE_DEPRECATED_WARNING
// clang-format on

#ifndef _STL_RESTORE_DEPRECATED_WARNING
#ifdef __clang__
#define _STL_RESTORE_DEPRECATED_WARNING _Pragma("clang diagnostic pop")
#else // __clang__
#define _STL_RESTORE_DEPRECATED_WARNING __pragma(warning(pop))
#endif // __clang__
#endif // _STL_RESTORE_DEPRECATED_WARNING

#define _CPPLIB_VER       650
#define _MSVC_STL_VERSION 142
#define _MSVC_STL_UPDATE  201912L

#ifndef _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#ifdef __EDG__
// not attempting to detect __EDG_VERSION__ being less than expected
#elif defined(__clang__)
#if __clang_major__ < 9
#error STL1000: Unexpected compiler version, expected Clang 9.0.0 or newer.
#endif // ^^^ old Clang ^^^
#elif defined(_MSC_VER)
#if _MSC_VER < 1924 // Coarse-grained, not inspecting _MSC_FULL_VER
#error STL1001: Unexpected compiler version, expected MSVC 19.24 or newer.
#endif // ^^^ old MSVC ^^^
#else // vvv other compilers vvv
// not attempting to detect other compilers
#endif // ^^^ other compilers ^^^
#endif // _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH

#ifndef _HAS_STATIC_RTTI
#define _HAS_STATIC_RTTI 1
#endif // _HAS_STATIC_RTTI

#if defined(_CPPRTTI) && !_HAS_STATIC_RTTI
#error /GR implies _HAS_STATIC_RTTI.
#endif // defined(_CPPRTTI) && !_HAS_STATIC_RTTI

// N4842 [dcl.constexpr]/1: "A function or static data member declared with the
// constexpr or consteval specifier is implicitly an inline function or variable"

// Functions that became constexpr in C++17
#if _HAS_CXX17
#define _CONSTEXPR17 constexpr
#else // ^^^ constexpr in C++17 and later / inline (not constexpr) in C++14 vvv
#define _CONSTEXPR17 inline
#endif // ^^^ inline (not constexpr) in C++14 ^^^

// Functions that became constexpr in C++20
#if _HAS_CXX20
#define _CONSTEXPR20 constexpr
#else // ^^^ constexpr in C++20 and later / inline (not constexpr) in C++17 and earlier vvv
#define _CONSTEXPR20 inline
#endif // ^^^ inline (not constexpr) in C++17 and earlier ^^^

// P0607R0 Inline Variables For The STL
#if _HAS_CXX17
#define _INLINE_VAR inline
#else // _HAS_CXX17
#define _INLINE_VAR
#endif // _HAS_CXX17

// N4190 Removing auto_ptr, random_shuffle(), And Old <functional> Stuff
#ifndef _HAS_AUTO_PTR_ETC
#define _HAS_AUTO_PTR_ETC (!_HAS_CXX17)
#endif // _HAS_AUTO_PTR_ETC

// P0003R5 Removing Dynamic Exception Specifications
#ifndef _HAS_UNEXPECTED
#define _HAS_UNEXPECTED (!_HAS_CXX17)
#endif // _HAS_UNEXPECTED

// P0004R1 Removing Deprecated Iostreams Aliases
#ifndef _HAS_OLD_IOSTREAMS_MEMBERS
#define _HAS_OLD_IOSTREAMS_MEMBERS (!_HAS_CXX17)
#endif // _HAS_OLD_IOSTREAMS_MEMBERS

// P0298R3 std::byte
#ifndef _HAS_STD_BYTE
#define _HAS_STD_BYTE _HAS_CXX17 // inspected by GSL, do not remove
#endif // _HAS_STD_BYTE

// P0302R1 Removing Allocator Support In std::function
// LWG-2385 function::assign allocator argument doesn't make sense
// LWG-2921 packaged_task and type-erased allocators
// LWG-2976 Dangling uses_allocator specialization for packaged_task
#ifndef _HAS_FUNCTION_ALLOCATOR_SUPPORT
#define _HAS_FUNCTION_ALLOCATOR_SUPPORT (!_HAS_CXX17)
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

// The non-Standard std::tr1 namespace and TR1-only machinery
#ifndef _HAS_TR1_NAMESPACE
#define _HAS_TR1_NAMESPACE (!_HAS_CXX17)
#endif // _HAS_TR1_NAMESPACE

// STL4000 is "_STATIC_CPPLIB is deprecated", currently in yvals.h
// STL4001 is "/clr:pure is deprecated", currently in yvals.h

#if _HAS_TR1_NAMESPACE
#ifdef _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _DEPRECATE_TR1_NAMESPACE
#else // _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _DEPRECATE_TR1_NAMESPACE                                                                                  \
    [[deprecated(                                                                                                 \
        "warning STL4002: "                                                                                       \
        "The non-Standard std::tr1 namespace and TR1-only machinery are deprecated and will be REMOVED. You can " \
        "define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING to acknowledge that you have received this warning.")]]
#endif // _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#endif // _HAS_TR1_NAMESPACE

// STL4003 was "The non-Standard std::identity struct is deprecated and will be REMOVED."

// Enforcement of matching allocator value_types
#ifndef _ENFORCE_MATCHING_ALLOCATORS
#define _ENFORCE_MATCHING_ALLOCATORS _HAS_CXX17
#endif // _ENFORCE_MATCHING_ALLOCATORS

#define _MISMATCHED_ALLOCATOR_MESSAGE(_CONTAINER, _VALUE_TYPE)                                 \
    _CONTAINER " requires that Allocator's value_type match " _VALUE_TYPE                      \
               " (See N4659 26.2.1 [container.requirements.general]/16 allocator_type)"        \
               " Either fix the allocator value_type or define _ENFORCE_MATCHING_ALLOCATORS=0" \
               " to suppress this diagnostic."

// Enforcement of Standard facet specializations
#ifndef _ENFORCE_FACET_SPECIALIZATIONS
#define _ENFORCE_FACET_SPECIALIZATIONS 0
#endif // _ENFORCE_FACET_SPECIALIZATIONS

#define _FACET_SPECIALIZATION_MESSAGE                                                  \
    "Unsupported facet specialization; see N4800 27.3.1.1.1 [locale.category]. "       \
    "Either use a Standard specialization or define _ENFORCE_FACET_SPECIALIZATIONS=0 " \
    "to suppress this diagnostic."

// To improve compiler throughput, use 'hidden friend' operators in <system_error> instead of non-members that are
// depicted in the Standard.
#ifndef _STL_OPTIMIZE_SYSTEM_ERROR_OPERATORS
#define _STL_OPTIMIZE_SYSTEM_ERROR_OPERATORS 1
#endif // _STL_OPTIMIZE_SYSTEM_ERROR_OPERATORS

#if _HAS_IF_CONSTEXPR
#define _CONSTEXPR_IF constexpr
#else // _HAS_IF_CONSTEXPR
#define _CONSTEXPR_IF
#endif // _HAS_IF_CONSTEXPR

#ifdef __clang__
#define _CONSTEVAL consteval
#else // ^^^ supports consteval / no consteval vvv
#define _CONSTEVAL constexpr
#endif // ^^^ no consteval ^^^

// Controls whether the STL will force /fp:fast to enable vectorization of algorithms defined
// in the standard as special cases; such as reduce, transform_reduce, inclusive_scan, exclusive_scan
#ifndef _STD_VECTORIZE_WITH_FLOAT_CONTROL
#ifdef _M_FP_EXCEPT
#define _STD_VECTORIZE_WITH_FLOAT_CONTROL 0
#else // ^^^ floating point exceptions enabled / floating point exceptions disabled (default) vvv
#define _STD_VECTORIZE_WITH_FLOAT_CONTROL 1
#endif // _M_FP_EXCEPT
#endif // _STD_VECTORIZE_WITH_FLOAT_CONTROL

// P0174R2 Deprecating Vestigial Library Parts
// P0521R0 Deprecating shared_ptr::unique()
// Other C++17 deprecation warnings

// N4659 D.4 [depr.cpp.headers]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_C_HEADER                                                               \
    [[deprecated("warning STL4004: "                                                            \
                 "<ccomplex>, <cstdalign>, <cstdbool>, and <ctgmath> are deprecated in C++17. " \
                 "You can define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING "                  \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_C_HEADER
#endif // ^^^ warning disabled ^^^

// N4659 D.6 [depr.str.strstreams]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_STRSTREAM                                              \
    [[deprecated("warning STL4005: <strstream> is deprecated in C++17. "        \
                 "You can define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_STRSTREAM
#endif // ^^^ warning disabled ^^^

// N4659 D.7 [depr.uncaught]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_UNCAUGHT_EXCEPTION                                              \
    [[deprecated("warning STL4006: "                                                     \
                 "std::uncaught_exception() is deprecated in C++17. "                    \
                 "It is superseded by std::uncaught_exceptions(), plural. "              \
                 "You can define _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_UNCAUGHT_EXCEPTION
#endif // ^^^ warning disabled ^^^

// N4659 D.8.1 [depr.weak.result_type]
// N4659 D.8.2 [depr.func.adaptor.typedefs]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS                                                                         \
    [[deprecated(                                                                                                 \
        "warning STL4007: Many result_type typedefs "                                                             \
        "and all argument_type, first_argument_type, and second_argument_type typedefs are deprecated in C++17. " \
        "You can define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING "                                     \
        "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS
#endif // ^^^ warning disabled ^^^

// N4659 D.8.3 [depr.negators]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_NEGATORS                                                                                \
    [[deprecated("warning STL4008: "                                                                             \
                 "std::not1(), std::not2(), std::unary_negate, and std::binary_negate are deprecated in C++17. " \
                 "They are superseded by std::not_fn(). "                                                        \
                 "You can define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING "                                   \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_NEGATORS
#endif // ^^^ warning disabled ^^^

// STL4009 was "std::allocator<void> is deprecated in C++17"

// N4659 D.9 [depr.default.allocator]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS                                              \
    [[deprecated("warning STL4010: "                                                        \
                 "Various members of std::allocator are deprecated in C++17. "              \
                 "Use std::allocator_traits instead of accessing these members directly. "  \
                 "You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS
#endif // ^^^ warning disabled ^^^

// N4659 D.10 [depr.storage.iterator]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_RAW_STORAGE_ITERATOR                                                  \
    [[deprecated("warning STL4011: "                                                           \
                 "std::raw_storage_iterator is deprecated in C++17. "                          \
                 "Consider using the std::uninitialized_copy() family of algorithms instead. " \
                 "You can define _SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING "     \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_RAW_STORAGE_ITERATOR
#endif // ^^^ warning disabled ^^^

// N4659 D.11 [depr.temporary.buffer]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_TEMPORARY_BUFFER                                                                   \
    [[deprecated("warning STL4012: "                                                                        \
                 "std::get_temporary_buffer() and std::return_temporary_buffer() are deprecated in C++17. " \
                 "You can define _SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING "                      \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_TEMPORARY_BUFFER
#endif // ^^^ warning disabled ^^^

// N4659 D.12 [depr.meta.types]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_IS_LITERAL_TYPE                                                     \
    [[deprecated("warning STL4013: "                                                         \
                 "std::is_literal_type and std::is_literal_type_v are deprecated in C++17. " \
                 "You can define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING "        \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_IS_LITERAL_TYPE
#endif // ^^^ warning disabled ^^^

// N4659 D.12 [depr.meta.types]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_RESULT_OF                                                      \
    [[deprecated("warning STL4014: "                                                    \
                 "std::result_of and std::result_of_t are deprecated in C++17. "        \
                 "They are superseded by std::invoke_result and std::invoke_result_t. " \
                 "You can define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING "         \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_RESULT_OF
#endif // ^^^ warning disabled ^^^

// N4659 D.13 [depr.iterator.primitives]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_ITERATOR_BASE_CLASS                                                                          \
    [[deprecated(                                                                                                     \
        "warning STL4015: "                                                                                           \
        "The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17. "        \
        "(The <iterator> header is NOT deprecated.) The C++ Standard has never required user-defined iterators to "   \
        "derive from std::iterator. To fix this warning, stop deriving from std::iterator and start providing "       \
        "publicly accessible typedefs named iterator_category, value_type, difference_type, pointer, and reference. " \
        "Note that value_type is required to be non-const, even for constant iterators. "                             \
        "You can define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING "                                      \
        "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_ITERATOR_BASE_CLASS
#endif // ^^^ warning disabled ^^^

// N4659 D.14 [depr.util.smartptr.shared.obs]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_SHARED_PTR_UNIQUE                                              \
    [[deprecated("warning STL4016: "                                                    \
                 "std::shared_ptr::unique() is deprecated in C++17. "                   \
                 "You can define _SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_SHARED_PTR_UNIQUE
#endif // ^^^ warning disabled ^^^

// N4659 D.15 [depr.locale.stdcvt]
// N4659 D.16 [depr.conversions]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_CODECVT_HEADER                                                                        \
    [[deprecated(                                                                                              \
        "warning STL4017: "                                                                                    \
        "std::wbuffer_convert, std::wstring_convert, and the <codecvt> header (containing std::codecvt_mode, " \
        "std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17. "        \
        "(The std::codecvt class template is NOT deprecated.) "                                                \
        "The C++ Standard doesn't provide equivalent non-deprecated functionality; "                           \
        "consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead. "            \
        "You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING "                                    \
        "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX17_DEPRECATE_CODECVT_HEADER
#endif // ^^^ warning disabled ^^^

// STL4018 was "The non-Standard std::tr2::sys namespace is deprecated and will be REMOVED."

#ifdef _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING
#define _DEPRECATE_FPOS_SEEKPOS
#else // ^^^ warning disabled / warning enabled vvv
#define _DEPRECATE_FPOS_SEEKPOS                                                                                        \
    [[deprecated("warning STL4019: "                                                                                   \
                 "The member std::fpos::seekpos() is non-Standard, and is preserved only for compatibility with "      \
                 "workarounds for old versions of Visual C++. It will be removed in a future release, and in this "    \
                 "release always returns 0. Please use standards-conforming mechanisms to manipulate fpos, such as "   \
                 "conversions to and from streamoff, or an integral type, instead. If you are receiving this message " \
                 "while compiling Boost.IOStreams, a fix has been submitted upstream to make Boost use "               \
                 "standards-conforming mechanisms, as it does for other compilers. You can define "                    \
                 "_SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING to acknowledge that you have received this warning, "      \
                 "or define _REMOVE_FPOS_SEEKPOS to remove std::fpos::seekpos entirely.")]]
#endif // ^^^ warning enabled ^^^

// P0482R6 Library Support For char8_t
// Other C++20 deprecation warnings

// N4810 D.16 [depr.locale.category]
#if _HAS_CXX20 && !defined(_SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS)
#define _CXX20_DEPRECATE_CODECVT_FACETS                                                                                \
    [[deprecated("warning STL4020: "                                                                                   \
                 "std::codecvt<char16_t, char, mbstate_t>, std::codecvt<char32_t, char, mbstate_t>, "                  \
                 "std::codecvt_byname<char16_t, char, mbstate_t>, and std::codecvt_byname<char32_t, char, mbstate_t> " \
                 "are deprecated in C++20 and replaced by specializations with a second argument of type char8_t. "    \
                 "You can define _SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING "                                   \
                 "or _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX20_DEPRECATE_CODECVT_FACETS
#endif // ^^^ warning disabled ^^^

// N4810 D.17 [depr.fs.path.factory]
#if _HAS_CXX20 && !defined(_SILENCE_CXX20_U8PATH_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS)
#define _CXX20_DEPRECATE_U8PATH                                                                                      \
    [[deprecated("warning STL4021: "                                                                                 \
                 "The std::filesystem::u8path() overloads are deprecated in C++20. "                                 \
                 "The constructors of std::filesystem::path provide equivalent functionality via construction from " \
                 "u8string, u8string_view, or iterators with value_type char8_t."                                    \
                 "You can define _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING "                                         \
                 "or _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX20_DEPRECATE_U8PATH
#endif // ^^^ warning disabled ^^^

#if !defined(_SILENCE_STDEXT_HASH_LOWER_BOUND_DEPRECATION_WARNING)
#define _DEPRECATE_STDEXT_HASH_LOWER_BOUND                                                                           \
    [[deprecated(                                                                                                    \
        "warning STL4022: "                                                                                          \
        "The hash_meow and unordered_meow containers' non-Standard lower_bound() member was provided for interface " \
        "compatibility with the ordered associative containers, and doesn't match the semantics of the "             \
        "hash_meow or unordered_meow containers. Please use the find() member instead. You can define "              \
        "_SILENCE_STDEXT_HASH_LOWER_BOUND_DEPRECATION_WARNING to suppress this deprecation.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _DEPRECATE_STDEXT_HASH_LOWER_BOUND
#endif // ^^^ warning disabled ^^^

#if !defined(_SILENCE_STDEXT_HASH_UPPER_BOUND_DEPRECATION_WARNING)
#define _DEPRECATE_STDEXT_HASH_UPPER_BOUND                                                                           \
    [[deprecated(                                                                                                    \
        "warning STL4023: "                                                                                          \
        "The hash_meow and unordered_meow containers' non-Standard upper_bound() member was provided for interface " \
        "compatibility with the ordered associative containers, and doesn't match the semantics of the "             \
        "hash_meow or unordered_meow containers. Please use the second iterator returned by the "                    \
        "equal_range() member instead. You can define "                                                              \
        "_SILENCE_STDEXT_HASH_UPPER_BOUND_DEPRECATION_WARNING to suppress this deprecation.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _DEPRECATE_STDEXT_HASH_UPPER_BOUND
#endif // ^^^ warning disabled ^^^

// P0966R1 [depr.string.capacity]
#if _HAS_CXX20 && !defined(_SILENCE_CXX20_STRING_RESERVE_WITHOUT_ARGUMENT_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS)
#define _CXX20_DEPRECATE_STRING_RESERVE_WITHOUT_ARGUMENT                                                             \
    [[deprecated("warning STL4024: "                                                                                 \
                 "std::string::reserve() without an argument is deprecated in C++20. "                               \
                 "To shrink the string's capacity, use std::string::shrink_to_fit() instead. Otherwise, provide an " \
                 "argument to std::string::reserve(). "                                                              \
                 "You can define _SILENCE_CXX20_STRING_RESERVE_WITHOUT_ARGUMENT_DEPRECATION_WARNING "                \
                 "or _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX20_DEPRECATE_STRING_RESERVE_WITHOUT_ARGUMENT
#endif // ^^^ warning disabled ^^^

// P0767R1 [depr.meta.types]
#if _HAS_CXX20 && !defined(_SILENCE_CXX20_IS_POD_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS)
#define _CXX20_DEPRECATE_IS_POD                                                                                     \
    [[deprecated("warning STL4025: "                                                                                \
                 "std::is_pod and std::is_pod_v are deprecated in C++20. "                                          \
                 "The std::is_trivially_copyable and/or std::is_standard_layout traits likely suit your use case. " \
                 "You can define _SILENCE_CXX20_IS_POD_DEPRECATION_WARNING "                                        \
                 "or _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _CXX20_DEPRECATE_IS_POD
#endif // ^^^ warning disabled ^^^

#if _HAS_CXX20 && !defined(_SILENCE_EXPERIMENTAL_ERASE_DEPRECATION_WARNING)
#define _DEPRECATE_EXPERIMENTAL_ERASE                                                                                 \
    [[deprecated("warning STL4026: "                                                                                  \
                 "std::experimental::erase() and std::experimental::erase_if() are deprecated by Microsoft and will " \
                 "be REMOVED. They are superseded by std::erase() and std::erase_if(). "                              \
                 "You can define _SILENCE_EXPERIMENTAL_ERASE_DEPRECATION_WARNING to acknowledge that you have "       \
                 "received this warning.")]]
#else // ^^^ warning enabled / warning disabled vvv
#define _DEPRECATE_EXPERIMENTAL_ERASE
#endif // ^^^ warning disabled ^^^

// next warning number: STL4027


// LIBRARY FEATURE-TEST MACROS

// C++14
#define __cpp_lib_chrono_udls                 201304L
#define __cpp_lib_complex_udls                201309L
#define __cpp_lib_exchange_function           201304L
#define __cpp_lib_generic_associative_lookup  201304L
#define __cpp_lib_integer_sequence            201304L
#define __cpp_lib_integral_constant_callable  201304L
#define __cpp_lib_is_final                    201402L
#define __cpp_lib_is_null_pointer             201309L
#define __cpp_lib_make_reverse_iterator       201402L
#define __cpp_lib_make_unique                 201304L
#define __cpp_lib_null_iterators              201304L
#define __cpp_lib_quoted_string_io            201304L
#define __cpp_lib_result_of_sfinae            201210L
#define __cpp_lib_robust_nonmodifying_seq_ops 201304L
#ifndef _M_CEE
#define __cpp_lib_shared_timed_mutex 201402L
#endif // _M_CEE
#define __cpp_lib_string_udls                  201304L
#define __cpp_lib_transformation_trait_aliases 201304L
#define __cpp_lib_tuple_element_t              201402L
#define __cpp_lib_tuples_by_type               201304L

// C++17
#define __cpp_lib_addressof_constexpr              201603L
#define __cpp_lib_allocator_traits_is_always_equal 201411L
#define __cpp_lib_as_const                         201510L
#define __cpp_lib_bool_constant                    201505L
#define __cpp_lib_enable_shared_from_this          201603L
#define __cpp_lib_incomplete_container_elements    201505L
#define __cpp_lib_invoke                           201411L
#define __cpp_lib_logical_traits                   201510L
#define __cpp_lib_map_try_emplace                  201411L
#define __cpp_lib_nonmember_container_access       201411L
#define __cpp_lib_shared_mutex                     201505L
#define __cpp_lib_shared_ptr_arrays                201611L
#define __cpp_lib_transparent_operators            201510L
#define __cpp_lib_type_trait_variable_templates    201510L
#define __cpp_lib_uncaught_exceptions              201411L
#define __cpp_lib_unordered_map_try_emplace        201411L
#define __cpp_lib_void_t                           201411L

#if _HAS_CXX17
#define __cpp_lib_any                        201606L
#define __cpp_lib_apply                      201603L
#define __cpp_lib_array_constexpr            201803L
#define __cpp_lib_atomic_is_always_lock_free 201603L
#define __cpp_lib_boyer_moore_searcher       201603L
#if _HAS_STD_BYTE
#define __cpp_lib_byte 201603L
#endif // _HAS_STD_BYTE
#define __cpp_lib_chrono 201611L
#define __cpp_lib_clamp  201603L
#ifndef _M_CEE
#define __cpp_lib_execution 201603L
#endif // _M_CEE
#define __cpp_lib_filesystem                        201703L
#define __cpp_lib_gcd_lcm                           201606L
#define __cpp_lib_hardware_interference_size        201703L
#define __cpp_lib_has_unique_object_representations 201606L
#define __cpp_lib_hypot                             201603L
#define __cpp_lib_is_aggregate                      201703L
#define __cpp_lib_is_invocable                      201703L
#define __cpp_lib_is_swappable                      201603L
#define __cpp_lib_launder                           201606L
#define __cpp_lib_make_from_tuple                   201606L
#define __cpp_lib_math_special_functions            201603L
#define __cpp_lib_memory_resource                   201603L
#define __cpp_lib_node_extract                      201606L
#define __cpp_lib_not_fn                            201603L
#define __cpp_lib_optional                          201606L
#ifndef _M_CEE
#define __cpp_lib_parallel_algorithm 201603L
#endif // _M_CEE
#define __cpp_lib_raw_memory_algorithms 201606L
#define __cpp_lib_sample                201603L
#define __cpp_lib_scoped_lock           201703L
#define __cpp_lib_shared_ptr_weak_type  201606L
#define __cpp_lib_string_view           201803L
#define __cpp_lib_to_chars              201611L
#define __cpp_lib_variant               201606L
#else // _HAS_CXX17
#define __cpp_lib_chrono 201510L
#endif // _HAS_CXX17

// C++20
#if _HAS_CXX20
#define __cpp_lib_atomic_float 201711L
#define __cpp_lib_bind_front   201907L

#if defined(__clang__) || defined(__EDG__)
#define __cpp_lib_bitops 201907L
#else // ^^^ Clang and EDG / MSVC vvv
// a future MSVC update will embed CPU feature detection into <bit> intrinsics
// TRANSITION, VSO-1020212
#endif // defined(__clang__) || defined(__EDG__)

#define __cpp_lib_bounded_array_traits 201902L

#ifdef __cpp_char8_t
#define __cpp_lib_char8_t 201811L
#endif // __cpp_char8_t

#if defined(__cpp_concepts) && __cpp_concepts > 201507L
#define __cpp_lib_concepts 201907L

// P0898R3 (as modified by P1754R1) std::boolean
#ifndef _HAS_STD_BOOLEAN
#define _HAS_STD_BOOLEAN 1
#endif // _HAS_STD_BOOLEAN
#endif // defined(__cpp_concepts) && __cpp_concepts > 201507L

#define __cpp_lib_endian                   201907L
#define __cpp_lib_erase_if                 201811L
#define __cpp_lib_generic_unordered_lookup 201811L
#define __cpp_lib_int_pow2                 201806L

#if defined(__clang__) || defined(__EDG__) || (defined(_MSC_VER) && _MSC_VER >= 1925)
#define __cpp_lib_is_constant_evaluated 201811L
#endif // TRANSITION, VS 2019 16.5 Preview 2

#define __cpp_lib_list_remove_return_type 201806L
#define __cpp_lib_math_constants          201907L
#define __cpp_lib_nothrow_convertible     201806L
#define __cpp_lib_remove_cvref            201711L
#define __cpp_lib_shift                   201806L
#define __cpp_lib_ssize                   201902L
#define __cpp_lib_starts_ends_with        201711L
#define __cpp_lib_to_address              201711L
#define __cpp_lib_to_array                201907L
#define __cpp_lib_type_identity           201806L
#define __cpp_lib_unwrap_ref              201811L
#endif // _HAS_CXX20

// EXPERIMENTAL
#define __cpp_lib_experimental_erase_if   201411L
#define __cpp_lib_experimental_filesystem 201406L


#ifdef _RTC_CONVERSION_CHECKS_ENABLED
#ifndef _ALLOW_RTCc_IN_STL
#error /RTCc rejects conformant code, so it is not supported by the C++ Standard Library. Either remove this \
compiler option, or define _ALLOW_RTCc_IN_STL to acknowledge that you have received this warning.
#endif // _ALLOW_RTCc_IN_STL
#endif // _RTC_CONVERSION_CHECKS_ENABLED

#ifndef _DECLSPEC_ALLOCATOR
#ifdef __clang__
#define _DECLSPEC_ALLOCATOR
#else // ^^^ Clang / non-Clang vvv
#define _DECLSPEC_ALLOCATOR __declspec(allocator)
#endif // ^^^ non-Clang ^^^
#endif // _DECLSPEC_ALLOCATOR

#define _STRINGIZEX(x)  #x
#define _STRINGIZE(x)   _STRINGIZEX(x)
#define _EMPTY_ARGUMENT // for empty macro argument

// NAMESPACE
#define _STD_BEGIN namespace std {
#define _STD_END   }
#define _STD       ::std::
#define _RANGES    ::std::ranges::

// We use the stdext (standard extension) namespace to contain extensions that are not part of the current standard
#define _STDEXT_BEGIN namespace stdext {
#define _STDEXT_END   }
#define _STDEXT       ::stdext::

#ifdef __cplusplus
#define _CSTD ::

#define _EXTERN_C     extern "C" {
#define _END_EXTERN_C }
#else // ^^^ __cplusplus / !__cplusplus vvv
#define _CSTD

#define _EXTERN_C
#define _END_EXTERN_C
#endif // __cplusplus

#ifdef _M_CEE_PURE
#define _EXTERN_C_UNLESS_PURE
#define _END_EXTERN_C_UNLESS_PURE
#define _STATIC_UNLESS_PURE // Avoid warning C4640: construction of local static object is not thread-safe (/Wall)
#else // ^^^ _M_CEE_PURE / !_M_CEE_PURE vvv
#define _EXTERN_C_UNLESS_PURE     _EXTERN_C
#define _END_EXTERN_C_UNLESS_PURE _END_EXTERN_C
#define _STATIC_UNLESS_PURE       static
#endif // _M_CEE_PURE

#if defined(MRTDLL) && !defined(_CRTBLD)
#error In yvals_core.h, defined(MRTDLL) implies defined(_CRTBLD); !defined(_CRTBLD) implies !defined(MRTDLL)
#endif // defined(MRTDLL) && !defined(_CRTBLD)

#if defined(MRTDLL) && !defined(_M_CEE_PURE)
#error In yvals_core.h, defined(MRTDLL) implies defined(_M_CEE_PURE); !defined(_M_CEE_PURE) implies !defined(MRTDLL)
#endif // defined(MRTDLL) && !defined(_M_CEE_PURE)

#endif // _STL_COMPILER_PREPROCESSOR
#endif // _YVALS_CORE_H_
