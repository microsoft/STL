// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <mdspan>
#include <optional>
#include <ranges>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <is_permissive.hpp>

enum class IsExplicit : bool { no, yes };
enum class IsNothrow : bool { no, yes };

template <std::integral Int, IsNothrow Nothrow = IsNothrow::yes, IsExplicit Explicit = IsExplicit::no>
struct ConvertibleToInt {
    Int val = 1;

    constexpr explicit(std::to_underlying(Explicit)) operator Int() const noexcept(std::to_underlying(Nothrow)) {
        return val;
    }
};

static_assert(std::is_aggregate_v<ConvertibleToInt<int>>);
static_assert(std::is_convertible_v<ConvertibleToInt<int>, int>);
static_assert(std::is_nothrow_convertible_v<ConvertibleToInt<int>, int>);
static_assert(!std::is_nothrow_convertible_v<ConvertibleToInt<int, IsNothrow::no>, int>);
static_assert(std::is_convertible_v<ConvertibleToInt<int, IsNothrow::no>, int>);
static_assert(!std::is_convertible_v<ConvertibleToInt<int, IsNothrow::no, IsExplicit::yes>, int>);

template <std::integral Int>
struct NonConstConvertibleToInt {
    operator Int() noexcept; // not defined
};

static_assert(std::is_convertible_v<NonConstConvertibleToInt<int>, int>);
static_assert(!std::is_convertible_v<const NonConstConvertibleToInt<int>, int>);
static_assert(std::is_nothrow_convertible_v<NonConstConvertibleToInt<int>, int>);
static_assert(!std::is_nothrow_convertible_v<const NonConstConvertibleToInt<int>, int>);

struct NonConvertibleToAnything {};

namespace detail {
    template <class T>
    void check_implicit_conversion(T); // not defined
}

template <class T, class... Args>
concept NotImplicitlyConstructibleFrom = std::constructible_from<T, Args...> && !requires(Args&&... args) {
    detail::check_implicit_conversion<T>({std::forward<Args>(args)...});
};

namespace detail {
    template <class T>
    constexpr bool is_extents_v = false;

    template <class T, size_t... E>
    constexpr bool is_extents_v<std::extents<T, E...>> = true;

    template <class Layout, class Mapping>
    constexpr bool is_mapping_of_v =
        std::is_same_v<typename Layout::template mapping<typename Mapping::extents_type>, Mapping>;

    template <class M>
    concept CheckNestedTypesOfLayoutMapping = is_extents_v<typename M::extents_type>
                                           && std::same_as<typename M::index_type, typename M::extents_type::index_type>
                                           && std::same_as<typename M::rank_type, typename M::extents_type::rank_type>
                                           && is_mapping_of_v<typename M::layout_type, M>;

    template <class M>
    concept CheckMemberFunctionsOfLayoutMapping = requires(const M m) {
        { m.extents() } -> std::same_as<const typename M::extents_type&>;
        { m.required_span_size() } -> std::same_as<typename M::index_type>;
        { m.is_unique() } -> std::same_as<bool>;
        { m.is_exhaustive() } -> std::same_as<bool>;
        { m.is_strided() } -> std::same_as<bool>;
    };

    template <class M>
    concept CheckStaticFunctionsOfLayoutMapping = requires {
        { M::is_always_strided() } -> std::same_as<bool>;
        { M::is_always_exhaustive() } -> std::same_as<bool>;
        { M::is_always_unique() } -> std::same_as<bool>;
        std::bool_constant<M::is_always_strided()>::value;
        std::bool_constant<M::is_always_exhaustive()>::value;
        std::bool_constant<M::is_always_unique()>::value;
    };
} // namespace detail

template <class M, class... Indices>
concept CheckCallOperatorOfLayoutMapping = requires(const M m, Indices... i) {
    { m(i...) } -> std::same_as<typename M::index_type>;
    { m(i...) == m(static_cast<M::index_type>(i)...) } -> std::same_as<bool>;
};

template <class M>
concept CheckStrideMemberFunction = requires(M mapping, M::rank_type i) {
    { mapping.stride(i) } -> std::same_as<typename M::index_type>;
};

template <class M>
constexpr bool check_layout_mapping_requirements() {
    static_assert(std::copyable<M>);
    static_assert(std::equality_comparable<M>);
    static_assert(std::is_nothrow_move_constructible_v<M>);
    static_assert(std::is_nothrow_move_assignable_v<M>);
    static_assert(std::is_nothrow_swappable_v<M>);
    static_assert(detail::CheckNestedTypesOfLayoutMapping<M>);
    static_assert(detail::CheckMemberFunctionsOfLayoutMapping<M>);
    static_assert(detail::CheckStaticFunctionsOfLayoutMapping<M>);

    []<size_t... Indices>(std::index_sequence<Indices...>) {
        static_assert(CheckCallOperatorOfLayoutMapping<M, decltype(Indices)...>);
    }(std::make_index_sequence<M::extents_type::rank()>{});

    if constexpr (requires(M m, M::rank_type i) { m.stride(i); }) {
        static_assert(CheckStrideMemberFunction<M>);
    }

    return true;
}

template <class MP, class E>
    requires detail::is_extents_v<E>
constexpr bool check_layout_mapping_policy_requirements() {
    using X = MP::template mapping<E>;
    static_assert(check_layout_mapping_requirements<X>());
    static_assert(std::same_as<typename X::layout_type, MP>);
    static_assert(std::same_as<typename X::extents_type, E>);
    return true;
}

template <class A>
constexpr bool check_accessor_policy_requirements();

namespace detail {
    template <class A>
    concept CheckNestedTypesOfAccessorPolicy =
        sizeof(typename A::element_type) > 0
        && !std::is_abstract_v<typename A::element_type> && std::copyable<typename A::data_handle_type>
        && std::is_nothrow_move_constructible_v<typename A::data_handle_type>
        && std::is_nothrow_move_assignable_v<typename A::data_handle_type>
        && std::is_nothrow_swappable_v<typename A::data_handle_type>
        && std::common_reference_with<typename A::reference, typename A::element_type&&>
        && (std::same_as<typename A::offset_policy, A>
            || check_accessor_policy_requirements<typename A::offset_policy>())
        && std::constructible_from<typename A::offset_policy, const A&>
        && std::is_same_v<typename A::offset_policy::element_type, typename A::element_type>;

    template <class A>
    concept CheckMemberFunctionsOfAccessorPolicy = requires(const A a, const A::data_handle_type p, size_t i) {
        { a.access(p, i) } -> std::same_as<typename A::reference>;
        { a.offset(p, i) } -> std::same_as<typename A::offset_policy::data_handle_type>;
    };
} // namespace detail

template <class A>
constexpr bool check_accessor_policy_requirements() {
    static_assert(std::copyable<A>);
    static_assert(std::is_nothrow_move_constructible_v<A>);
    static_assert(std::is_nothrow_move_assignable_v<A>);
    static_assert(std::is_nothrow_swappable_v<A>);
    static_assert(detail::CheckNestedTypesOfAccessorPolicy<A>);
    static_assert(detail::CheckMemberFunctionsOfAccessorPolicy<A>);
    return true;
}

template <class ElementType>
struct TrivialAccessor {
    using offset_policy    = TrivialAccessor;
    using element_type     = ElementType;
    using reference        = ElementType&;
    using data_handle_type = ElementType*;

    constexpr reference access(data_handle_type handle, std::size_t off) const noexcept {
        return handle[off];
    }

    constexpr data_handle_type offset(data_handle_type handle, std::size_t off) const noexcept {
        return handle + off;
    }

    int member;
};

static_assert(check_accessor_policy_requirements<TrivialAccessor<int>>());
static_assert(std::is_trivially_copyable_v<TrivialAccessor<int>>);
static_assert(std::is_trivially_default_constructible_v<TrivialAccessor<int>>);

namespace detail {
    template <size_t... Extents, class Fn>
    constexpr void check_members_with_mixed_extents(Fn&& fn) {
        auto select_extent = [](size_t e) consteval {
            return e == std::dynamic_extent ? (std::min) (sizeof...(Extents), size_t{3}) : e;
        };

        // Check signed integers
        fn(std::extents<signed char, Extents...>{select_extent(Extents)...});
        fn(std::extents<short, Extents...>{select_extent(Extents)...});
        fn(std::extents<int, Extents...>{select_extent(Extents)...});
        fn(std::extents<long, Extents...>{select_extent(Extents)...});
        fn(std::extents<long long, Extents...>{select_extent(Extents)...});

        // Check unsigned integers
        fn(std::extents<unsigned char, Extents...>{select_extent(Extents)...});
        fn(std::extents<unsigned short, Extents...>{select_extent(Extents)...});
        fn(std::extents<unsigned int, Extents...>{select_extent(Extents)...});
        fn(std::extents<unsigned long, Extents...>{select_extent(Extents)...});
        fn(std::extents<unsigned long long, Extents...>{select_extent(Extents)...});
    }

    template <class Fn, size_t... Seq>
    constexpr void check_members_with_various_extents_impl(Fn&& fn, std::index_sequence<Seq...>) {
        auto static_or_dynamic = [](size_t i) consteval {
            return i == 0 ? std::dynamic_extent : (std::min) (sizeof...(Seq), size_t{3});
        };

        if constexpr (sizeof...(Seq) <= 1) {
            check_members_with_mixed_extents<>(std::forward<Fn>(fn));
        } else if constexpr (sizeof...(Seq) <= 2) {
            (check_members_with_mixed_extents<static_or_dynamic(Seq)>(std::forward<Fn>(fn)), ...);
        } else if constexpr (sizeof...(Seq) <= 4) {
            (check_members_with_mixed_extents<static_or_dynamic(Seq & 0x2), static_or_dynamic(Seq & 0x1)>(
                 std::forward<Fn>(fn)),
                ...);
        } else if constexpr (sizeof...(Seq) <= 8) {
            (check_members_with_mixed_extents<static_or_dynamic(Seq & 0x4), static_or_dynamic(Seq & 0x2),
                 static_or_dynamic(Seq & 0x1)>(std::forward<Fn>(fn)),
                ...);
        } else if constexpr (sizeof...(Seq) <= 16) {
            (check_members_with_mixed_extents<static_or_dynamic(Seq & 0x8), static_or_dynamic(Seq & 0x4),
                 static_or_dynamic(Seq & 0x2), static_or_dynamic(Seq & 0x1)>(std::forward<Fn>(fn)),
                ...);
        } else {
            static_assert(sizeof...(Seq) <= 16, "We don't need more testing.");
        }
    }
} // namespace detail

template <class Fn>
constexpr void check_members_with_various_extents(Fn&& fn) {
    detail::check_members_with_various_extents_impl(std::forward<Fn>(fn), std::make_index_sequence<1>{});
    detail::check_members_with_various_extents_impl(std::forward<Fn>(fn), std::make_index_sequence<2>{});
    detail::check_members_with_various_extents_impl(std::forward<Fn>(fn), std::make_index_sequence<4>{});
    detail::check_members_with_various_extents_impl(std::forward<Fn>(fn), std::make_index_sequence<8>{});
    if (!std::is_constant_evaluated()) {
        detail::check_members_with_various_extents_impl(std::forward<Fn>(fn), std::make_index_sequence<16>{});
    }
}

template <class Mapping>
struct MappingProperties {
    Mapping::index_type req_span_size;
    bool uniqueness;
    bool exhaustiveness;
    bool strideness;
};

template <class Mapping>
    requires (!is_permissive_v<Mapping>)
MappingProperties<Mapping> get_mapping_properties(const Mapping& mapping) {
    using IndexType     = Mapping::index_type;
    constexpr auto rank = Mapping::extents_type::rank();
    constexpr std::make_index_sequence<rank> rank_indices;

    auto get_extent = [&](size_t i) {
        assert(i < rank);
        return mapping.extents().extent(i);
    };
    auto multidim_indices = [&]<size_t... Indices>(std::index_sequence<Indices...>) {
        return std::views::cartesian_product(std::views::iota(IndexType{0}, get_extent(Indices))...);
    }(rank_indices);

    auto map_index      = [&](const auto& tpl) { return std::apply(mapping, tpl); };
    auto mapped_indices = multidim_indices | std::views::transform(map_index) | std::ranges::to<std::vector>();
    std::ranges::sort(mapped_indices);

    MappingProperties<Mapping> props{};

    // Find required span size (N4950 [mdspan.layout.reqmts]/12)
    if (std::ranges::contains(std::views::iota(0u, rank) | std::views::transform(get_extent), IndexType{0})) {
        props.req_span_size = 0;
    } else {
        props.req_span_size = static_cast<IndexType>(1 + mapped_indices.back());
    }

    // Is mapping unique? (N4950 [mdspan.layout.reqmts]/14)
    props.uniqueness = std::ranges::adjacent_find(mapped_indices) == mapped_indices.end();

    // Is mapping exhaustive? (N4950 [mdspan.layout.reqmts]/16)
    props.exhaustiveness =
        std::ranges::adjacent_find(mapped_indices, [](auto x, auto y) { return y - x > 1; }) == mapped_indices.end();

    { // Is mapping strided? (N4950 [mdspan.layout.reqmts]/18)
        props.strideness = true; // assumption
        for (auto r : std::views::iota(0u, rank)) {
            std::optional<IndexType> sr;
            for (auto i : multidim_indices) {
                const auto i_plus_dr = [&]<size_t... Indices>(std::index_sequence<Indices...>) {
                    return std::array<IndexType, rank>{
                        static_cast<IndexType>(std::get<Indices>(i) + (Indices == r ? 1 : 0))...};
                }(rank_indices);

                if (i_plus_dr[r] < get_extent(r)) {
                    const auto diff = static_cast<IndexType>(map_index(i_plus_dr) - map_index(i));
                    if (!sr.has_value()) {
                        sr = diff;
                    } else if (*sr != diff) {
                        props.strideness = false;
                        break;
                    }
                }
            }

            if (!props.strideness) {
                break;
            }
        }
    }

    return props;
}

template <class Mapping>
    requires (is_permissive_v<Mapping>)
constexpr MappingProperties<Mapping> get_mapping_properties(const Mapping&) {
    return {}; // we cannot get properties in '/permissive' mode
}
