// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <concepts>
#include <mdspan>
#include <type_traits>
#include <utility>

enum class IsNothrow : bool { no, yes };

template <class Int, IsNothrow Nothrow = IsNothrow::yes>
struct ConvertibleToInt {
    constexpr operator Int() const noexcept(std::to_underlying(Nothrow)) {
        return Int{1};
    }
};

struct NonConvertibleToAnything {};

template <class T>
constexpr void check_implicit_conversion(T); // not defined

// clang-format off
template <class T, class... Args>
concept NotImplicitlyConstructibleFrom =
    std::constructible_from<T, Args...>
    && !requires(Args&&... args) { check_implicit_conversion<T>({std::forward<Args>(args)...}); };
// clang-format on

template <class T>
inline constexpr bool is_extents_v = false;

template <class T, size_t... E>
inline constexpr bool is_extents_v<std::extents<T, E...>> = true;

template <class Layout, class Mapping>
inline constexpr bool is_mapping_of_v =
    std::is_same_v<typename Layout::template mapping<typename Mapping::extents_type>, Mapping>;

template <class M>
concept CheckNestedTypesOfLayoutMapping =
    requires {
        requires is_extents_v<typename M::extents_type>;
        requires std::same_as<typename M::index_type, typename M::extents_type::index_type>;
        requires std::same_as<typename M::rank_type, typename M::extents_type::rank_type>;
        requires is_mapping_of_v<typename M::layout_type, M>;
    };

template <class M>
concept CheckMemberFunctionsOfLayoutMapping = requires(const M m) {
                                                  { m.extents() } -> std::same_as<const typename M::extents_type&>;
                                                  { m.required_span_size() } -> std::same_as<typename M::index_type>;
                                                  { m.is_unique() } -> std::same_as<bool>;
                                                  { m.is_exhaustive() } -> std::same_as<bool>;
                                                  { m.is_strided() } -> std::same_as<bool>;
                                              };

template <class M>
concept CheckStaticFunctionsOfLayoutMapping = requires(const M m) {
                                                  { M::is_always_strided() } -> std::same_as<bool>;
                                                  { M::is_always_exhaustive() } -> std::same_as<bool>;
                                                  { M::is_always_unique() } -> std::same_as<bool>;
                                                  std::bool_constant<M::is_always_strided()>::value;
                                                  std::bool_constant<M::is_always_exhaustive()>::value;
                                                  std::bool_constant<M::is_always_unique()>::value;
                                              };

// clang-format off
template <class M, class... Indices>
concept CheckCallOperatorOfLayoutMapping =
    requires(const M m, Indices... i) {
        { m(i...) } -> std::same_as<typename M::index_type>;
        { m(i...) == m(static_cast<typename M::index_type>(i)...) } -> std::same_as<bool>;
    };
// clang-format on

template <class M>
concept CheckStrideMemberFunc = requires(M mapping, typename M::rank_type i) {
                                    { mapping.stride(i) } -> std::same_as<typename M::index_type>;
                                };

template <class M>
constexpr bool check_layout_mapping_requirements() {
    static_assert(std::copyable<M>);
    static_assert(std::equality_comparable<M>);
    static_assert(std::is_nothrow_move_constructible_v<M>);
    static_assert(std::is_nothrow_move_assignable_v<M>);
    static_assert(std::is_nothrow_swappable_v<M>);
    static_assert(CheckNestedTypesOfLayoutMapping<M>);
    static_assert(CheckMemberFunctionsOfLayoutMapping<M>);
    static_assert(CheckStaticFunctionsOfLayoutMapping<M>);

    []<size_t... Indices>(std::index_sequence<Indices...>) {
        static_assert(CheckCallOperatorOfLayoutMapping<M, decltype(Indices)...>);
    }
    (std::make_index_sequence<M::extents_type::rank()>{});

    if constexpr (requires(M m, typename M::rank_type i) { m.stride(i); }) {
        static_assert(CheckStrideMemberFunc<M>);
    }

    return true;
}

template <class MP, class E>
    requires is_extents_v<E>
constexpr bool check_layout_mapping_policy_requirements() {
    using X = typename MP::template mapping<E>;
    static_assert(check_layout_mapping_requirements<X>());
    static_assert(std::same_as<typename X::layout_type, MP>);
    static_assert(std::same_as<typename X::extents_type, E>);
    return true;
}
