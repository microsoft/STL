// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <mdspan>
#include <span>
#include <type_traits>
#include <utility>

#include "test_mdspan_support.hpp"

template <class IndexType, size_t... Extents, size_t... Indices>
constexpr void do_check_members(index_sequence<Indices...>) {
    using Ext = extents<IndexType, Extents...>;

    // Each specialization of extents models regular and is trivially copyable
    static_assert(regular<Ext>);
    static_assert(is_trivially_copyable_v<Ext>);

    // Check member types
    static_assert(same_as<typename Ext::index_type, IndexType>);
    static_assert(same_as<typename Ext::size_type, make_unsigned_t<IndexType>>);
    static_assert(same_as<typename Ext::rank_type, size_t>);

    // Check static observers
    static_assert(Ext::rank() == sizeof...(Extents));
    static_assert(Ext::rank_dynamic() == ((Extents == dynamic_extent) + ... + 0));
    static_assert(((Ext::static_extent(Indices) == Extents) && ...));

    // Check noexceptness of static observers
    static_assert(noexcept(Ext::rank()));
    static_assert(noexcept(Ext::rank_dynamic()));
    static_assert(noexcept(Ext::static_extent(0)));

    // Check default constructor
    Ext ext;
    static_assert(is_nothrow_default_constructible_v<Ext>);

    // Check 'extent' observer
    assert(
        (((cmp_equal(ext.extent(Indices), Extents) && Extents != dynamic_extent) || ext.extent(Indices) == 0) && ...));

    using OtherIndexType = conditional_t<is_signed_v<IndexType>, long long, unsigned long long>;
    using Ext2           = extents<OtherIndexType, Extents...>;

    { // Check construction from other extents
        Ext2 ext2{ext};
        assert(((ext.extent(Indices) == ext2.extent(Indices)) && ...));
        assert(ext == ext2);
        static_assert(is_nothrow_constructible_v<Ext2, Ext>);
        // Other tests are defined in 'check_construction_from_other_extents' function
    }

    { // Check construction from extents pack
        Ext2 ext2{ext.extent(Indices)...};
        assert(((ext.extent(Indices) == ext2.extent(Indices)) && ...));
        assert(ext == ext2);
        static_assert(is_nothrow_constructible_v<Ext2, Ext>);
        // Other tests are defined in 'check_construction_from_extents_pack' function
    }

    { // Check construction from array and span
        array<IndexType, Ext::rank()> arr = {ext.extent(Indices)...};
        Ext2 ext2a{arr};
        assert(((ext.extent(Indices) == ext2a.extent(Indices)) && ...));
        assert(ext == ext2a);
        static_assert(is_nothrow_constructible_v<Ext2, decltype(arr)>);

        span s{arr};
        Ext2 ext2b{s};
        assert(((ext.extent(Indices) == ext2b.extent(Indices)) && ...));
        assert(ext == ext2b);
        static_assert(is_nothrow_constructible_v<Ext2, decltype(s)>);
        // Other tests are defined in 'check_construction_from_array_and_span' function
    }
}

template <class IndexType, size_t... Extents>
constexpr void check_members() {
    do_check_members<IndexType, Extents...>(make_index_sequence<sizeof...(Extents)>{});
}

constexpr void check_construction_from_other_extents() {
    { // Check construction from too big or too small other extents
        using Ext = extents<int, 3, 3>;
        static_assert(!is_constructible_v<Ext, int>);
        static_assert(!is_constructible_v<Ext, extents<int, 3, 3, 3>>);
    }

    { // Check construction with different values
        static_assert(is_nothrow_constructible_v<extents<int, 3, 3>, extents<int, 3, 3>>);
        static_assert(is_nothrow_constructible_v<extents<int, 3, 3>, extents<int, 3, dynamic_extent>>);
        static_assert(is_nothrow_constructible_v<extents<int, 3, dynamic_extent>, extents<int, 3, 3>>);
        static_assert(is_nothrow_constructible_v<extents<int, 3, dynamic_extent>, extents<int, 3, dynamic_extent>>);
        static_assert(!is_constructible_v<extents<int, 3, 3>, extents<int, 3, 2>>);
    }

    { // Check postconditions
        extents<int, dynamic_extent, dynamic_extent> ext{4, 4};
        extents<int, 4, 4> ext2{ext};
        assert(ext == ext2);
        assert(ext2.extent(0) == 4);
        assert(ext2.extent(1) == 4);

        extents<long, 4, dynamic_extent> ext3{ext};
        assert(ext == ext3);
        assert(ext3.extent(0) == 4);
        assert(ext3.extent(1) == 4);
    }

    { // Check implicit conversions
        static_assert(!NotImplicitlyConstructibleFrom<extents<int, 3>, extents<int, 3>>);
        static_assert(NotImplicitlyConstructibleFrom<extents<int, 3>, extents<long long, 3>>);
        static_assert(NotImplicitlyConstructibleFrom<extents<int, 3, 3>, extents<int, dynamic_extent, 3>>);
        static_assert(NotImplicitlyConstructibleFrom<extents<int, 3, 3>, extents<int, dynamic_extent, dynamic_extent>>);
    }
}

constexpr void check_construction_from_extents_pack() {
    { // Check construction from various types
        using Ext = extents<int, 3, 3, dynamic_extent, dynamic_extent>;
        static_assert(is_nothrow_constructible_v<Ext, int, int>);
        static_assert(!is_constructible_v<Ext, int, int, int>);
        static_assert(is_nothrow_constructible_v<Ext, int, int, int, int>);
    }

    { // Check construction from types (not) convertible to index_type
        using Ext = extents<long, 2, dynamic_extent>;
        static_assert(is_nothrow_constructible_v<Ext, ConvertibleToInt<int>>);
        static_assert(!is_constructible_v<Ext, NonConvertibleToAnything>);
        static_assert(is_nothrow_constructible_v<Ext, int, ConvertibleToInt<int>>);
        static_assert(!is_constructible_v<Ext, int, NonConvertibleToAnything>);
    }

    { // Check construction from types that may throw during conversion to index_type
        using Ext = extents<long long, 4, dynamic_extent>;
        static_assert(!is_constructible_v<Ext, ConvertibleToInt<int, IsNothrow::no>>);
        static_assert(!is_constructible_v<Ext, int, ConvertibleToInt<int, IsNothrow::no>>);
    }

    { // Check postconditions
        using Ext = extents<int, dynamic_extent, dynamic_extent, 4>;
        Ext ext1a{4, ConvertibleToInt<int>{}, 4};
        Ext ext1b{4, 1, 4};
        assert(ext1a == ext1b);

        Ext ext2a{4, ConvertibleToInt<int>{}};
        Ext ext2b{4, 1};
        assert(ext2a == ext2b);
    }

    { // Check construction from integers with mismatched signs
        using Ext = extents<long long, dynamic_extent>;
        (void) Ext{4ull};
    }

    { // Check narrowing conversions
        using Ext = extents<short, dynamic_extent>;
        (void) Ext{4ll};
    }

    { // Check implicit conversions
        static_assert(NotImplicitlyConstructibleFrom<extents<int, 3>, unsigned long long>);
        static_assert(NotImplicitlyConstructibleFrom<extents<int, 3, 3>, long, long>);
        static_assert(NotImplicitlyConstructibleFrom<extents<int, 3, 3, 3>, char, signed char, unsigned char>);
    }
}

constexpr void check_construction_from_array_and_span() {
    { // Check construction from arrays/spans where [array/span].size() is equal to rank()
        using Ext = extents<short, 1, dynamic_extent>;

        array<int, 2> arr1 = {1, 5};
        Ext ext1a{arr1};
        span s1{arr1};
        Ext ext1b{s1};
        assert(ext1a == ext1b);
        static_assert(is_nothrow_constructible_v<Ext, decltype(arr1)>);
        static_assert(is_nothrow_constructible_v<Ext, decltype(s1)>);

        array<ConvertibleToInt<int>, 2> arr2;
        Ext ext2a{arr2};
        span s2{arr2};
        Ext ext2b{s2};
        assert(ext2a == ext2b);
        static_assert(is_nothrow_constructible_v<Ext, decltype(arr2)>);
        static_assert(is_nothrow_constructible_v<Ext, decltype(s2)>);

        static_assert(!is_constructible_v<Ext, array<NonConvertibleToAnything, 2>>);
        static_assert(!is_constructible_v<Ext, span<NonConvertibleToAnything, 2>>);
    }

    { // Check construction from arrays/spans where [array/span].size() is equal to rank_dynamic()
        using Ext = extents<unsigned char, 3, dynamic_extent, 3, dynamic_extent>;

        array<int, 2> arr1 = {4, 4};
        Ext ext1a{arr1};
        span s1{arr1};
        Ext ext1b{s1};
        assert(ext1a == ext1b);
        static_assert(is_nothrow_constructible_v<Ext, decltype(arr1)>);
        static_assert(is_nothrow_constructible_v<Ext, decltype(s1)>);

        array<ConvertibleToInt<int>, 2> arr2;
        Ext ext2a{arr2};
        span s2{arr2};
        Ext ext2b{s2};
        assert(ext2a == ext2b);
        static_assert(is_nothrow_constructible_v<Ext, decltype(arr2)>);
        static_assert(is_nothrow_constructible_v<Ext, decltype(s2)>);

        static_assert(!is_constructible_v<Ext, array<NonConvertibleToAnything, 2>>);
        static_assert(!is_constructible_v<Ext, span<NonConvertibleToAnything, 2>>);
    }

    { // Check construction with integers with mismatched signs
        using Ext = extents<long long, dynamic_extent>;

        array arr = {4ull};
        (void) Ext{arr};

        span s{arr};
        (void) Ext{s};
    }

    { // Check narrowing conversions
        using Ext = extents<short, dynamic_extent>;

        array arr = {4ll};
        (void) Ext{arr};

        span s{arr};
        (void) Ext{s};
    }

    { // Check construction from arrays/spans with elements that may throw during conversion to index_type
        using Ext = extents<long, dynamic_extent, dynamic_extent>;
        static_assert(!is_constructible_v<Ext, array<ConvertibleToInt<int, IsNothrow::no>, 2>>);
        static_assert(!is_constructible_v<Ext, span<ConvertibleToInt<int, IsNothrow::no>, 2>>);
    }

    { // Check construction from arrays/spans with invalid size
        using Ext = extents<short, 5, 5>;
        static_assert(!is_constructible_v<Ext, array<int, 1>>);
        static_assert(!is_constructible_v<Ext, array<int, 3>>);
        static_assert(!is_constructible_v<Ext, span<int, 1>>);
        static_assert(!is_constructible_v<Ext, span<int, 3>>);
        static_assert(!is_constructible_v<Ext, span<int>>);
        static_assert(!is_constructible_v<Ext, span<int>>);
    }

    { // Check implicit conversions
        static_assert(!NotImplicitlyConstructibleFrom<extents<signed char, 3, dynamic_extent>, array<int, 1>>);
        static_assert(NotImplicitlyConstructibleFrom<extents<signed char, 3, dynamic_extent>, array<int, 2>>);
        static_assert(!NotImplicitlyConstructibleFrom<extents<signed char, 3, dynamic_extent>, span<int, 1>>);
        static_assert(NotImplicitlyConstructibleFrom<extents<signed char, 3, dynamic_extent>, span<int, 2>>);
    }
}

constexpr void check_equality_operator() {
    { // All extents are static
        extents<int, 3, 3> e1;
        extents<long, 2, 3> e2;
        extents<size_t, 3, 3> e3;
        assert(e1 != e2);
        assert(e2 != e3);
        assert(e1 == e3);
    }

    { // Some extents are static, some dynamic
        extents<int, 3, dynamic_extent> e1{1};
        extents<long, dynamic_extent, 3> e2{2};
        extents<size_t, 2, dynamic_extent> e3{3};
        assert(e1 != e2);
        assert(e2 == e3);
        assert(e1 != e2);
    }

    { // All extents are dynamic
        dextents<int, 2> e1{1, 2};
        dextents<long, 2> e2{1, 2};
        dextents<size_t, 2> e3{1, 3};
        assert(e1 == e2);
        assert(e2 != e3);
        assert(e1 != e3);
    }
}

constexpr bool test() {
    check_members<short>();
    check_members<int, 1, 2, 3>();
    check_members<unsigned short, 4, 4>();
    check_members<unsigned long long, dynamic_extent, 4, 5>();
    check_members<short, dynamic_extent, dynamic_extent, 6>();
    check_members<unsigned char, dynamic_extent, dynamic_extent, dynamic_extent>();
    check_construction_from_other_extents();
    check_construction_from_extents_pack();
    check_construction_from_array_and_span();
    check_equality_operator();
    return true;
}

template <class T, size_t ExpectedRank>
constexpr bool all_extents_dynamic = false;

template <class IndexType, size_t... Extents, size_t ExpectedRank>
constexpr bool all_extents_dynamic<extents<IndexType, Extents...>, ExpectedRank> =
    ((Extents == dynamic_extent) && ...) && (sizeof...(Extents) == ExpectedRank);

template <class... Args>
concept CanDeduceExtents = requires(Args&&... args) { extents{forward<Args>(args)...}; };

// Check deduction guide
using DG = decltype(extents{'1', 2, 3u, 4ll, ConvertibleToInt<size_t>{}});
static_assert(all_extents_dynamic<DG, 5>);
static_assert(same_as<DG::index_type, size_t>);
static_assert(!CanDeduceExtents<int, long, short, NonConvertibleToAnything>);

// Check dextents
static_assert(all_extents_dynamic<dextents<signed char, 0>, 0>);
static_assert(all_extents_dynamic<dextents<unsigned short, 2>, 2>);
static_assert(all_extents_dynamic<dextents<int, 3>, 3>);
static_assert(all_extents_dynamic<dextents<unsigned long, 5>, 5>);

int main() {
    static_assert(test());
    test();
}
