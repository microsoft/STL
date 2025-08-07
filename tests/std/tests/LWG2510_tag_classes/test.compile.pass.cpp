// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <new>
#include <scoped_allocator>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#ifndef _M_CEE_PURE
#include <future>
#include <mutex>
#endif // _M_CEE_PURE

#if _HAS_CXX17
#include <execution>
#endif // _HAS_CXX17

#if _HAS_CXX20
#include <chrono>
#include <format>
#include <iterator>
#include <ranges>
#include <stop_token>
#endif // _HAS_CXX20

#if _HAS_CXX23
#include <expected>
#include <generator>
#include <mdspan>
#include <optional>
#endif // _HAS_CXX23

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

using namespace std;

// LWG-2510 "Tag types should not be DefaultConstructible"

template <class T>
void check_implicit_default_construction(const T&); // not defined

template <class T, class = void>
constexpr bool implicitly_default_constructible = false;
template <class T>
constexpr bool implicitly_default_constructible<T, void_t<decltype(check_implicit_default_construction<T>({}))>> = true;

STATIC_ASSERT(implicitly_default_constructible<int>);
STATIC_ASSERT(implicitly_default_constructible<void*>);

STATIC_ASSERT(!implicitly_default_constructible<void>);
STATIC_ASSERT(!implicitly_default_constructible<int&>);
STATIC_ASSERT(!implicitly_default_constructible<void()>);

template <class T>
void check_tag_class() { // COMPILE-ONLY
    STATIC_ASSERT(is_default_constructible_v<T>);
    STATIC_ASSERT(!implicitly_default_constructible<T>);

    STATIC_ASSERT(is_copy_constructible_v<T>);
    STATIC_ASSERT(is_move_constructible_v<T>);
    STATIC_ASSERT(is_constructible_v<T, T&>);
    STATIC_ASSERT(is_constructible_v<T, const T>);
    STATIC_ASSERT(is_convertible_v<T&, T>);
    STATIC_ASSERT(is_convertible_v<const T&, T>);
    STATIC_ASSERT(is_convertible_v<T, T>);
    STATIC_ASSERT(is_convertible_v<const T, T>);
    STATIC_ASSERT(is_copy_assignable_v<T>);
    STATIC_ASSERT(is_move_assignable_v<T>);
    STATIC_ASSERT(is_assignable_v<T&, T&>);
    STATIC_ASSERT(is_assignable_v<T&, const T>);

    STATIC_ASSERT(is_nothrow_destructible_v<T>);

    STATIC_ASSERT(!is_polymorphic_v<T>);

    constexpr T constant_argument;
    (void) constant_argument;

    // desired properties not yet guaranteed by the Standard
    STATIC_ASSERT(is_nothrow_default_constructible_v<T>);
    STATIC_ASSERT(is_nothrow_copy_constructible_v<T>);
    STATIC_ASSERT(is_nothrow_move_constructible_v<T>);
    STATIC_ASSERT(is_nothrow_constructible_v<T, T&>);
    STATIC_ASSERT(is_nothrow_constructible_v<T, const T>);
#if _HAS_CXX20
    STATIC_ASSERT(is_nothrow_convertible_v<T&, T>);
    STATIC_ASSERT(is_nothrow_convertible_v<const T&, T>);
    STATIC_ASSERT(is_nothrow_convertible_v<T, T>);
    STATIC_ASSERT(is_nothrow_convertible_v<const T, T>);
#endif // _HAS_CXX20
    STATIC_ASSERT(is_nothrow_copy_assignable_v<T>);
    STATIC_ASSERT(is_nothrow_move_assignable_v<T>);
    STATIC_ASSERT(is_nothrow_assignable_v<T&, T&>);
    STATIC_ASSERT(is_nothrow_assignable_v<T&, const T>);

    STATIC_ASSERT(is_trivially_default_constructible_v<T>);
    STATIC_ASSERT(is_trivially_copy_constructible_v<T>);
    STATIC_ASSERT(is_trivially_move_constructible_v<T>);
    STATIC_ASSERT(is_trivially_constructible_v<T, T&>);
    STATIC_ASSERT(is_trivially_constructible_v<T, const T>);
    STATIC_ASSERT(is_trivially_copy_assignable_v<T>);
    STATIC_ASSERT(is_trivially_move_assignable_v<T>);
    STATIC_ASSERT(is_trivially_assignable_v<T&, T&>);
    STATIC_ASSERT(is_trivially_assignable_v<T&, const T>);
    STATIC_ASSERT(is_trivially_destructible_v<T>);
    STATIC_ASSERT(is_trivially_copyable_v<T>);

    STATIC_ASSERT(is_standard_layout_v<T>);
    STATIC_ASSERT(is_empty_v<T>); // only guaranteed for allocator_arg_t and piecewise_construct_t

    // ABI-specific, seemingly agreed among mainstream implementations
    STATIC_ASSERT(sizeof(T) == 1);
}

void check_standard_tags() { // COMPILE-ONLY
    check_tag_class<nothrow_t>();
    check_tag_class<allocator_arg_t>();
    check_tag_class<piecewise_construct_t>();
#ifndef _M_CEE_PURE
    check_tag_class<adopt_lock_t>();
    check_tag_class<defer_lock_t>();
    check_tag_class<try_to_lock_t>();
#endif // _M_CEE_PURE

#if _HAS_CXX17
    check_tag_class<in_place_t>();
    check_tag_class<in_place_index_t<0>>();
    check_tag_class<in_place_index_t<42>>();
    check_tag_class<in_place_index_t<static_cast<size_t>(-1)>>();
    check_tag_class<in_place_type_t<int>>();
    check_tag_class<in_place_type_t<in_place_type_t<int>>>();
    check_tag_class<in_place_type_t<void>>();
    check_tag_class<in_place_type_t<void()>>();

    // LWG-4273 "Standard execution policy types should be conventional tag class types"
    check_tag_class<execution::sequenced_policy>();
    check_tag_class<execution::parallel_policy>();
    check_tag_class<execution::parallel_unsequenced_policy>();
#endif // _HAS_CXX17

#if _HAS_CXX20
    check_tag_class<destroying_delete_t>();
    check_tag_class<nostopstate_t>();
    check_tag_class<chrono::last_spec>();

    // LWG-4273 "Standard execution policy types should be conventional tag class types"
    check_tag_class<execution::unsequenced_policy>();
#endif // _HAS_CXX20

#if _HAS_CXX23
    check_tag_class<from_range_t>();
    check_tag_class<unexpect_t>();
    // TODO: Cover sorted_equivalent_t and sorted_unique_t once flat_meow are implemented.
#endif // _HAS_CXX23

    // TODO: Update to cover newly introduced tag class types.
}

// We intentionally implement internal disambiguation tag types like standard ones.
void check_implementation_details() { // COMPILE-ONLY
    // TODO: Synchronize the check list with actual implementation details.
    check_tag_class<_Alloc_exact_args_t>();
    check_tag_class<_Alloc_unpack_tuple_t>();
    check_tag_class<_Exact_args_t>();
    check_tag_class<_Leave_proxy_unbound>();
    check_tag_class<_Move_allocator_tag>();
    check_tag_class<_One_then_variadic_args_t>();
    check_tag_class<_String_constructor_concat_tag>();
    check_tag_class<_Secret_copyability_ignoring_tag>();
    check_tag_class<_Secret_scoped_allocator_construct_tag>();
    check_tag_class<_Unforced>();
    check_tag_class<_Unpack_tuple_t>();
    check_tag_class<_Value_init_tag>();
    check_tag_class<_Zero_then_variadic_args_t>();
#ifndef _M_CEE_PURE
    check_tag_class<_From_raw_state_tag>();
#endif // _M_CEE_PURE

#if _HAS_CXX17
    check_tag_class<_No_init_tag>();
    check_tag_class<_Not_fn_tag>();
#endif // _HAS_CXX17

#if _HAS_CXX20
    check_tag_class<_Auto_id_tag>();
    check_tag_class<_For_overwrite_tag>();
    check_tag_class<_Variantish_empty_tag>();
    check_tag_class<chrono::_Secret_time_zone_construct_tag>();
    check_tag_class<chrono::_Secret_time_zone_link_construct_tag>();
    check_tag_class<chrono::_Secret_tzdb_list_construct_tag>();
    check_tag_class<ranges::_Construct_tag>();
#endif // _HAS_CXX20

#if _HAS_CXX23
    check_tag_class<_Construct_expected_from_invoke_result_tag>();
    check_tag_class<_Construct_from_invoke_result_tag>();
    check_tag_class<_Extents_from_tuple>();
    check_tag_class<_Gen_detail::_Secret_tag>();
#endif // _HAS_CXX23
}
