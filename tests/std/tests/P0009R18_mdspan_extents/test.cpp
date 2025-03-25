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

#include <test_mdspan_support.hpp>

using namespace std;

template <class IndexType, size_t... Extents, size_t... Indices>
constexpr void check_members(index_sequence<Indices...>) {
    using Ext = extents<IndexType, Extents...>;

    // Each specialization of extents models regular and is trivially copyable
    static_assert(regular<Ext>);
    static_assert(is_trivially_copyable_v<Ext>);

    // Check implicit properties
    static_assert(is_nothrow_copy_constructible_v<Ext>);
    static_assert(is_nothrow_move_constructible_v<Ext>);
    static_assert(is_nothrow_copy_assignable_v<Ext>);
    static_assert(is_nothrow_move_assignable_v<Ext>);
    static_assert(is_nothrow_swappable_v<Ext>);

    // Check member types
    static_assert(same_as<typename Ext::index_type, IndexType>);
    static_assert(same_as<typename Ext::size_type, make_unsigned_t<IndexType>>);
    static_assert(same_as<typename Ext::rank_type, size_t>);

    // Check static observers
    static_assert(Ext::rank() == sizeof...(Extents));
    static_assert(Ext::rank_dynamic() == (size_t{Extents == dynamic_extent} + ... + 0));
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
        static_assert(is_nothrow_constructible_v<Ext2, decltype(ext.extent(Indices))...>);
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

constexpr void check_defaulted_default_constructor() {
    { // All extents are static
        using Ext = extents<short, 3, 5, 7>;
        static_assert(is_nothrow_default_constructible_v<Ext>);

        Ext ext;
        assert(ext.extent(0) == 3);
        assert(ext.extent(1) == 5);
        assert(ext.extent(2) == 7);
    }

    { // Some extents are static, some dynamic
        using Ext = extents<signed char, dynamic_extent, dynamic_extent, 4>;
        static_assert(is_nothrow_default_constructible_v<Ext>);

        Ext ext;
        assert(ext.extent(0) == 0);
        assert(ext.extent(1) == 0);
        assert(ext.extent(2) == 4);
    }

    { // All extents are dynamic
        using Ext = dextents<unsigned long, 3>;
        static_assert(is_nothrow_default_constructible_v<Ext>);

        Ext ext;
        assert(ext.extent(0) == 0);
        assert(ext.extent(1) == 0);
        assert(ext.extent(2) == 0);
    }
}

constexpr void check_construction_from_other_extents() {
    { // Check construction from too big or too small other extents
        using Ext = extents<int, 3, 3>;
        static_assert(!is_constructible_v<Ext, extents<int, 3>>);
        static_assert(!is_constructible_v<Ext, extents<int, 3, 3, 3>>);
    }

    { // Check construction with different values
        static_assert(is_nothrow_constructible_v<extents<int, 3, 3>, extents<int, 3, 3>>);
        static_assert(is_nothrow_constructible_v<extents<int, 3, 3>, extents<int, 3, dynamic_extent>>);
        static_assert(is_nothrow_constructible_v<extents<int, 3, dynamic_extent>, extents<int, 3, 3>>);
        static_assert(is_nothrow_constructible_v<extents<int, 3, dynamic_extent>, extents<int, 3, dynamic_extent>>);
        static_assert(!is_constructible_v<extents<int, 3, 3>, extents<int, 3, 2>>);
    }

    { // Check postconditions: static extents from static extents
        extents<signed char, 3, 4> ext;
        extents<short, 3, 4> ext2 = ext;
        assert(ext2.extent(0) == 3);
        assert(ext2.extent(1) == 4);
        assert(ext == ext2);
    }

    { // Check postconditions: dynamic extents from static extents
        extents<unsigned char, 2, 3> ext;
        dextents<short, 2> ext2 = ext;
        assert(ext2.extent(0) == 2);
        assert(ext2.extent(1) == 3);
        assert(ext == ext2);
    }

    { // Check postconditions: dynamic extents from dynamic extents
        dextents<unsigned char, 2> ext{5, 10};
        dextents<unsigned short, 2> ext2 = ext;
        assert(ext2.extent(0) == 5);
        assert(ext2.extent(1) == 10);
        assert(ext == ext2);
    }

    { // Check postconditions: static extents from dynamic extents
        dextents<signed char, 2> ext{9, 6};
        extents<unsigned short, 9, 6> ext2{ext}; // NB: explicit constructor
        assert(ext2.extent(0) == 9);
        assert(ext2.extent(1) == 6);
        assert(ext == ext2);
    }

    { // Check postconditions: wider index type to narrower index type
        dextents<short, 2> ext{3, 5};
        dextents<signed char, 2> ext2{ext}; // NB: explicit constructor
        assert(ext2.extent(0) == 3);
        assert(ext2.extent(1) == 5);
        assert(ext == ext2);
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
        using Ext = extents<signed char, 3, 3, dynamic_extent, dynamic_extent>;
        static_assert(is_nothrow_constructible_v<Ext, int, int>);
        static_assert(!is_constructible_v<Ext, int, int, int>);
        static_assert(is_nothrow_constructible_v<Ext, int, int, int, int>);
    }

    { // Check construction from types (not) convertible to index_type
        using Ext = extents<long, 2, dynamic_extent>;
        static_assert(is_nothrow_constructible_v<Ext, ConvertibleToInt<int>>);
        static_assert(!is_nothrow_constructible_v<Ext, ConvertibleToInt<int, IsNothrow::yes, IsExplicit::yes>>);
        static_assert(!is_constructible_v<Ext, NonConvertibleToAnything>);
        static_assert(is_nothrow_constructible_v<Ext, int, ConvertibleToInt<int>>);
        static_assert(!is_nothrow_constructible_v<Ext, int, ConvertibleToInt<int, IsNothrow::yes, IsExplicit::yes>>);
        static_assert(!is_constructible_v<Ext, int, NonConvertibleToAnything>);
    }

    { // Check construction from types that may throw during conversion to index_type
        using Ext = extents<long long, 4, dynamic_extent>;
        static_assert(!is_constructible_v<Ext, ConvertibleToInt<int, IsNothrow::no>>);
        static_assert(!is_constructible_v<Ext, int, ConvertibleToInt<int, IsNothrow::no>>);
    }

    { // Check postconditions when 'sizeof...(pack) == rank()'
        using Ext = extents<int, dynamic_extent, dynamic_extent, 4>;
        Ext ext{4, ConvertibleToInt<int>{.val = 4}, 4};
        assert(ext.extent(0) == 4);
        assert(ext.extent(1) == 4);
        assert(ext.extent(2) == 4);
        Ext ext2{4, 4, 4};
        assert(ext == ext2);
    }

    { // Check postconditions when 'sizeof...(pack) == rank_dynamic()'
        using Ext = extents<long long, dynamic_extent, dynamic_extent, 5>;
        Ext ext{3, ConvertibleToInt<int>{.val = 3}};
        assert(ext.extent(0) == 3);
        assert(ext.extent(1) == 3);
        assert(ext.extent(2) == 5);
        Ext ext2{3, 3};
        assert(ext == ext2);
    }

    { // Check that elements from pack are passed through 'move'
        using Ext = dextents<signed char, 3>;
        struct FancyIndex {
            constexpr operator integral auto() const& noexcept {
                return 3;
            }

            constexpr operator integral auto() const&& noexcept {
                return 3;
            }

            constexpr operator Ext::index_type() && noexcept {
                return 4;
            }
        };

        FancyIndex i;
        Ext ext{FancyIndex{}, i, as_const(i)};
        assert(ext.extent(0) == 4);
        assert(ext.extent(1) == 4);
        assert(ext.extent(2) == 4);
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
    { // Check construction from array/span where 'size()' is equal to 'rank()' and OtherIndexType models 'integral'
        using Ext           = extents<short, 1, dynamic_extent>;
        array<short, 2> arr = {1, 5};
        Ext ext{arr};
        assert(ext.extent(0) == 1);
        assert(ext.extent(1) == 5);

        Ext ext2{span{arr}};
        assert(ext == ext2);

        static_assert(is_nothrow_constructible_v<Ext, decltype(arr)>);
        static_assert(is_nothrow_constructible_v<Ext, span<int, 2>>);
    }

    { // Check construction from array/span where 'size()' is equal to 'rank()' and OtherIndexType is class type
        using Ext = extents<signed char, 3, dynamic_extent>;
        array<ConvertibleToInt<int>, 2> arr{{{.val = 3}, {.val = 5}}};
        Ext ext{arr};
        assert(ext.extent(0) == 3);
        assert(ext.extent(1) == 5);

        const span s{arr};
        Ext ext2{s};
        assert(ext == ext2);

        static_assert(is_nothrow_constructible_v<Ext, decltype(arr)>);
        static_assert(is_nothrow_constructible_v<Ext, decltype(s)>);
    }

    { // Check construction from array/span where 'size()' is equal to 'rank()' and OtherIndexType is "special"
        using Ext = extents<unsigned int, dynamic_extent, 5>;
        struct SpecialIndex {
            constexpr operator integral auto() noexcept {
                return 3;
            }

            constexpr operator integral auto() const noexcept {
                return 5;
            }
        };

        // Elements of 'arr' and 's' should be passed through 'as_const'
        array<SpecialIndex, 2> arr;
        Ext ext{arr};
        assert(ext.extent(0) == 5);
        assert(ext.extent(1) == 5);

        span s{arr};
        Ext ext2{s};
        assert(ext == ext2);
    }

    { // Check invalid construction from array/span where 'size()' is equal to 'rank()'
        using Ext = extents<long long, 3, dynamic_extent>;
        static_assert(!is_constructible_v<Ext, array<ConvertibleToInt<int, IsNothrow::yes, IsExplicit::yes>, 2>>);
        static_assert(!is_constructible_v<Ext, span<ConvertibleToInt<int, IsNothrow::yes, IsExplicit::yes>, 2>>);
        static_assert(!is_constructible_v<Ext, array<ConvertibleToInt<int, IsNothrow::no>, 2>>);
        static_assert(!is_constructible_v<Ext, span<ConvertibleToInt<int, IsNothrow::no>, 2>>);
        static_assert(!is_constructible_v<Ext, array<NonConstConvertibleToInt<int>, 2>>);
        static_assert(!is_constructible_v<Ext, span<NonConstConvertibleToInt<int>, 2>>);
        static_assert(!is_constructible_v<Ext, array<NonConvertibleToAnything, 2>>);
        static_assert(!is_constructible_v<Ext, span<NonConvertibleToAnything, 2>>);
    }

    { // Check construction from array/span where 'size()' is equal to 'rank_dynamic()' and OtherIndexType models
      // 'integral'
        using Ext         = extents<unsigned char, 3, dynamic_extent, 3, dynamic_extent>;
        array<int, 2> arr = {4, 4};
        Ext ext{arr};
        assert(ext.extent(0) == 3);
        assert(ext.extent(1) == 4);
        assert(ext.extent(2) == 3);
        assert(ext.extent(3) == 4);

        Ext ext2{span{arr}};
        assert(ext == ext2);

        static_assert(is_nothrow_constructible_v<Ext, decltype(arr)>);
        static_assert(is_nothrow_constructible_v<Ext, span<int, 2>>);
    }

    { // Check construction from array/span where 'size()' is equal to 'rank_dynamic()' and OtherIndexType is class type
        using Ext = extents<unsigned short, 3, dynamic_extent, 3, dynamic_extent>;
        array<ConvertibleToInt<unsigned short>, 2> arr{{{.val = 2}, {.val = 2}}};
        Ext ext{arr};
        assert(ext.extent(0) == 3);
        assert(ext.extent(1) == 2);
        assert(ext.extent(2) == 3);
        assert(ext.extent(3) == 2);

        span s{arr};
        Ext ext2{s};
        assert(ext == ext2);

        static_assert(is_nothrow_constructible_v<Ext, decltype(arr)>);
        static_assert(is_nothrow_constructible_v<Ext, decltype(s)>);
    }

    { // Check construction from array/span where 'size()' is equal to 'rank_dynamic()' and OtherIndexType is "special"
        using Ext = extents<unsigned int, dynamic_extent, dynamic_extent, 3>;
        struct SpecialIndex {
            constexpr operator integral auto() noexcept {
                return 5;
            }

            constexpr operator integral auto() const noexcept {
                return 3;
            }
        };

        // Elements of 'arr' and 's' should be passed through 'as_const'
        array<SpecialIndex, 2> arr;
        Ext ext{arr};
        assert(ext.extent(0) == 3);
        assert(ext.extent(1) == 3);
        assert(ext.extent(2) == 3);

        span s{arr};
        Ext ext2{s};
        assert(ext == ext2);
    }

    { // Check invalid construction from array/span where 'size()' is equal to 'rank_dynamic()'
        using Ext = extents<unsigned char, dynamic_extent, 5, dynamic_extent, 5>;
        static_assert(!is_constructible_v<Ext, array<ConvertibleToInt<int, IsNothrow::yes, IsExplicit::yes>, 2>>);
        static_assert(!is_constructible_v<Ext, span<ConvertibleToInt<int, IsNothrow::yes, IsExplicit::yes>, 2>>);
        static_assert(!is_constructible_v<Ext, array<ConvertibleToInt<int, IsNothrow::no>, 2>>);
        static_assert(!is_constructible_v<Ext, span<ConvertibleToInt<int, IsNothrow::no>, 2>>);
        static_assert(!is_constructible_v<Ext, array<NonConstConvertibleToInt<int>, 2>>);
        static_assert(!is_constructible_v<Ext, span<NonConstConvertibleToInt<int>, 2>>);
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
        (void) Ext{span{arr}};
    }

    { // Check construction from arrays/spans with invalid size
        using Ext = extents<short, 5, 5>;
        static_assert(!is_constructible_v<Ext, array<int, 1>>);
        static_assert(!is_constructible_v<Ext, array<int, 3>>);
        static_assert(!is_constructible_v<Ext, span<int, 1>>);
        static_assert(!is_constructible_v<Ext, span<int, 3>>);
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

        same_as<bool> decltype(auto) cond = e1 != e2;
        assert(cond);
        assert(e2 != e3);
        assert(e1 == e3);

        static_assert(noexcept(e1 != e2));
        static_assert(noexcept(e1 == e3));
    }

    { // Some extents are static, some dynamic
        extents<int, 3, dynamic_extent> e1{1};
        extents<long, dynamic_extent, 3> e2{2};
        extents<size_t, 2, dynamic_extent> e3{3};

        same_as<bool> decltype(auto) cond = e1 != e2;
        assert(cond);
        assert(e2 == e3);
        assert(e1 != e2);

        static_assert(noexcept(e1 != e2));
        static_assert(noexcept(e2 == e3));
    }

    { // All extents are dynamic
        dextents<int, 2> e1{1, 2};
        dextents<long, 2> e2{1, 2};
        dextents<size_t, 2> e3{1, 3};

        same_as<bool> decltype(auto) cond = e1 == e2;
        assert(cond);
        assert(e2 != e3);
        assert(e1 != e3);

        static_assert(noexcept(e1 == e2));
        static_assert(noexcept(e2 != e3));
    }

    { // Different ranks
        static_assert(extents<int, 2, 3>{} != extents<short, 2>{});
        static_assert(extents<int, 2, 2>{} != extents<long, 2, 3, 5>{});
        static_assert(noexcept(extents<int, 2, 3>{} != extents<short, 2>{}));
        static_assert(noexcept(extents<int, 2, 2>{} != extents<long, 2, 3, 5>{}));
    }
}

template <class... Args>
concept CanDeduceExtents = requires(Args&&... args) { extents{forward<Args>(args)...}; };

template <class T, size_t ExpectedRank>
constexpr bool all_extents_dynamic = false;

template <class IndexType, size_t... Extents, size_t ExpectedRank>
constexpr bool all_extents_dynamic<extents<IndexType, Extents...>, ExpectedRank> =
    ((Extents == dynamic_extent) && ...) && (sizeof...(Extents) == ExpectedRank);

constexpr void check_deduction_guide() {
    { // Check 'CanDeduceExtents' concept
        static_assert(CanDeduceExtents<signed char, short, int, long, long long>);
        static_assert(CanDeduceExtents<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>);
        static_assert(CanDeduceExtents<int, ConvertibleToInt<int>, int>);
        static_assert(!CanDeduceExtents<int, ConvertibleToInt<int, IsNothrow::no>, int>);
        static_assert(!CanDeduceExtents<int, NonConvertibleToAnything, int>);
    }

    { // Check correctness
        extents ext{'1', 2, 3u, 4ll, ConvertibleToInt<size_t>{.val = 5}};
        assert(ext.extent(0) == '1');
        assert(ext.extent(1) == 2);
        assert(ext.extent(2) == 3);
        assert(ext.extent(3) == 4);
        assert(ext.extent(4) == 5);

        static_assert(all_extents_dynamic<decltype(ext), 5>);
        static_assert(same_as<decltype(ext)::index_type, size_t>);
    }
}

constexpr bool test() {
    check_members_with_various_extents([]<class IndexType, size_t... Extents>(const extents<IndexType, Extents...>&) {
        check_members<IndexType, Extents...>(make_index_sequence<sizeof...(Extents)>{});
    });
    check_defaulted_default_constructor();
    check_construction_from_other_extents();
    check_construction_from_extents_pack();
    check_construction_from_array_and_span();
    check_equality_operator();
    check_deduction_guide();

    return true;
}

// Check dextents
static_assert(all_extents_dynamic<dextents<signed char, 0>, 0>);
static_assert(all_extents_dynamic<dextents<short, 1>, 1>);
static_assert(all_extents_dynamic<dextents<int, 2>, 2>);
static_assert(all_extents_dynamic<dextents<long, 3>, 3>);
static_assert(all_extents_dynamic<dextents<long long, 4>, 4>);
static_assert(all_extents_dynamic<dextents<unsigned char, 5>, 5>);
static_assert(all_extents_dynamic<dextents<unsigned short, 6>, 6>);
static_assert(all_extents_dynamic<dextents<unsigned int, 7>, 7>);
static_assert(all_extents_dynamic<dextents<unsigned long, 8>, 8>);
static_assert(all_extents_dynamic<dextents<unsigned long long, 9>, 9>);

int main() {
    static_assert(test());
    test();
}
